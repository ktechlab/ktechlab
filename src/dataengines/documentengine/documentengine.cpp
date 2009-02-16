#include "documentengine.h"
#include "shell/core.h"
#include "shell/uicontroller.h"
#include "shell/mainwindow.h"

#include <KDebug>


DocumentEngine::DocumentEngine( QObject* parent, const QVariantList& args )
    : Plasma::DataEngine( parent, args )
{
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
    KTechLab::Core *core = KTechLab::Core::self();
    QString foo = core->uiControllerInternal()->defaultMainWindow()->objectName();
    kDebug() << source << endl;
    kDebug() << foo << endl;
    if ( source == I18N_NOOP("opened") ) {
        setData( source, I18N_NOOP("test"), QVariant(i18n("Blair") ));
        setData( source, I18N_NOOP("test2"), QVariant(foo) );
        return true;
    }

    return false;
}

K_EXPORT_PLASMA_DATAENGINE(ktechlabdocument, DocumentEngine)

#include "documentengine.moc"

// vim: sw=4 sts=4 et tw=100
