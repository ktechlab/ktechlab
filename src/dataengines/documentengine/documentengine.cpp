#include "documentengine.h"
#include "shell/core.h"
#include "interfaces/idocumentplugin.h"

#include <interfaces/iplugin.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/idocumentcontroller.h>

#include <KDebug>
#include <Plasma/DataEngineManager>


DocumentEngine::DocumentEngine( QObject* parent, const QVariantList& args )
    :   Plasma::DataEngine( parent, args ),
        m_core( KTechLab::Core::self() ),
        m_disabled( false )
{
    if ( !m_core ) {
        kError() << "Please only do use this DataEngine from a started KTechLab session." << endl;
        m_disabled = true;
    }
}

DocumentEngine::~DocumentEngine()
{
}

QStringList DocumentEngine::sources() const
{
    return QStringList() << QString("opened");
}

void DocumentEngine::init()
{
}

bool DocumentEngine::sourceRequestEvent( const QString &name )
{
    return updateSourceEvent( name );
}

bool DocumentEngine::updateSourceEvent( const QString &source )
{
    if ( m_disabled ) {
        return false;
    }

    const KDevelop::IDocumentController *docController = m_core->documentController();
    QList<KDevelop::IDocument*> docList = docController->openDocuments();
    QStringList urlList;
    KDevelop::IDocument *document = 0;
    foreach (KDevelop::IDocument *doc, docList) {
        urlList << doc->url().prettyUrl();
        // check if this document is requested as source, store pointer
        if ( source.startsWith( doc->url().prettyUrl() ) ) {
            document = doc;
        }
    }

    //information about opened documents requested
    if ( source == I18N_NOOP("opened") ) {
        setData( source,
                 I18N_NOOP("documentCount"),
                 QString::number( docList.count() )
        );
        setData( source, I18N_NOOP("documentList"), urlList );
        if ( docController->activeDocument() ) {
            setData( source,
                     I18N_NOOP("active"),
                     docController->activeDocument()->url().prettyUrl()
            );
        }

        return true;
    }
    //get a plugin to provide a DataSource for this document type
    QStringList constraints;
    constraints << QString("'%1' in [X-KDevelop-SupportedMimeTypes]").arg(
            document->mimeType()->name() );
    QList<KDevelop::IPlugin*> plugins =
            m_core->pluginController()->allPluginsForExtension( "KTLDocument", constraints );
    if ( plugins.isEmpty() ) {
        return false;
    }
    //there should be only one plugin, so only try to load the first one
    KTechLab::IDocumentPlugin *plugin =
            qobject_cast<KTechLab::IDocumentPlugin*>( plugins.first() );

    // specific document is chosen
    if ( document ) {
        QString component = source;
        component.remove( document->url().prettyUrl() );
        if ( component.startsWith("/") ) {
            component.remove(0,1);
        }
        addSource( plugin->createDataContainer( document, component ) );

        return true;
    }
    // handle components
    if ( source.startsWith( I18N_NOOP("component/") ) ) {
        addSource( plugin->createComponentContainer( source ) );
    }

    return false;
}

K_EXPORT_PLASMA_DATAENGINE(ktechlabdocument, DocumentEngine)

#include "documentengine.moc"

// vim: sw=4 sts=4 et tw=100
