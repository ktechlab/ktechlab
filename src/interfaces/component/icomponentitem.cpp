/*
    Copyright (C) 2009-2010 Julian Bäume <julian@svg4all.de>

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

#include "icomponentitem.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

using namespace KTechLab;

IComponentItem::IComponentItem(QGraphicsItem* parentItem)
    : QGraphicsSvgItem(parentItem)
{
    setAcceptHoverEvents(true);
    setFlags(
        ItemIsFocusable | ItemIsSelectable |
        ItemIsMovable | ItemSendsScenePositionChanges
    );
}

IComponentItem::~IComponentItem()
{

}

void IComponentItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if (   event->button() == Qt::LeftButton
        && contains(event->scenePos())
        && contains(event->buttonDownScenePos(Qt::LeftButton)) ){
        if (event->modifiers() != Qt::ControlModifier)
            scene()->clearSelection();
        setSelected(true);
        event->accept();
    }
    QGraphicsSvgItem::mouseReleaseEvent(event);
}
