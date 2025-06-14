//
// C++ Implementation: circuiticndocument
//
// Description:
//
//
// Author: Zoltan P <zoltan.padrah@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "circuiticndocument.h"

#include "cnitemgroup.h"
#include "connector.h"
#include "conrouter.h"
#include "ecnode.h"
#include "flowcontainer.h"
#include "item.h"
#include "junctionnode.h"
#include "nodegroup.h"

#include <ktechlab_debug.h>

CircuitICNDocument::CircuitICNDocument(const QString &caption)
    : ICNDocument(caption)
{
}

CircuitICNDocument::~CircuitICNDocument()
{
    // Go to hell, KtlQCanvas. I'm in charge of what gets deleted.
    KtlQCanvasItemList all = m_canvas->allItems();
    const KtlQCanvasItemList::Iterator end = all.end();
    for (KtlQCanvasItemList::Iterator it = all.begin(); it != end; ++it)
        (*it)->setCanvas(nullptr);

    // Remove all items from the canvas
    selectAll();
    deleteSelection();

    // Delete anything that got through the above couple of lines
    ConnectorList connectorsToDelete = m_connectorList;
    connectorsToDelete.clear();
    qDeleteAll(connectorsToDelete);

    deleteAllNodes();
}

void CircuitICNDocument::deleteAllNodes()
{
    const ECNodeMap::iterator nodeListEnd = m_ecNodeList.end();
    for (ECNodeMap::iterator it = m_ecNodeList.begin(); it != nodeListEnd; ++it) {
        qCDebug(KTL_LOG) << "CircuitICNDocument::deleteAllNodes removing [" << it.key() << "] " << it.value();
        // delete *it; // 2015.07.31 - this will not work

        // prevent crash on node destructor
        {
            ECNode *ecNode = it.value();
            // unregisterUID( ecNode->id() ); // do not use, will modify m_ecNodeList
            ICNDocument::unregisterUID(ecNode->id());
            ecNode->setICNDocument(nullptr);
        }

        it.value()->deleteLater();
    }

    m_ecNodeList.clear();
}

bool CircuitICNDocument::canConnect(KtlQCanvasItem *qcanvasItem1, KtlQCanvasItem *qcanvasItem2) const
{
    // Rough outline of what can and can't connect:
    // * At most three connectors to a node
    // * Can't have connectors going between different levels (e.g. can't have
    //   a connector coming outside a FlowContainer from inside).
    // * Can't have more than one route between any two nodes
    // * In all connections between nodes, must have at least one input and one
    //   output node at the ends.

    // nothing special in a circuit; we can connect almost anything
    return ICNDocument::canConnect(qcanvasItem1, qcanvasItem2);
}

Connector *CircuitICNDocument::createConnector(Node *node, Connector *con, const QPoint &pos2, QPointList *pointList)
{
    if (!canConnect(node, con))
        return nullptr;

    // FIXME dynamic_cast used, fix it in Connector class

    ECNode *conStartNode = dynamic_cast<ECNode *>(con->startNode());
    ECNode *conEndNode = dynamic_cast<ECNode *>(con->endNode());
    ECNode *ecNode = dynamic_cast<ECNode *>(node);

    const bool usedManual = con->usesManualPoints();

    ECNode *newNode = new JunctionNode(this, 0, pos2);

    QPointList autoPoints;
    if (!pointList) {
        addAllItemConnectorPoints();
        ConRouter cr(this);
        cr.mapRoute(int(node->x()), int(node->y()), pos2.x(), pos2.y());
        autoPoints = cr.pointList(false);
        pointList = &autoPoints;
    }

    QList<QPointList> oldConPoints = con->splitConnectorPoints(pos2);
    con->hide();

    // The actual new connector
    Connector *new1 = newNode->createConnector(node);
    ecNode->addConnector(new1);
    new1->setRoutePoints(*pointList, usedManual);

    // The two connectors formed from the original one when split
    Connector *new2 = newNode->createConnector(conStartNode);
    conStartNode->addConnector(new2);
    new2->setRoutePoints(oldConPoints.at(0), usedManual);

    Connector *new3 = conEndNode->createConnector(newNode);
    newNode->addConnector(new3);
    new3->setRoutePoints(oldConPoints.at(1), usedManual);

    // Avoid flicker: tell them to update their draw lists now
    con->updateConnectorPoints(false);
    new1->updateDrawList();
    new2->updateDrawList();
    new3->updateDrawList();

    // Now it's safe to remove the connector...
    con->removeConnectorNoArg();
    flushDeleteList();

    deleteNodeGroup(conStartNode);
    deleteNodeGroup(conEndNode);
    createNodeGroup(newNode)->init();

    return new1;
}

Connector *CircuitICNDocument::createConnector(Connector *con1, Connector *con2, const QPoint &pos1, const QPoint &pos2, QPointList *pointList)
{
    if (!canConnect(con1, con2))
        return nullptr;

    const bool con1UsedManual = con1->usesManualPoints();
    const bool con2UsedManual = con2->usesManualPoints();

    QList<QPointList> oldCon1Points = con1->splitConnectorPoints(pos1);
    QList<QPointList> oldCon2Points = con2->splitConnectorPoints(pos2);

    ECNode *node1a = dynamic_cast<ECNode *>(con1->startNode());
    ECNode *node1b = dynamic_cast<ECNode *>(con1->endNode());

    ECNode *node2a = dynamic_cast<ECNode *>(con2->startNode());
    ECNode *node2b = dynamic_cast<ECNode *>(con2->endNode());

    if (!node1a || !node1b || !node2a || !node2b)
        return nullptr;

    con1->hide();
    con2->hide();

    // from this point forward, we are dealing with a circuit document -> all nodes are electronic

    ECNode *newNode1 = new JunctionNode(this, 0, pos1);
    ECNode *newNode2 = new JunctionNode(this, 0, pos2);

    Connector *con1a = newNode1->createConnector(node1a);
    node1a->addConnector(con1a);

    Connector *con1b = newNode1->createConnector(node1b);
    node1b->addConnector(con1b);

    Connector *newCon = newNode1->createConnector(newNode2);
    newNode2->addConnector(newCon);

    Connector *con2a = node2a->createConnector(newNode2);
    newNode2->addConnector(con2a);

    Connector *con2b = node2b->createConnector(newNode2);
    newNode2->addConnector(con2b);

    if (!con1a || !con1b || !con2a || !con2b) {
        // This should never happen, as the canConnect function should strictly
        // determine whether the connectors could be created before hand.
        qCWarning(KTL_LOG) << "Not all the connectors were created, this should never happen";

        if (con1a)
            con1a->removeConnectorNoArg();
        if (con1b)
            con1b->removeConnectorNoArg();
        if (con2a)
            con2a->removeConnectorNoArg();
        if (con2b)
            con2b->removeConnectorNoArg();

        newNode1->removeNode();
        newNode2->removeNode();

        flushDeleteList();
        return nullptr;
    }

    con1a->setRoutePoints(oldCon1Points.at(0), con1UsedManual);
    con1b->setRoutePoints(oldCon1Points.at(1), con1UsedManual);

    con2a->setRoutePoints(oldCon2Points.at(0), con2UsedManual);
    con2b->setRoutePoints(oldCon2Points.at(1), con2UsedManual);

    QPointList autoPoints;

    if (!pointList) {
        addAllItemConnectorPoints();
        ConRouter cr(this);
        cr.mapRoute(pos1.x(), pos1.y(), pos2.x(), pos2.y());
        autoPoints = cr.pointList(false);
        pointList = &autoPoints;
    }

    newCon->setRoutePoints(*pointList, true);

    // Avoid flicker: tell them to update their draw lists now
    con1->updateConnectorPoints(false);
    con2->updateConnectorPoints(false);
    newCon->updateDrawList();
    con1a->updateDrawList();
    con1b->updateDrawList();
    con2a->updateDrawList();
    con2b->updateDrawList();

    // Now it's safe to remove the connectors
    con1->removeConnectorNoArg();
    con2->removeConnectorNoArg();

    flushDeleteList();

    deleteNodeGroup(node1a);
    deleteNodeGroup(node1b);
    deleteNodeGroup(node2a);
    deleteNodeGroup(node2b);

    NodeGroup *ng = createNodeGroup(newNode1);

    ng->addNode(newNode2, true);
    ng->init();

    return newCon;
}

Connector *CircuitICNDocument::createConnector(const QString &startNodeId, const QString &endNodeId, QPointList *pointList)
{
    ECNode *startNode = getEcNodeWithID(startNodeId);
    ECNode *endNode = getEcNodeWithID(endNodeId);

    if (!startNode || !endNode) {
        qCDebug(KTL_LOG) << "Either/both the connector start node and end node could not be found";
        return nullptr;
    }

    if (!canConnect(startNode, endNode))
        return nullptr;

    Connector *connector = endNode->createConnector(startNode);
    if (!connector) {
        qCCritical(KTL_LOG) << "End node did not create the connector";
        return nullptr;
    }

    startNode->addConnector(connector);
    flushDeleteList(); // Delete any connectors that might have been removed by the nodes

    // Set the route to the manual created one if the user created such a route
    if (pointList)
        connector->setRoutePoints(*pointList, true);

    // FIXME WTF is going on here? Redundant/meaningless code?
    //	ConnectorList connectorList;
    //	connectorList.append(connector);

    setModified(true);

    requestRerouteInvalidatedConnectors();
    return connector;
}

Node *CircuitICNDocument::nodeWithID(const QString &id)
{
    if (m_ecNodeList.contains(id))
        return m_ecNodeList[id];
    else
        return nullptr;
}

ECNode *CircuitICNDocument::getEcNodeWithID(const QString &id)
{
    if (m_ecNodeList.contains(id))
        return m_ecNodeList[id];
    else
        return nullptr;
}

void CircuitICNDocument::slotAssignNodeGroups()
{
    ICNDocument::slotAssignNodeGroups();

    const ECNodeMap::iterator end = m_ecNodeList.end();
    for (ECNodeMap::iterator it = m_ecNodeList.begin(); it != end; ++it) {
        NodeGroup *ng = createNodeGroup(*it);
        if (ng)
            ng->init();
    }

    // We've destroyed the old node groups, so any collapsed flowcontainers
    // containing new node groups need to update them to make them invisible.
    const ItemMap::const_iterator itemListEnd = m_itemList.end();
    for (ItemMap::const_iterator it = m_itemList.begin(); it != itemListEnd; ++it) {
        if (FlowContainer *fc = dynamic_cast<FlowContainer *>(*it))
            fc->updateContainedVisibility();
    }
}

void CircuitICNDocument::flushDeleteList()
{
    // Remove duplicate items in the delete list
    KtlQCanvasItemList::iterator end = m_itemDeleteList.end();
    for (KtlQCanvasItemList::iterator it = m_itemDeleteList.begin(); it != end; ++it) {
        if (*it && (m_itemDeleteList.count(*it) > 1)) {
            *it = nullptr;
        }
    }

    m_itemDeleteList.removeAll(nullptr);

    /* again we're spending time to figure out what special method to call instead of a generic call..*/
    end = m_itemDeleteList.end();
    for (KtlQCanvasItemList::iterator it = m_itemDeleteList.begin(); it != end; ++it) {
        KtlQCanvasItem *qcanvasItem = *it;
        m_selectList->removeQCanvasItem(*it);

        if (Item *item = dynamic_cast<Item *>(qcanvasItem))
            m_itemList.remove(item->id());
        else if (ECNode *node = dynamic_cast<ECNode *>(qcanvasItem))
            m_ecNodeList.remove(node->id());
        else if (Connector *con = dynamic_cast<Connector *>(qcanvasItem))
            m_connectorList.removeAll(con);
        else
            qCCritical(KTL_LOG) << "Unknown qcanvasItem! " << qcanvasItem;

        qcanvasItem->setCanvas(nullptr);

        delete qcanvasItem;
        *it = nullptr;
    }

    // 	// Check connectors for merging
    bool doneJoin = false;
    const ECNodeMap::iterator nlEnd = m_ecNodeList.end();
    for (ECNodeMap::iterator it = m_ecNodeList.begin(); it != nlEnd; ++it) {
        (*it)->removeNullConnectors();
        int conCount = (*it)->connectorList().count();
        if (conCount == 2 && !(*it)->parentItem()) {
            if (joinConnectors(*it))
                doneJoin = true;
        }
    }

    if (doneJoin)
        flushDeleteList();

    requestRerouteInvalidatedConnectors();
}

bool CircuitICNDocument::registerItem(KtlQCanvasItem *qcanvasItem)
{
    if (!qcanvasItem)
        return false;

    if (!ItemDocument::registerItem(qcanvasItem)) {
        if (ECNode *node = dynamic_cast<ECNode *>(qcanvasItem)) {
            m_ecNodeList[node->id()] = node;
            Q_EMIT nodeAdded(static_cast<Node *>(node));
        } else if (Connector *connector = dynamic_cast<Connector *>(qcanvasItem)) {
            m_connectorList.append(connector);
            Q_EMIT connectorAdded(connector);
        } else {
            qCCritical(KTL_LOG) << "Unrecognised item";
            return false;
        }
    }

    requestRerouteInvalidatedConnectors();

    return true;
}

void CircuitICNDocument::unregisterUID(const QString &uid)
{
    m_ecNodeList.remove(uid);
    ICNDocument::unregisterUID(uid);
}

/* this method smells a little funny */
NodeList CircuitICNDocument::nodeList() const
{
    NodeList l;

    ECNodeMap::const_iterator end = m_ecNodeList.end();
    for (ECNodeMap::const_iterator it = m_ecNodeList.begin(); it != end; ++it)
        l << it.value();

    return l;
}

void CircuitICNDocument::selectAllNodes()
{
    const ECNodeMap::iterator nodeEnd = m_ecNodeList.end();
    for (ECNodeMap::iterator nodeIt = m_ecNodeList.begin(); nodeIt != nodeEnd; ++nodeIt) {
        if (*nodeIt)
            select(*nodeIt);
    }
}

bool CircuitICNDocument::joinConnectors(ECNode *node)
{
    // We don't want to destroy the node if it has a parent
    if (node->parentItem())
        return false;

    node->removeNullConnectors();

    // an electronic node can be removed if it has exactly 2 connectors connected to it

    int conCount = node->getAllConnectors().count();
    if (conCount != 2)
        return false;

    Connector *con1, *con2;
    ECNode *startNode = nullptr;
    ECNode *endNode = nullptr;
    QPointList conPoints;

    con1 = node->connectorList().at(0).data();
    con2 = node->connectorList().at(1).data();

    if (con1 == con2)
        return false;

    // we don't know on which end of the connectors is our node, so we must check both ends
    // HACK // TODO // dynamic_cast used, because Connector doesn't know about ECNode, only Node
    if (con1->startNode() == node)
        startNode = dynamic_cast<ECNode *>(con1->endNode());
    else
        startNode = dynamic_cast<ECNode *>(con1->startNode());

    if (con2->startNode() == node)
        endNode = dynamic_cast<ECNode *>(con2->endNode());
    else
        endNode = dynamic_cast<ECNode *>(con2->startNode());

    conPoints = con1->connectorPoints(false) + con2->connectorPoints(false);

    if (!startNode || !endNode)
        return false;

    Connector *newCon = endNode->createConnector(startNode);

    if (!newCon)
        return false;

    startNode->addConnector(newCon);
    newCon->setRoutePoints(conPoints, con1->usesManualPoints() || con2->usesManualPoints());

    // Avoid flicker: update draw lists now
    con1->updateConnectorPoints(false);
    con2->updateConnectorPoints(false);
    newCon->updateDrawList();

    node->removeNode();
    con1->removeConnectorNoArg();
    con2->removeConnectorNoArg();

    return true;
}

#include "moc_circuiticndocument.cpp"
