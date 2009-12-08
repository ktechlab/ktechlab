/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "componentitem.h"
#include "theme.h"

#include <QVariantMap>
#include <QSvgRenderer>
#include <KDebug>

using namespace KTechLab;

ComponentItem::ComponentItem ( const QVariantMap& data, Theme *theme, QGraphicsItem* parentItem )
    : QGraphicsSvgItem ( parentItem ),
      m_renderer( new QSvgRenderer() ),
      m_theme( theme )
{
    setAcceptHoverEvents(true);
    // may be the renderer should provided by the Theme. Then different renderers for
    // the same component can be shared
    m_renderer->load( m_theme->findFirstFile( data.value("fileName").toString() ) );
    setSharedRenderer(m_renderer);
    // move to position in the scene.
    QPoint pos(data.value("x").toInt(),data.value("y").toInt());
    // coordinates from circuit file represent the center
    // of the component, so we have to move it -QPoint(32,32)
    // to compensate the SVG viewBox and -QPoint(4,4) to compensate
    // the raster of the kde3 version
    pos -= QPoint(36,36);
    setPos( pos );
}

ComponentItem::~ComponentItem()
{
    delete m_renderer;
}

void ComponentItem::dataUpdated( const QString &name, const QVariantMap &data )
{
    kDebug() << "been here!";
    //updated component, so repaint
    update();
}

// vim: sw=4 sts=4 et tw=100
