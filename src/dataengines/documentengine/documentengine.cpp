#include "documentengine.h"
#include "shell/core.h"
#include "shell/documentcontroller.h"

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
        if ( source == doc->url().prettyUrl() ) {
            document = doc;
        }
    }

    if ( source == I18N_NOOP("opened") ) {
        setData( source, I18N_NOOP("documentCount"), QString::number( docList.count() ) );
        setData( source, I18N_NOOP("documentList"), urlList );

        return true;
    }
    // specific document is chosen
    if ( document ) {
        setData( source, I18N_NOOP("available"), true );
        setData( source, I18N_NOOP("mime"), document->mimeType()->name() );
        //setData( source, );
        return true;
    }

    return false;
}

K_EXPORT_PLASMA_DATAENGINE(ktechlabdocument, DocumentEngine)

#include "documentengine.moc"

// vim: sw=4 sts=4 et tw=100
