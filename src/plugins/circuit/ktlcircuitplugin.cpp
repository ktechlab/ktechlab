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
#include <interfaces/simulator/isimulationmanager.h>

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
    m_componentViewFactory = new KTLComponentViewFactory(this);
    KDevelop::Core::self()->uiController()->addToolView( i18n("Components"), m_componentViewFactory );

    m_componentEditorFactory = new KTLComponentEditorFactory(this);
    KDevelop::Core::self()->uiController()->addToolView( i18n("Component Editor"), m_componentEditorFactory );

    m_documentFactory = new KTLCircuitDocumentFactory(this);
    KDevelop::Core::self()->documentController()->registerDocumentForMimetype( "application/x-circuit", m_documentFactory );

    m_fakeComponentItemFactory = new FakeComponentItemFactory;
    registerComponentFactory(m_fakeComponentItemFactory);
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

void KTLCircuitPlugin::newCircuitFile()
{
	qDebug() << "KTLCircuitPlugin::newCircuitFile() activated\n";

	// get a temporary file name
	QTemporaryFile tmpFile(QDir::tempPath().append(QDir::separator())
		.append("ktlXXXXXX.circuit"));
	tmpFile.setAutoRemove(false);
	tmpFile.open();
	qDebug() << "creating temporary file: " << tmpFile.fileName()
		<< "pattern: " << tmpFile.fileTemplate();
	// write a minial circuit document into the temporary file
	tmpFile.write("<!DOCTYPE KTechlab>\n"
				"<document type=\"circuit\" >"
				"</document>"
				);
	tmpFile.close();
	KUrl url(tmpFile.fileName());
	core()->documentController()->openDocument(url, "ktlcircuit");
}

void KTLCircuitPlugin::printSimulationManagerStatus()
{
	ISimulationManager *sim = ISimulationManager::self();
	kDebug() << "Registered document mime types:";
	kDebug() << sim->registeredDocumentMimeTypeNames();
	kDebug() << "Simulation types:";
	kDebug() << sim->registeredSimulationTypes();
}

#define EMPTY_DOCUMENT_URL i18n("Untitled")

static
bool isEmptyDocumentUrl(const KUrl &url)
{
    QRegExp r(QString("^%1(\\s\\(\\d+\\))?$").arg(EMPTY_DOCUMENT_URL));
    return r.indexIn(url.prettyUrl()) != -1;
}

void KTLCircuitPlugin::printOpeningRelatedInfo()
{
	QTemporaryFile tmpFile(QDir::tempPath().append(QDir::separator())
		.append("ktlXXXXXX.circuit"));
	tmpFile.setAutoRemove(false);
	tmpFile.open();
	tmpFile.write("<!DOCTYPE KTechlab>\n"
				"<document type=\"circuit\" >"
				"</document>"
				);
	tmpFile.close();
	KUrl url(tmpFile.fileName());

	kDebug() << "url: " << url;
	kDebug() << "isEmptyDocumentURl: " << isEmptyDocumentUrl(url);

	/*
	{
		KMimeType::List allMimes = KMimeType::allMimeTypes();
		foreach(KMimeType::Ptr t, allMimes){
			kDebug() << "all mimes: " << t->name() <<
				", patterns: " << t->patterns();
		}
	}
	*/

	int accuracy = -1;
	// KMimeType::Ptr mimeType = KMimeType::findByUrl( url, 0, false, false, &accuracy ); // bad
	// KMimeType::Ptr mimeType = KMimeType::findByUrl( url, 0, true, false, &accuracy ); // bad
	KMimeType::Ptr mimeType = KMimeType::findByUrl( url, 0, false, true, &accuracy ); // good
	// KMimeType::Ptr mimeType = KMimeType::findByUrl( url ); // bad

	// TODO for working around the bug/feature of KDE, reimpment DocumentController::openDocument(url),
	//  and then call:
	// 	    virtual Q_SCRIPTABLE bool openDocument(IDocument* doc,
	//             const KTextEditor::Range& range = KTextEditor::Range::invalid(),
	//             DocumentActivationParams activationParams = 0,
	//             IDocument* buddy = 0) = 0;
	// feature of KDE: KMimeType not recognizing circuit document type, only in certain cases

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
