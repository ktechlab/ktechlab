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
	/* TODO add verification about: 1 .circuit file extension is defined on the
	 * current user; if there are more, then also print the search directories
	 * and say that the user might experience problems
	 */
	/*
	 * TODO update documentation on the wiki, about setting up ktechlab
	 */
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


#include <ktexteditor/document.h>
#include <ktexteditor/view.h>

#include <kencodingfiledialog.h>
#include <interfaces/idocument.h>
#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/iprojectcontroller.h>
#include <kparts/mainwindow.h>

#include <shell/documentcontroller.h>

#include "shell/textdocument.h"

using namespace KDevelop;

#define EMPTY_DOCUMENT_URL i18n("Untitled")

static
bool isEmptyDocumentUrl(const KUrl &url)
{
    QRegExp r(QString("^%1(\\s\\(\\d+\\))?$").arg(EMPTY_DOCUMENT_URL));
    return r.indexIn(url.prettyUrl()) != -1;
}


static
IDocument* openDocumentInternal(const KUrl & inputUrl, const QString& prefName = QString(),
        const KTextEditor::Range& range = KTextEditor::Range::invalid(), const QString& encoding = "",
        DocumentController::DocumentActivationParams activationParams = 0,
        IDocument* buddy = 0)
    {
		IDocumentController *controller = Core::self()->documentController();
        IDocument* previousActiveDocument = controller->activeDocument();
        KTextEditor::Cursor previousActivePosition;
        if(previousActiveDocument && previousActiveDocument->textDocument() && previousActiveDocument->textDocument()->activeView())
            previousActivePosition = previousActiveDocument->textDocument()->activeView()->cursorPosition();


        QString _encoding = encoding;

        KUrl url = inputUrl;

        if ( url.isEmpty() && (!activationParams.testFlag(IDocumentController::DoNotCreateView)) )
        {
            KUrl dir;
            if( controller->activeDocument() )
            {
                dir = controller->activeDocument()->url().upUrl();
            }else
            {
                dir = KGlobal::config()->group("Open File")
					.readEntry( "Last Open File Directory",
								Core::self()->projectController()->projectsBaseDirectory() );
            }

            KEncodingFileDialog::Result res = KEncodingFileDialog::getOpenUrlAndEncoding( "", dir.url(),
												i18n( "*|Text File\n" ),
												Core::self()->uiController()->activeMainWindow()->widget(),
												// Core::self()->uiControllerInternal()->defaultMainWindow(),
												i18n( "Open File" ) );
            if( !res.URLs.isEmpty() )
                url = res.URLs.first();
            _encoding = res.encoding;
        }
        if ( url.isEmpty() )
            //still no url
            return 0;

        KGlobal::config()->group("Open File").writeEntry( "Last Open File Directory", url.upUrl() );

        // clean it and resolve possible symlink
        url.cleanPath( KUrl::SimplifyDirSeparators );
        if ( url.isLocalFile() )
        {
            QString path = QFileInfo( url.toLocalFile() ).canonicalFilePath();
            if ( !path.isEmpty() )
                url.setPath( path );
        }

        //get a part document
        IDocument* doc=0;
        Q_FOREACH(IDocument *openedDoc, Core::self()->documentController()->openDocuments()){
			if(openedDoc->url() == url){
				doc = openedDoc;
			}
		}
        // if (documents.contains(url))
        //     doc=documents.value(url);
        // else
        if(!doc) // not found
        {
            KMimeType::Ptr mimeType;

            if (/* DocumentController:: */ isEmptyDocumentUrl(url))
            {
                mimeType = KMimeType::mimeType("text/plain");
            }
            else
            {
                //make sure the URL exists
                if ( !url.isValid() || !KIO::NetAccess::exists( url, KIO::NetAccess::DestinationSide, 0 ) )
                {
                    kDebug() << "cannot find URL:" << url.url();
                    return 0;
                }

                mimeType = KMimeType::findByUrl( url );

                if( !url.isLocalFile() && mimeType->isDefault() )
                {
                    // fall back to text/plain, for remote files without extension, i.e. COPYING, LICENSE, ...
                    // using a syncronous KIO::MimetypeJob is hazardous and may lead to repeated calls to
                    // this function without it having returned in the first place
                    // and this function is *not* reentrant, see assert below:
                    // Q_ASSERT(!documents.contains(url) || documents[url]==doc);
                    mimeType = KMimeType::mimeType("text/plain");
                }
            }

            // is the URL pointing to a directory?
            if ( mimeType->is( "inode/directory" ) )
            {
                kDebug() << "cannot open directory:" << url.url();
                return 0;
            }

            if( prefName.isEmpty() )
            {
                // Try to find a plugin that handles this mimetype
                QString constraint = QString("'%1' in [X-KDevelop-SupportedMimeTypes]").arg(mimeType->name());
                KPluginInfo::List plugins = IPluginController::queryPlugins( constraint );

                if( !plugins.isEmpty() )
                {
                    KPluginInfo info = plugins.first();
                    Core::self()->pluginController()->loadPlugin( info.pluginName() );
                }
            }

            IDocumentFactory *factory = KDevelop::Core::self()->documentController()->factory( mimeType->name() );
            if( factory )
            // if( factories.contains( mimeType->name() ) )
            {
				doc = factory->create(url, Core::self());
                // doc = factories[mimeType->name()]->create(url, Core::self());
            }

            if(!doc) {
                if( !prefName.isEmpty() )
                {
                    doc = new PartDocument(url, Core::self(), prefName);
//                 }
//                 else  if ( Core::self()->partControllerInternal()->isTextType(mimeType))
//                 {
//                     doc = new TextDocument(url, Core::self(), _encoding);
//                 }
//                 else if( Core::self()->partControllerInternal()->canCreatePart(url) )
//                 {
//                     doc = new PartDocument(url, Core::self());
                } else
                {
                    int openAsText = KMessageBox::questionYesNo(0, i18n("KDevelop could not find the editor for file '%1' of type %2.\nDo you want to open it as plain text?", url.fileName(), mimeType->name()), i18n("Could Not Find Editor"));
                    if (openAsText == KMessageBox::Yes)
                        doc = new TextDocument(url, Core::self(), _encoding);
                    else
                        return 0;
                }
            }
        }

        // The url in the document must equal the current url, else the housekeeping will get broken
        Q_ASSERT(!doc || doc->url() == url);

        if(doc) // && openDocumentInternal(doc, range, activationParams, buddy))
		{
			Core::self()->documentController()->openDocument(doc, range, activationParams, buddy);
            return doc;
		}
        else
            return 0;

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
