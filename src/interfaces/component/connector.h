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

#ifndef KTECHLAB_CONNECTOR_H
#define KTECHLAB_CONNECTOR_H

#include <QtCore/QObject>
#include <QVariantMap>

class QPainterPath;
namespace KTechLab {
class Node;
class ConnectorPrivate;

/**
 * \short Represent a connection between 2 components on a document.
 * A Connector consists of the following data:
 * \li id - unique identifier of the connector
 * \li route - a string representation of a QPainterPath
 * \li end-node-is-child - 1 -> node belongs to component; 0 -> otherwise
 * \li end-node-id - id of the node; only set if end-node-is-child == 0
 * \li end-node-cid - id of the connection; only set if end-node-is-child == 1
 * \li end-node-parent - id of the end-node-cid parent; only set if end-node-is-child == 1
 * \li start-node-is-child - 1 -> node belongs to component; 0 -> otherwise
 * \li start-node-id - id of the node; only set if start-node-is-child == 0
 * \li start-node-cid - id of the connection; only set if start-node-is-child == 1
 * \li start-node-parent - id of the start-node-cid parent; only set if start-node-is-child == 1
 * \li manual-route - 1 -> route is layed out manually; 0 -> otherwise
 * \li (router - name of the routing plugin that created the route)
 */
class Connector : public QObject
{
    Q_OBJECT
public:
    /**
     * Create a new instance given the connector data.
     */
    Connector(QObject* parent = 0);
    /**
     * Copy constructor
     */
    Connector(const Connector& connector);
    virtual ~Connector();

    /**
     * Check the connection to a given node.
     *
     * \param node - check if this connector connects to this node
     * \returns true, if the given node is either start- or end-node
     */
    bool connectsTo(const Node* node) const;

    /**
     * Set the path to a specific route
     */
    void setRoute(const QPainterPath& route);
    /**
    * Set the node, where the route starts
    */
    void setStartNode(const Node* node);
    /**
     * Get information about the start node. Keep in mind,
     * that this only contains data about the document, not
     * the scene.
     *
     * \returns 0 if start node isn't set up, yet. the node otherwise
     */
    const Node* startNode() const;
    /**
    * Set the node, where the route ends
    */
    void setEndNode(const Node* node);
    /**
     * Get information about the end node. Keep in mind,
     * that this only contains data about the document, not
     * the scene.
     *
     * \returns 0 if start node isn't set up, yet. the node otherwise
     */
    const Node* endNode() const;
    /**
     * Get a path representing the route of the connector
     */
    QPainterPath route() const;
    /**
     * Get connector data as a \class QVariantMap
     */
    QVariantMap data() const;

private:
    ConnectorPrivate *d;
};

}

#endif // KTECHLAB_CONNECTOR_H
