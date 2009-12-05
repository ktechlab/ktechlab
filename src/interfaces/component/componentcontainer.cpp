#include "componentcontainer.h"

#include <KUrl>

using namespace KTechLab;

ComponentContainer::ComponentContainer( const QString &component )
{
    m_component = component;
    if ( m_component.startsWith( "component/" ) ) {
        m_component.remove( "component/" );
    }
    KUrl url( m_component );
    if ( !url.isLocalFile() ) {
        return;
    }
    setObjectName( "component/" + m_component );
    setData( "url", url );
}
