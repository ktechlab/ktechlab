/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "icndocument.h"
#include "connector.h"
#include "conrouter.h"
#include "node.h"
#include "nodegroup.h"

#include <kdebug.h>

NodeGroup::NodeGroup(ICNDocument *icnDocument, const char *name)
		: QObject(icnDocument, name) {
	p_icnDocument = icnDocument;
	b_visible = true;
}

NodeGroup::~NodeGroup() {
	clearConList();

//m_extNodeList.remove((Node*)0);
	const NodeList::iterator xnEnd = m_extNodeList.end();
	for (NodeList::iterator it = m_extNodeList.begin(); it != xnEnd; ++it)
		(*it)->setNodeGroup(0);

	m_extNodeList.clear();

//m_nodeList.remove((Node*)0);
	const NodeList::iterator nEnd = m_nodeList.end();
	for (NodeList::iterator it = m_nodeList.begin(); it != nEnd; ++it)
		(*it)->setNodeGroup(0);

	m_nodeList.clear();
}

void NodeGroup::setVisible(bool visible) {
	if (b_visible == visible) return;

	b_visible = visible;

//m_nodeList.remove((Node*)0);

	const NodeList::iterator nEnd = m_nodeList.end();
	for (NodeList::iterator it = m_nodeList.begin(); it != nEnd; ++it)
		(*it)->setVisible(visible);
}

void NodeGroup::addNode(Node *node, bool checkSurrouding) {
	if(!node || node->isChildNode() || m_nodeList.contains(node)) 
		return;

	m_nodeList.append(node);
	node->setNodeGroup(this);
	node->setVisible(b_visible);

	if(checkSurrouding) {
		ConnectorList con = node->getAllConnectors();
		ConnectorList::iterator end = con.end();
		for(ConnectorList::iterator it = con.begin(); it != end; ++it) {
assert(*it);
assert((*it)->startNode() != (*it)->endNode());
			if((*it)->startNode() != node)
				addNode((*it)->startNode(), true);

			if((*it)->endNode() != node)
				addNode((*it)->endNode(), true);
		}
	}
}

void NodeGroup::translate(int dx, int dy) {
	if ((dx == 0) && (dy == 0))
		return;

//	m_nodeList.remove((Node*)0);

	const ConnectorList::iterator conEnd = m_conList.end();
	for(ConnectorList::iterator it = m_conList.begin(); it != conEnd; ++it) {
		(*it)->updateConnectorPoints(false);
		(*it)->translateRoute(dx, dy);
	}

	const NodeList::iterator nodeEnd = m_nodeList.end();
	for(NodeList::iterator it = m_nodeList.begin(); it != nodeEnd; ++it)
		(*it)->moveBy(dx, dy);
}

void NodeGroup::updateRoutes() {
	resetRoutedMap();

	// Basic algorithm used here: starting with the external nodes, find the
	// pair with the shortest distance between them. Route the connectors
	// between the two nodes appropriately. Remove that pair of nodes from the
	// list, and add the nodes along the connector route (which have been spaced
	// equally along the route). Repeat until all the nodes are connected.

	const ConnectorList::iterator conEnd = m_conList.end();
	for (ConnectorList::iterator it = m_conList.begin(); it != conEnd; ++it) {
assert(*it);
		(*it)->updateConnectorPoints(false);
	}

	Node *n1, *n2;
	NodeList currentList = m_extNodeList;

	while (!currentList.isEmpty()) {
		findBestPair(&currentList, &n1, &n2);

		if(n1 == 0 || n2 == 0) return;

		NodeList route = findRoute(n1, n2);
		currentList += route;
		ConRouter cr(p_icnDocument);
		cr.mapRoute((int)n1->x(), (int)n1->y(), (int)n2->x(), (int)n2->y());
		QPointListList pl = cr.dividePoints(route.size() + 1);

		const NodeList::iterator routeEnd = route.end();
		const QPointListList::iterator plEnd = pl.end();
		Node *prev = n1;
		NodeList::iterator routeIt = route.begin();
		for (QPointListList::iterator it = pl.begin(); it != plEnd; ++it) {
			Node *next = (routeIt == routeEnd) ? n2 : (Node*)*(routeIt++);
			removeRoutedNodes(&currentList, prev, next);

			if (prev != n1) {
				QPoint first = (*it).first();
				prev->moveBy(first.x() - prev->x(), first.y() - prev->y());
			}

			Connector *con = findCommonConnector(prev, next);

			if(con) {
				con->updateConnectorPoints(false);
				con->setRoutePoints((*it), false, false);
				con->updateConnectorPoints(true);
			}

			prev = next;
		}
	}
}

NodeList NodeGroup::findRoute(Node *startNode, Node *endNode) {
	NodeList nl;

	if (!startNode || !endNode || startNode == endNode)
		return nl;

	IntList temp;
	IntList il = findRoute(temp, getNodePos(startNode), getNodePos(endNode));

	const IntList::iterator end = il.end();
	for (IntList::iterator it = il.begin(); it != end; ++it) {
		Node *node = getNodePtr(*it);
assert(node);
		nl += node;
	}

	nl.remove(startNode);
	nl.remove(endNode);

	return nl;
}

IntList NodeGroup::findRoute(IntList used, int currentNode, int endNode, bool *success) {
	bool temp;

	if(!success) success = &temp;

	*success = false;

	if (!used.contains(currentNode)) 
		used.append(currentNode);

	if (currentNode == endNode) {
		*success = true;
		return used;
	}

// WTF do we think we're doing here? 
	const uint n = m_nodeList.size() + m_extNodeList.size();
	for (uint i = 0; i < n; ++i) {
		if (b_routedMap[i * n + currentNode] && !used.contains(i)) {
			IntList il = findRoute(used, i, endNode, success);

			if(*success)
				return il;
		}
	}

	IntList il;
	return il;
}

Connector *NodeGroup::findCommonConnector(Node *n1, Node *n2) const {
	if(!n1 || !n2 || n1 == n2)
		return 0;
//assert(n1 && n2 && n1 != n2);

	ConnectorList n1Con = n1->getAllConnectors();
	ConnectorList n2Con = n2->getAllConnectors();

	const ConnectorList::iterator end = n1Con.end();
	for (ConnectorList::iterator it = n1Con.begin(); it != end; ++it) {
		if (n2Con.find(*it) != n2Con.end())
			return *it;
	}

	return 0;
}

void NodeGroup::findBestPair(NodeList *list, Node **n1, Node **n2) const {
	*n1 = 0;
	*n2 = 0;

	if(list->size() < 2) return;

	int shortest = 1 << 30;

	const NodeList::iterator end = list->end();
	for(NodeList::iterator it1 = list->begin(); it1 != end; ++it1) {
		NodeList::iterator it2 = it1;

		for(++it2; it2 != end; ++it2) {
assert(*it1 != *it2);

			if(canRoute(*it1, *it2)) {
// Try and find any that are aligned horizontally 
				if((*it1)->y() == (*it2)->y()) {
					const int distance = std::abs(int((*it1)->x() - (*it2)->x()));

					if(distance < shortest) {
						shortest = distance;
						*n1 = *it1;
						*n2 = *it2;
					}
				}

// vertical is just as good so we do that in the same loop.
				if((*it1)->x() == (*it2)->x()) {
					const int distance = std::abs(int((*it1)->y() - (*it2)->y()));

					if (distance < shortest) {
						shortest = distance;
						*n1 = *it1;
						*n2 = *it2;
					}
				}
			}
		}
	}

	if(*n1) return;

	// Now, lets just find the two closest nodes
	for(NodeList::iterator it1 = list->begin(); it1 != end; ++it1) {
		NodeList::iterator it2 = it1;

		for(++it2; it2 != end; ++it2) {
			if(canRoute(*it1, *it2)) {
				const double dx = (*it1)->x() - (*it2)->x();
				const double dy = (*it1)->y() - (*it2)->y();

				const int distance = hypot(dx, dy);

				if(distance < shortest) {
					shortest = distance;
					*n1 = *it1;
					*n2 = *it2;
				}
			}
		}
	}

	if(!*n1)
		kdError() << "NodeGroup::findBestPair: Could not find a routable pair of nodes!" << endl;
}


bool NodeGroup::canRoute(Node *n1, Node *n2) const {
	if(!n1 || !n2) return false;

	IntList reachable;

	getReachable(&reachable, getNodePos(n1));
	return reachable.contains(getNodePos(n2));
}


void NodeGroup::getReachable(IntList *reachable, int node) const {
	if (!reachable || reachable->contains(node))
		return;

	reachable->append(node);

	const uint n = m_nodeList.size() + m_extNodeList.size();
	assert(node < int(n));

// what kind of weird recursive algorithm is this??? 
	for (uint i = 0; i < n; ++i) {
		if (b_routedMap[i * n + node]) {
			getReachable(reachable, i);
		}
	}
}

void NodeGroup::resetRoutedMap() {
	const uint n = m_nodeList.size() + m_extNodeList.size();

	b_routedMap.resize(0);
	b_routedMap.resize(n * n, false);

	const ConnectorList::iterator end = m_conList.end();
	for(ConnectorList::iterator it = m_conList.begin(); it != end; ++it) {
		Connector *con = *it;
assert(con);
		int n1 = getNodePos(con->startNode());
		int n2 = getNodePos(con->endNode());

		if(n1 != -1 && n2 != -1) {
			b_routedMap[n1 * n + n2] = b_routedMap[n2 * n + n1] = true;
		}
	}
}

void NodeGroup::removeRoutedNodes(NodeList *nodes, Node *n1, Node *n2) {
	if (!nodes) return;

	// Lets get rid of any duplicate nodes in nodes (as a general cleaning operation)
// TODO: optimize this out. 
	const NodeList::iterator end = nodes->end();
	for (NodeList::iterator it = nodes->begin(); it != end; ++it) {
		if (nodes->contains(*it) > 1)
			*it = 0;
	}
	nodes->remove((Node*)0);
// ###

	const int n1pos = getNodePos(n1);
	const int n2pos = getNodePos(n2);

	if(n1pos == -1 || n2pos == -1) return;

	const uint n = m_nodeList.size() + m_extNodeList.size();

// b_routedMap is actually an N^2 array. It is used to determine whether a path exists between two nodes.
// TODO: explicate this code, it's really tricky and I'm not sure it's correct. 
	b_routedMap[n1pos * n + n2pos] = b_routedMap[n2pos * n + n1pos] = false;

	bool n1HasCon = false, n2HasCon = false;

	for (uint i = 0; i < n; ++i) {
		n1HasCon |= b_routedMap[n1pos * n + i];
		n2HasCon |= b_routedMap[n2pos * n + i];
	}

	if(!n1HasCon) nodes->remove(n1);
	if(!n2HasCon) nodes->remove(n2);
}

int NodeGroup::getNodePos(Node *n) const {
assert(n);

	int pos = m_nodeList.findIndex(n);

	if (pos != -1) return pos;

	pos = m_extNodeList.findIndex(n);

	if (pos != -1) return pos + m_nodeList.size();

	return -1;
}

Node* NodeGroup::getNodePtr(unsigned int n) const {
	const int a = m_nodeList.size();
	if(n < a) return m_nodeList[n];

	if (n < a + m_extNodeList.size()) 
		return m_extNodeList[n - a];

	return 0;
}

void NodeGroup::clearConList() {
	const ConnectorList::iterator end = m_conList.end();
	for(ConnectorList::iterator it = m_conList.begin(); it != end; ++it) {
		Connector *con = *it;

assert(con);
		con->setNodeGroup(0);
		disconnect(con, SIGNAL(removed(Connector*)), this, SLOT(connectorRemoved(Connector*)));
	}

	m_conList.clear();
}

void NodeGroup::init() {
	{
	NodeList::iterator xnEnd = m_extNodeList.end();
	for (NodeList::iterator it = m_extNodeList.begin(); it != xnEnd; ++it) {
		(*it)->setNodeGroup(0);
	}

	m_extNodeList.clear();
	}

	clearConList();

	// First, lets get all of our external nodes and internal connectors
	const NodeList::iterator nlEnd = m_nodeList.end();
	for (NodeList::iterator nodeIt = m_nodeList.begin(); nodeIt != nlEnd; ++nodeIt) {
		// 2. rewrite
		ConnectorList conList = (*nodeIt)->getAllConnectors();

		ConnectorList::iterator conEnd = conList.end();
		for(ConnectorList::iterator conIt = conList.begin(); conIt != conEnd; ++conIt) {
			Connector *con = *conIt;

assert(con->startNode() != con->endNode());

			if (con->startNode() != *nodeIt) {
				addExtNode(con->startNode());
				m_conList.insert(con);
				con->setNodeGroup(this);
			}

			if (con->endNode() != *nodeIt) {
				addExtNode(con->endNode());
				m_conList.insert(con);
				con->setNodeGroup(this);
			}

			connect(con, SIGNAL(removed(Connector*)), this, SLOT(connectorRemoved(Connector*)));
		}

		// Connect the node up to us
		connect(*nodeIt, SIGNAL(removed(Node*)), this, SLOT(nodeRemoved(Node*)));
	}

	// And connect up our external nodes
	{
	NodeList::iterator xnEnd = m_extNodeList.end();
	for (NodeList::iterator it = m_extNodeList.begin(); it != xnEnd; ++it) {
// 		connect( *it, SIGNAL(moved(Node*)), this, SLOT(extNodeMoved()) );
		connect(*it, SIGNAL(removed(Node*)), this, SLOT(nodeRemoved(Node*)));
	}
	}
}

void NodeGroup::nodeRemoved(Node *node) {
	// We are probably about to get deleted by ICNDocument anyway...so no point in doing anything
	m_nodeList.remove(node);
	node->setNodeGroup(0);
	node->setVisible(false); // I think this should be false even though it was previously set true...
	m_extNodeList.remove(node);
}

void NodeGroup::connectorRemoved(Connector *connector) {
	m_conList.erase(connector);
}

void NodeGroup::addExtNode(Node *node) {
	if (!m_extNodeList.contains(node) && !m_nodeList.contains(node)) {
		m_extNodeList.append(node);
		node->setNodeGroup(this);
	}
}

#include "nodegroup.moc"
