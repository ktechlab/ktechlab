/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef NODEGROUP_H
#define NODEGROUP_H

#include <QBitArray>
#include <QList>
#include <QObject>
#include <QPointer>

#include "cnitem.h"

class ICNDocument;
class Connector;
class ConRouter;
class Node;
class NodeGroup;

class QTimer;

typedef QList<int> IntList;
typedef QList<NodeGroup *> NodeGroupList;
typedef QList<QPointer<Node>> NodeList;

/**
Controls a group of nodes who are not attached to any CNItems (poor things!)
along with their associated connectors.
@author David Saxton
*/
class NodeGroup : public QObject
{
    Q_OBJECT
public:
    NodeGroup(ICNDocument *icnDocument);
    ~NodeGroup() override;
    /**
     * Adds a node to the group (this checks to make sure that the node is not
     * a child node). If checkSurrouding is true, then surrounding nodes will be
     * checked to see if they are valid for inclusion - and if so, include them.
     */
    void addNode(Node *node, bool checkSurrouding);
    /**
     * Returns the list of internal nodes
     */
    NodeList internalNodeList() const
    {
        return m_nodeList;
    }
    /**
     * Returns the list of external nodes
     */
    NodeList externalNodeList() const
    {
        return m_extNodeList;
    }
    /**
     * Returns the list of connectors
     */
    ConnectorList connectorList() const
    {
        return m_conList;
    }
    /**
     * Translates the routes by the given amount
     */
    void translate(int dx, int dy);
    void init();
    /**
     * @returns true if node is an internal node to this group
     */
    bool contains(Node *node) const
    {
        return m_nodeList.contains(node);
    }
    /**
     * Reroute the NodeGroup. This function should only ever be called by
     * ICNDocument::rerouteInvalidatedConnectors(), as it is important that
     * there is only ever one entity controlling the routing of connectors.
     */
    void updateRoutes();
    /**
     * Sets the visibility of all nodes in the group.
     */
    void setVisible(bool visible);

public slots:
    /**
     * Called when an internal or external node is deleted
     */
    void nodeRemoved(Node *node);
    /**
     * Called when a connector is removed
     */
    void connectorRemoved(Connector *connector);

protected:
    void clearConList();
    /**
     * Finds the common connector between two nodes
     */
    Connector *findCommonConnector(Node *n1, Node *n2);
    /**
     * Find the best pair of nodes in the given list to route between. These
     * will be nodes that give a ncie path (e.g. if they're aligned horizontally
     * or vertically), or otherwise the closest such pair. The two nodes will be
     * returned in n1 and n2.
     */
    void findBestPair(NodeList *list, Node **n1, Node **n2);
    /**
     * Finds the nodes along the route with the given start and end nodes (which
     * will be unique). The end nodes are not included in the returned list.
     */
    NodeList findRoute(Node *startNode, Node *endNode);

    ConnectorList m_conList;
    NodeList m_nodeList;
    NodeList m_extNodeList;
    ICNDocument *p_icnDocument;
    QBitArray b_routedMap; // Routes between different nodes
    bool b_visible;

private:
    IntList findRoute(IntList used, int currentNode, int endNode, bool *success = nullptr);
    void resetRoutedMap();
    /**
     * Looks at b_routedMap as well as the connectors coming out of nodes, and
     * removes the nodes from the given list that have all of their connectors
     * routed.
     */
    void removeRoutedNodes(NodeList *nodes, Node *n1, Node *n2);
    void addExtNode(Node *node);
    /**
     * Looks at b_mappedRoute to see if there is a completely unrouted set of
     * connectors between the two given nodes;
     */
    bool canRoute(Node *n1, Node *n2);
    void getReachable(IntList *reachable, int node);
    /**
     * Either: position of node in m_nodeList,
     * or: (position of node in m_extNodeList) + m_nodeList.size()
     * or: -1
     */
    int getNodePos(Node *n);
    /**
     * Essentially the inverse of getNodePos
     */
    Node *getNodePtr(int n);
};

#endif
