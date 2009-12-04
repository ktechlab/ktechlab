#include "componentcontainer.h"

#include <KUrl>

using namespace KTechLab;

ComponentContainer::ComponentContainer( const QString &component )
{
    m_component = component;
    if ( m_component.startsWith( I18N_NOOP("component/") ) ) {
        m_component.remove( I18N_NOOP("component/") );
    }
    KUrl url( m_component );
    if ( !url.isLocalFile() ) {
        return;
    }
    setObjectName( I18N_NOOP("component/") + m_component );
    setData( I18N_NOOP("url"), url );
}
