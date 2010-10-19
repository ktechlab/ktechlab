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
#include "../idocumentscene.h"
#include "idocumentitem.h"

namespace KTechLab {

class IDocumentScene;
class Connector;
class Node;

/**
 * \short Small class to implement mouse-interaction for Connectors
 * This will handle some basic interaction with the user.
 * It is also used to provide access to the underlying data-structures
 * and keep track of other \ref QGraphicItem instances used to
 * define this connection.
 */
class KTLINTERFACES_EXPORT ConnectorItem : public QGraphicsPathItem, public IDocumentItem
{
public:
    /**
     * Construct a new connector within the given scene. The scene is
     * mandatory, because this is where all routing takes place and the
     * ConnectorItem needs information from the scene like the \ref Node
     * instances it is connected to.
     *
     * \param scene - the scene, that belongs to the document, the Connector
     * is used in
     * \param parent - a parent item, used as described in \ref QGraphicsScene
     * documentation
     */
    ConnectorItem(IDocumentScene* scene, QGraphicsItem* parent = 0);
    /**
     * Construct a new connector within the given scene. The connector data
     * is used to create an instance of the internal representation of the connection.
     *
     * See \ref ConnectorItem(IDocumentScene*, QGraphicsItem*)
     */
    ConnectorItem(const QVariantMap& connectorData, IDocumentScene* scene, QGraphicsItem* parent = 0);
    virtual ~ConnectorItem();

    /**
     * Set the node, where the connector starts.
     */
    void setStartNode(const Node* node);
    /**
     * Set the node, where the connector ends.
     */
    void setEndNode(const Node* node);
    /**
     * Get the node, where the connector starts.
     */
    const Node* startNode() const;
    /**
     * Get the node, where the connector ends.
     */
    const Node* endNode() const;

    enum { Type = KTechLab::GraphicsItems::ConnectorItemType };
    virtual int type() const { return Type; };

    /**
     * Get internal data to store it to disk
     */
    virtual QVariantMap data() const;

    /**
     * Set the route for this ConnectorItem. Don't use QGraphicsPathItem::setPath()!
     */
    void setRoute(const QPainterPath& route);
    /* TODO: make this class inherit QGraphicsItem and only store a QGraphicsPathItem interally */
    /**
     * Get the route for this ConnectorItem
     */
    QPainterPath route() const;

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private:
    void init();
    class ConnectorItemPrivate;
    ConnectorItemPrivate *d;
    Connector *m_connector;
    IDocumentScene* m_scene;
};

}
#endif // CONNECTORITEM_H
