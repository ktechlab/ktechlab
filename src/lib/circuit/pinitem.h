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

#ifndef PINITEM_H
#define PINITEM_H

#include "interfaces/component/node.h"
#include "circuitexport.h"

namespace KTechLab
{

class IComponentItem;
class IDocumentScene;

/**
 * A QGraphicsItem used to indicate mouse interaction. When this
 * item is clicked, routing will start from this point in the parent scene.
 */
class CIRCUIT_EXPORT PinItem : public Node
{
public:
    PinItem(const QRectF &rect, IComponentItem* parent = 0, QGraphicsScene* scene = 0);

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);

private:
    bool fetchCircuit();
    IDocumentScene* m_circuit;
};

}

#endif // PINITEM_H
