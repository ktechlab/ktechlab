/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "componentitem.h"

#include <QVariantMap>
#include <KDebug>

using namespace KTechLab;

ComponentItem::ComponentItem( QObject *parent, Theme *theme, const QVariantList &args )
  : m_theme( theme )
{
    setAcceptDrops( false );
}

void ComponentItem::paintInterface( QPainter *p, const QStyleOptionGraphicsItem *option, const QRect &contentsRect )
{
//    m_icon.paint(p, QPointF(0,0));
}

void ComponentItem::dataUpdated( const QString &name, const QVariantMap &data )
{
    kDebug() << "been here!";
    if ( data["mime"].toString().endsWith("component") ) {
        m_itemData = data[ "item" ].toMap();

//        m_icon.setImagePath( imagePathForComponent( m_itemData ) );

        //updated component, so repaint
        update();
    }
}

QString ComponentItem::imagePathForComponent( const QVariantMap &map ) const
{
/*    if ( !containment() ) {
        kWarning() << "Couldn't get the applets containment";
        return m_theme ? m_theme->imagePath( "ktechlab/components/unknown" ) : "";
    }
    QString componentTheme = containment()->config( "circuit" ).readEntry( "componentTheme", "din" );
    QString path = m_theme ? m_theme->imagePath( "ktechlab/components/"+componentTheme+"/"+map[ "type" ].toString().replace("/","_") ) : "";
    if ( path.isEmpty() ) {
        //default to unknown component
        path = m_theme ? m_theme->imagePath( "ktechlab/components/unknown" ) : "";
    }
    kDebug()<< "finding path for" << "ktechlab/components/"+componentTheme+"/"+map[ "type" ].toString().replace("/","_")
            << path;
    return path;*/
}

// vim: sw=4 sts=4 et tw=100
