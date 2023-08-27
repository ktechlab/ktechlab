/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "fpnode.h"
#include "connector.h"
#include "flowconnector.h"
#include "flowpart.h"
#include "icndocument.h"

#include <QPainter>

#include <ktechlab_debug.h>

FPNode::FPNode(ICNDocument *icnDocument, Node::node_type type, int dir, const QPoint &pos, QString *id)
    : Node(icnDocument, type, dir, pos, id)
{
    if (icnDocument)
        icnDocument->registerItem(this);
    m_outputConnector = nullptr;
}

FPNode::~FPNode()
{
}

FlowPart *FPNode::outputFlowPart() const
{
    // for InputFlowNode this member is overridden

    if (!m_outputConnector)
        return nullptr;
    if (m_outputConnector->endNode() == nullptr)
        return nullptr;

    return (dynamic_cast<FPNode *>(m_outputConnector->endNode()))->outputFlowPart();
}

FlowPartList FPNode::inputFlowParts() const
{
    // for InputFlowNode it's overridden

    FlowPartList list;
    FlowPart *flowPart = dynamic_cast<FlowPart *>(parentItem());

    if (flowPart) {
        list.append(flowPart);
        return list;
    }

    const FlowConnectorList::const_iterator end = m_inFlowConnList.end();
    for (FlowConnectorList::const_iterator it = m_inFlowConnList.begin(); it != end; ++it) {
        if (*it) {
            Node *startNode = (*it)->startNode();
            FlowPart *flowPart = startNode ? dynamic_cast<FlowPart *>(startNode->parentItem()) : nullptr;
            if (flowPart)
                list.append(flowPart);
        }
    }

    return list;
}

inline QPolygon arrowPoints(int dir)
{
    QPolygon pa(3);
    switch (dir) {
    case 0:
        pa[0] = QPoint(3, 0);
        pa[1] = QPoint(0, 2);
        pa[2] = QPoint(0, -2);
        break;
    case 180:
        pa[0] = QPoint(-3, 0);
        pa[1] = QPoint(0, 2);
        pa[2] = QPoint(0, -2);
        break;
    case 90:
        pa[0] = QPoint(2, 0);
        pa[1] = QPoint(-2, 0);
        pa[2] = QPoint(0, 3);
        break;
    case 270:
        pa[0] = QPoint(2, 0);
        pa[1] = QPoint(-2, 0);
        pa[2] = QPoint(0, -3);
        break;
    };
    return pa;
}

void FPNode::addOutputConnector(Connector *const connector)
{
    // for Junction and output flownodes
    if (!handleNewConnector(connector))
        return;

    if (m_outputConnector)
        qCCritical(KTL_LOG) << "BUG: adding an output connector when we already have one";

    // FIXME dynamic_cast connector
    m_outputConnector = dynamic_cast<FlowConnector *>(connector);
}

void FPNode::addInputConnector(Connector *const connector)
{
    // for Junction and Input flownodes
    if (!handleNewConnector(connector))
        return;

    // FIXME dynamic_cast connector
    m_inFlowConnList.append(dynamic_cast<FlowConnector *>(connector));
}

bool FPNode::handleNewConnector(Connector *connector)
{
    if (!connector)
        return false;

    // FIXME dynamic_cast connector
    if (m_inFlowConnList.contains(dynamic_cast<FlowConnector *>(connector)) || ((Connector *)m_outputConnector == connector)) {
        qCWarning(KTL_LOG) << " Already have connector = " << connector;
        return false;
    }

    connect(this, &FPNode::removed, connector, &Connector::removeConnector);
    connect(connector, &Connector::removed, this, &FPNode::checkForRemoval);
    connect(connector, &Connector::selected, this, &FPNode::setNodeSelected);

    if (!isChildNode())
        p_icnDocument->slotRequestAssignNG();

    return true;
}

Connector *FPNode::createInputConnector(Node *startNode)
{
    if ((!acceptInput()) || !startNode)
        return nullptr;

    // FIXME dynamic_cast used
    Connector *connector = new FlowConnector(dynamic_cast<FPNode *>(startNode), dynamic_cast<FPNode *>(this), p_icnDocument);
    addInputConnector(connector);

    return connector;
}

int FPNode::numCon(bool includeParentItem, bool includeHiddenConnectors) const
{
    unsigned count = 0;

    FlowConnectorList connectors = m_inFlowConnList;
    if (m_outputConnector)
        connectors.append(m_outputConnector);

    FlowConnectorList::const_iterator end = connectors.end();
    for (FlowConnectorList::const_iterator it = connectors.begin(); it != end; ++it) {
        if (*it && (includeHiddenConnectors || (*it)->canvas()))
            count++;
    }

    if (isChildNode() && includeParentItem)
        count++;

    return count;
}

void FPNode::removeConnector(Connector *connector)
{
    if (!connector)
        return;

    FlowConnectorList::iterator it;

    // FIXME dynamic_cast connector
    it = m_inFlowConnList.find(dynamic_cast<FlowConnector *>(connector));
    if (it != m_inFlowConnList.end()) {
        (*it)->removeConnectorNoArg();
        (*it) = nullptr;
    }

    if ((Connector *)m_outputConnector == connector) {
        connector->removeConnectorNoArg();
        m_outputConnector = nullptr;
    }
}

void FPNode::checkForRemoval(Connector *connector)
{
    removeConnector(connector);
    setNodeSelected(false);

    removeNullConnectors();

    if (!p_parentItem) {
        int conCount = m_inFlowConnList.count();
        if (m_outputConnector)
            conCount++;
        if (conCount < 2)
            removeNode();
    }
    // for JunctionFlowNode this method is overridden!
}

void FPNode::removeNullConnectors()
{
    m_inFlowConnList.remove((FlowConnector *)nullptr);
}

QPoint FPNode::findConnectorDivergePoint(bool *found)
{
    bool temp;
    if (!found)
        found = &temp;
    *found = false;

    if (numCon(false, false) != 2)
        return QPoint(0, 0);

    QPointList p1;
    QPointList p2;

    int inSize = m_inFlowConnList.count();

    FlowConnectorList connectors = m_inFlowConnList;
    if (m_outputConnector)
        connectors.append(m_outputConnector);

    const FlowConnectorList::const_iterator end = connectors.end();
    bool gotP1 = false;
    bool gotP2 = false;
    int at = -1;
    for (FlowConnectorList::const_iterator it = connectors.begin(); it != end && !gotP2; ++it) {
        at++;
        if (!(*it) || !(*it)->canvas())
            continue;

        if (gotP1) {
            p2 = (*it)->connectorPoints(at < inSize);
            gotP2 = true;
        } else {
            p1 = (*it)->connectorPoints(at < inSize);
            gotP1 = true;
        }
    }

    if (!gotP1 || !gotP2)
        return QPoint(0, 0);

    unsigned maxLength = p1.size() > p2.size() ? p1.size() : p2.size();

    for (unsigned i = 1; i < maxLength; ++i) {
        if (p1[i] != p2[i]) {
            *found = true;
            return p1[i - 1];
        }
    }
    return QPoint(0, 0);
}

void FPNode::setVisible(bool yes)
{
    if (isVisible() == yes)
        return;

    KtlQCanvasPolygon::setVisible(yes);

    const FlowConnectorList::iterator inputEnd = m_inFlowConnList.end();
    for (FlowConnectorList::iterator it = m_inFlowConnList.begin(); it != inputEnd; ++it) {
        Connector *connector = *it;
        if (connector) {
            if (isVisible())
                connector->setVisible(true);
            else {
                Node *node = connector->startNode();
                connector->setVisible(node && node->isVisible());
            }
        }
    }

    Connector *connector = m_outputConnector;
    if (connector) {
        if (isVisible())
            connector->setVisible(true);
        else {
            Node *node = connector->endNode();
            connector->setVisible(node && node->isVisible());
        }
    }
}

bool FPNode::isConnected(Node *node, NodeList *checkedNodes)
{
    if (this == node)
        return true;

    bool firstNode = !checkedNodes;
    if (firstNode)
        checkedNodes = new NodeList();

    else if (checkedNodes->contains(this))
        return false;

    checkedNodes->append(this);

    const FlowConnectorList::const_iterator inputEnd = m_inFlowConnList.end();
    for (FlowConnectorList::const_iterator it = m_inFlowConnList.begin(); it != inputEnd; ++it) {
        Connector *connector = *it;
        if (connector) {
            Node *startNode = connector->startNode();
            if (startNode && startNode->isConnected(node, checkedNodes)) {
                if (firstNode) {
                    delete checkedNodes;
                }
                return true;
            }
        }
    }

    Connector *connector = m_outputConnector;
    if (connector) {
        Node *endNode = connector->endNode();
        if (endNode && endNode->isConnected(node, checkedNodes)) {
            if (firstNode) {
                delete checkedNodes;
            }
            return true;
        }
    }

    if (firstNode) {
        delete checkedNodes;
    }

    return false;
}

ConnectorList FPNode::inputConnectorList() const
{
    return (ConnectorList)(FlowConnectorList)m_inFlowConnList;
}

ConnectorList FPNode::outputConnectorList() const
{
    ConnectorList out;
    if (m_outputConnector)
        out.append((Connector *)m_outputConnector); // un upcast between downcasts :o
    return out;
}

ConnectorList FPNode::getAllConnectors() const
{
    ConnectorList all = (ConnectorList)(FlowConnectorList)m_inFlowConnList;
    if (m_outputConnector)
        all.append((Connector *)m_outputConnector);
    return all;
}

Connector *FPNode::getAConnector() const
{
    if (!m_inFlowConnList.isEmpty())
        return *m_inFlowConnList.begin();

    if (m_outputConnector)
        return m_outputConnector;

    return nullptr;
}

#include "moc_fpnode.cpp"
