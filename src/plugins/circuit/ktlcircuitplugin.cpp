/***************************************************************************
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktlcircuitplugin.h"

#include "circuitdocument.h"
#include "interfaces/component/componentmodel.h"
#include "interfaces/component/icomponentplugin.h"
#include "interfaces/component/icomponent.h"
#include "shell/core.h"
#include "interfaces/iplugincontroller.h"
#include <interfaces/idocumentcontroller.h>

#include <interfaces/iuicontroller.h>
#include <interfaces/idocumentcontroller.h>
#include <KGenericFactory>
#include <KAboutData>
#include <KDebug>
#include <QTreeView>
#include <QHeaderView>
#include "componenteditorview.h"
#include "fakecomponentitemfactory.h"
#include <kactioncollection.h>
#include <kaction.h>
#include <qdir.h>
#include <qtemporaryfile.h>
#include <kmessagebox.h>
#include <kglobal.h>
#include <kmimetype.h>
#include <kstandarddirs.h>
#include <interfaces/simulator/isimulationmanager.h>
#include <qstring.h>
#include <kparts/mainwindow.h>


using namespace KTechLab;

K_PLUGIN_FACTORY(KTLCircuitFactory, registerPlugin<KTLCircuitPlugin>(); )
K_EXPORT_PLUGIN(KTLCircuitFactory(KAboutData("ktlcircuit","ktlcircuit", ki18n("KTechLab Circuits"), "0.1", ki18n("Managing, viewing, manipulating circuit files"), KAboutData::License_LGPL)))

class KTechLab::KTLComponentViewFactory: public KDevelop::IToolViewFactory
{
public:
    KTLComponentViewFactory( KTLCircuitPlugin *plugin )
        : m_plugin(plugin)
    {};

    virtual QWidget * create( QWidget *parent )
    {
        QTreeView *componentView = new QTreeView( parent );
        componentView->setModel( m_plugin->componentModel() );
        componentView->header()->hide();
        componentView->setAcceptDrops( false );
        componentView->setDragEnabled( true );

        return componentView;
    };

    virtual QString id() const
    {
        return "org.ktechlab.ComponentView";
    };

    virtual Qt::DockWidgetArea defaultPosition()
    {
        return Qt::LeftDockWidgetArea;
    };
private:
    KTLCircuitPlugin * m_plugin;
};

class KTechLab::KTLComponentEditorFactory: public KDevelop::IToolViewFactory
{
public:
    KTLComponentEditorFactory( KTLCircuitPlugin *plugin )
        : m_plugin(plugin)
    {};

    virtual QWidget * create( QWidget *parent )
    {
        ComponentEditorView *componentEditor = new ComponentEditorView( parent );

        KDevelop::IDocumentController* docController = m_plugin->core()->documentController();
        componentEditor->activated(docController->activeDocument());
        QObject::connect( docController, SIGNAL( documentActivated( KDevelop::IDocument* ) ),
                          componentEditor, SLOT( activated( KDevelop::IDocument* ) ) );

        return componentEditor;
    };

    virtual QString id() const
    {
        return "org.ktechlab.ComponentEditor";
    };

    virtual Qt::DockWidgetArea defaultPosition()
    {
        return Qt::RightDockWidgetArea;
    };
private:
    KTLCircuitPlugin * m_plugin;
};

class KTechLab::KTLCircuitDocumentFactory: public KDevelop::IDocumentFactory
{
public:
    KTLCircuitDocumentFactory( KTLCircuitPlugin *plugin )
        : m_plugin(plugin)
    {};

    virtual KDevelop::IDocument * create( const KUrl &url, KDevelop::ICore *core )
    {
        KDevelop::Core *ktlCore = dynamic_cast<KDevelop::Core*>(core);
        if ( ktlCore )
            return new CircuitDocument( url, ktlCore );

        return 0;
    }
private:
    KTLCircuitPlugin * m_plugin;
};

KTLCircuitPlugin::KTLCircuitPlugin( QObject *parent, const QVariantList& /* args */ )
    : KTechLab::IDocumentPlugin( KTLCircuitFactory::componentData(), parent ),
    m_componentModel( new ComponentModel() )
{

    init();
}

void KTLCircuitPlugin::init()
{
	verifyMimetypeDefinition();

    m_componentViewFactory = new KTLComponentViewFactory(this);
    KDevelop::Core::self()->uiController()->addToolView( i18n("Components"), m_componentViewFactory );

    m_componentEditorFactory = new KTLComponentEditorFactory(this);
    KDevelop::Core::self()->uiController()->addToolView( i18n("Component Editor"), m_componentEditorFactory );

    m_documentFactory = new KTLCircuitDocumentFactory(this);
    KDevelop::Core::self()->documentController()->registerDocumentForMimetype( "application/x-circuit", m_documentFactory );

    m_fakeComponentItemFactory = new FakeComponentItemFactory;
    registerComponentFactory(m_fakeComponentItemFactory);
}

void KTLCircuitPlugin::verifyMimetypeDefinition()
{
	// apparently this code gets called at KDevPlatform shutdown. Ignore.
	if(core()->shuttingDown()){
		kDebug() << "called at core shutdown. Exiting immediately.";
		return;
	}

	QTemporaryFile tmpFile;
	createNewEmptyCircuitFile(tmpFile);
	KUrl url(tmpFile.fileName());
	kDebug() << "test file url: " << url;

	// as used by kdevplatform
	KMimeType::Ptr testMimeType = KMimeType::findByUrl( url );
	kDebug() << "mime type for test file: " << testMimeType->name();
	if(testMimeType->is("application/x-circuit")){
		// looks good, verification success
		tmpFile.remove();
		return;
	}

	// gather some debug information

	kDebug() << "expect problems: circuit file type not properly registered";

	const QStringList globFiles = KGlobal::dirs()->findAllResources(
		"xdgdata-mime", QString::fromLatin1("globs"));
	kDebug() << "Mime glob files are located at: " << globFiles;

	QStringList allRelevantLines;
	Q_FOREACH(QString globFileName, globFiles){
		QStringList relevantLines = readGlobFile(globFileName);
		allRelevantLines.append(relevantLines);
	}
	kDebug() << "Relevant Mime file lines: " << allRelevantLines;

	QString messageText(
		"The KTechLab circuit file type is not properly registered on your system, "
		"and apparently you won't be able to open circuit (.circuit, application/x-circuit) files."
		"Likely this is caused by a configuration problem for file type definitions. "
		"Please find below debug information that might help fixing the problem. "
		"\n\n"
	);

	if(allRelevantLines.size() > 1){
		kDebug() << "Multiple circuit definitions?";
		QString multiDefMsg = QString(
			"The circuit file type might be registered multiple times. "
			"The following relevant lines exist in the file type definions databases:\n %1\n\n"
		).arg(allRelevantLines.join("\n"));
		messageText.append(multiDefMsg);
	}
	if(allRelevantLines.size() == 0){
		kDebug() << "No file definitions?";
		QString noMimetypeMsg = QString(
			"Very likely the circuit file type is not set up, "
			"or the file type database containing it is not in use.\n\n"
		);
		messageText.append(noMimetypeMsg);
	}

	QString databaseLocationMsg = QString(
		"The file type databases in use are at the following locations, "
		"specified by the XDG_DATA_DIRS environment variable at the launch of KTechLab."
		"\n%1.\n"
	).arg(globFiles.join("\n"));
	messageText.append(databaseLocationMsg);

	QWidget *mainWindow = core()->self()->uiController()->activeMainWindow()->widget();

	KMessageBox::error(mainWindow, messageText, "KTechLab");
}

QStringList KTLCircuitPlugin::readGlobFile(QString globFileName) {
	QStringList ret;
	QFile globFile(globFileName);
	if(!globFile.open(QIODevice::ReadOnly)) {
		return ret;
	}
	QTextStream globStream(&globFile);
	while(!globStream.atEnd()) {
		QString line = globStream.readLine();
		if(line.startsWith("#")) {
			continue;
		}
		if(line.contains("application/x-circuit")) {
			ret.append(globFileName.append(": '").append(line).append("'"));
		}
	}
	globFile.close();
	return ret;
}

void KTLCircuitPlugin::createActionsForMainWindow(
	Sublime::MainWindow* /* window */,
	QString& xmlFile,
	KActionCollection& actions)
{
	xmlFile = "ktlcircuitui.rc";

	KIconLoader *loader = KIconLoader::global();

	KAction *newCircuit = actions.addAction("file_new_circuit");
	newCircuit->setText( i18n("New Circuit" ) );
	newCircuit->setIcon( loader->loadIcon( "ktechlab_circuit", KIconLoader::NoGroup, KIconLoader::SizeHuge ) );
	connect(newCircuit, SIGNAL(triggered()), this, SLOT(newCircuitFile()));

	KAction *simulatorStatus = actions.addAction( "help_debug_simulator_status");
	simulatorStatus->setText(i18n("Print simulator manager status"));
	connect(simulatorStatus, SIGNAL(triggered()), this, SLOT(printSimulationManagerStatus()));

	KAction *printOpenRelated = actions.addAction( "help_debug_open_related_info");
	printOpenRelated->setText(i18n("Print opening related info"));
	connect(printOpenRelated, SIGNAL(triggered()), this, SLOT(printOpeningRelatedInfo()));

}

KTLCircuitPlugin::~KTLCircuitPlugin()
{
    // it crashes, when we delete this. I guess,
    // it has been deleted before, somewhere else.
    //delete m_componentViewFactory;
    delete m_fakeComponentItemFactory;
    delete m_documentFactory;
    delete m_componentModel;
}

ComponentModel * KTLCircuitPlugin::componentModel()
{
    return m_componentModel;
}

void KTLCircuitPlugin::registerComponentFactory( IComponentItemFactory *factory )
{
    QList<ComponentMetaData> metaData = factory->allMetaData();
    kDebug() << "registering" << metaData.size() << "components";
    foreach (ComponentMetaData data, metaData) {
        m_componentModel->insertComponentData( data, factory );
    }
}

void KTLCircuitPlugin::deregisterComponentFactory(IComponentItemFactory* factory)
{
    QList<ComponentMetaData> metaData = factory->allMetaData();
    kDebug() << "deregistering" << metaData.size() << "components";
    foreach (ComponentMetaData data, metaData) {
        m_componentModel->removeComponentData( data, factory );
    }
}

IComponentItemFactory* KTLCircuitPlugin::componentItemFactory(const QString& name,
															  Theme* /* theme */ )
{
    IComponentItemFactory* factory = m_componentModel->factoryForComponent(name);
    if (!factory) {
        kWarning() << "factory for data not found";
        return m_componentModel->factoryForComponent("ec/unknown");
    }
    return factory;
}

void KTLCircuitPlugin::unload()
{
    KDevelop::Core::self()->uiController()->removeToolView(m_componentViewFactory);
    KDevelop::Core::self()->uiController()->removeToolView(m_componentEditorFactory);
}

void KTLCircuitPlugin::createNewEmptyCircuitFile(QTemporaryFile& tmpFile)
{
	tmpFile.setFileTemplate(QDir::tempPath().append(QDir::separator())
		.append("ktl-circuit-XXXXXX.circuit"));
	tmpFile.setAutoRemove(false);
	tmpFile.open();
	kDebug() << "creating temporary file: " << tmpFile.fileName()
		<< "pattern: " << tmpFile.fileTemplate();
	// write a minial circuit document into the temporary file
	tmpFile.write("<!DOCTYPE KTechlab>\n"
				"<document type=\"circuit\" >"
				"</document>"
				);
	tmpFile.close();
}

void KTLCircuitPlugin::newCircuitFile()
{
	qDebug() << "KTLCircuitPlugin::newCircuitFile() activated\n";

	// get a temporary file name
	QTemporaryFile tmpFile;
	createNewEmptyCircuitFile(tmpFile);
	KUrl url(tmpFile.fileName());
	kDebug() << "new file url: " << url;
	core()->documentController()->openDocument(url, "");
	// openDocumentInternal(url);
}

void KTLCircuitPlugin::printSimulationManagerStatus()
{
	ISimulationManager *sim = ISimulationManager::self();
	kDebug() << "Registered document mime types:";
	kDebug() << sim->registeredDocumentMimeTypeNames();
	kDebug() << "Simulation types:";
	kDebug() << sim->registeredSimulationTypes();
}


void KTLCircuitPlugin::printOpeningRelatedInfo()
{
	QTemporaryFile tmpFile;
	createNewEmptyCircuitFile(tmpFile);
	KUrl url(tmpFile.fileName());

	kDebug() << "url: " << url;

	int accuracy = -1;
	// KMimeType::Ptr mimeType = KMimeType::findByUrl( url, 0, false, false, &accuracy ); // bad
	// KMimeType::Ptr mimeType = KMimeType::findByUrl( url, 0, true, false, &accuracy ); // bad
	KMimeType::Ptr mimeType = KMimeType::findByUrl( url, 0, false, true, &accuracy ); // good
	// KMimeType::Ptr mimeType = KMimeType::findByUrl( url ); // bad

	// TODO split the debug code in a separate friend class, so the main code will be more clear
	//

	kDebug() << "mime type: " << mimeType->name();
	kDebug() << "mime type accuracy: " << accuracy;

	KDevelop::IDocumentFactory *f = KDevelop::Core::self()->
			documentController()->factory(mimeType->name());
	kDebug() << "factory for mime type: " << f;

	QString constraint = QString("'%1' in [X-KDevelop-SupportedMimeTypes]")
			.arg(mimeType->name());
	KPluginInfo::List plugins = KDevelop::IPluginController::queryPlugins( constraint );

	foreach(KPluginInfo info, plugins){
		kDebug() << "found plugin: " << info.pluginName();
	}
}




#include "ktlcircuitplugin.moc"

// vim: sw=4 sts=4 et tw=100
