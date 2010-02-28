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

using namespace KTechLab;

ConnectorItem::ConnectorItem(QGraphicsItem* parent, QGraphicsScene* scene)
    : QGraphicsPathItem(parent, scene)
{
    setAcceptHoverEvents(true);
}

void ConnectorItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    setPen(QPen(Qt::darkYellow));
}

void ConnectorItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    setPen(QPen(Qt::black));
}

void ConnectorItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mousePressEvent(event);
}

void ConnectorItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

