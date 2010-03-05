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

#ifndef CONNECTORITEM_H
#define CONNECTORITEM_H

#include "../ktlinterfacesexport.h"
#include <QGraphicsPathItem>

namespace KTechLab {
class Connector;

/**
 * \short Small class to implement mouse-interaction for Connectors
 * This will handle some basic interaction with the user.
 */
class KTLINTERFACES_EXPORT ConnectorItem : public QGraphicsPathItem
{
public:
    ConnectorItem(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    ConnectorItem(const Connector& connector, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    ConnectorItem(const QVariantMap& connectorData, QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);
    virtual ~ConnectorItem();

    void setConnector(const Connector& connector);
protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private:
    void init();
    Connector *m_connector;
};

}
#endif // CONNECTORITEM_H
