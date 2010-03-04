/*
    <one line to give the program's name and a brief idea of what it does.>
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

#include "connectoritem.h"
#include <QPen>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOption>
#include <QGraphicsScene>

using namespace KTechLab;

ConnectorItem::ConnectorItem(QGraphicsItem* parent, QGraphicsScene* scene)
    : QGraphicsPathItem(parent, scene)
{
    setAcceptHoverEvents(true);
    setFlag(ItemIsSelectable,true);
    setZValue(-1);
}

void ConnectorItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    if (!isSelected()){
        setPen(QPen(Qt::darkYellow));
        event->accept();
    }
    QGraphicsPathItem::hoverEnterEvent(event);
}

void ConnectorItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    if (!isSelected()){
        setPen(QPen(Qt::black));
        event->accept();
    }
    QGraphicsPathItem::hoverLeaveEvent(event);
}

void ConnectorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton){
        if (event->modifiers() != Qt::ControlModifier)
            scene()->clearSelection();
        setSelected(true);
        event->accept();
    }
}

QVariant ConnectorItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSelectedHasChanged && value.toBool()){
        setPen(QPen(Qt::darkYellow));
    } else if (change == ItemSelectedHasChanged && !value.toBool()){
        setPen(QPen(Qt::black));
    }
    return QGraphicsItem::itemChange(change, value);
}

void KTechLab::ConnectorItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    //This is quite ugly. we copy the whole object, because it's const
    //may be, we should just paint the path on our own.
    QStyleOptionGraphicsItem *cOption = new QStyleOptionGraphicsItem(*option);
    //reset selected state, since we handle painting of selection-related visualisation
    cOption->state &= ~QStyle::State_Selected;

    QGraphicsPathItem::paint(painter, cOption, widget);
    delete cOption;
}
