/*
    Copyright (C) 2010 Julian Bäume <julian@svg4all.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "pinitem.h"
#include "interfaces/component/connectoritem.h"
#include <interfaces/component/icomponentitem.h>

#if KDE_ENABLED
#include <KDebug>
#endif

#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <qdrag.h>

using namespace KTechLab;

PinItem::PinItem(const QRectF& rect, IComponentItem* parent, IDocumentScene* scene)
    : Node(parent, scene),
    m_circuit(scene)
{
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsMovable, false);
    setPos(rect.topLeft());
    setRect(QRectF(QPointF(-rect.width()/2,-rect.height()/2),rect.size()));
    setBrush(QBrush(Qt::SolidPattern));
    //set opacity to nearly 0, because 0 will hide the item
    //and it won't receive any events if hidden
    if (parent){
        this->setOpacity(0.01);
        setParent(parent);
    }
}

void PinItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    this->setOpacity(1);
    event->accept();
    QGraphicsEllipseItem::hoverEnterEvent(event);
}

void PinItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    //set opacity to nearly 0, because 0 will hide the item
    //and it won't receive any events if hidden
    if (parentItem())
        this->setOpacity(0.01);
    event->accept();
    QGraphicsEllipseItem::hoverLeaveEvent(event);
}
