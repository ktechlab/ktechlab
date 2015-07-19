/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "icndocument.h"
#include "connector.h"
#include "conrouter.h"
#include "node.h"
#include "nodegroup.h"

#include <kdebug.h>
#include <cassert>
#include <cstdlib>

NodeGroup::NodeGroup( ICNDocument *icnDocument, const char *name )
	: QObject( icnDocument, name )
{
	p_icnDocument = icnDocument;
	b_visible = true;
}


NodeGroup::~NodeGroup()
{
	clearConList();
	
	m_extNodeList.remove( (Node*)0l );
	const NodeList::iterator xnEnd = m_extNodeList.end();
	for ( NodeList::iterator it = m_extNodeList.begin(); it != xnEnd; ++it )
		(*it)->setNodeGroup(0l);
	m_extNodeList.clear();
	
	m_nodeList.remove( (Node*)0l );
	const NodeList::iterator nEnd = m_nodeList.end();
	for ( NodeList::iterator it = m_nodeList.begin(); it != nEnd; ++it )
		(*it)->setNodeGroup(0l);
	m_nodeList.clear();
}


void NodeGroup::setVisible( bool visible )
{
	if ( b_visible == visible )
		return;
	
	b_visible = visible;
	
	m_nodeList.remove( (Node*)0l );
	const NodeList::iterator nEnd = m_nodeList.end();
	for ( NodeList::iterator it = m_nodeList.begin(); it != nEnd; ++it )
		(*it)->setVisible(visible);
}


void NodeGroup::addNode( Node *node, bool checkSurrouding )
{
	if ( !node || node->isChildNode() || m_nodeList.contains(node) ) {
		return;
	}
	
	m_nodeList.append(node);
	node->setNodeGroup(this);
	node->setVisible(b_visible);
	
	if (checkSurrouding)
	{
		ConnectorList con = node->getAllConnectors();
		ConnectorList::iterator end = con.end();
		for ( ConnectorList::iterator it = con.begin(); it != end; ++it )
		{
			if (*it) {
				// maybe we can put here a check, because only 1 of there checks should pass
				if( (*it)->startNode() != node )
					addNode( (*it)->startNode(), true );
				if( (*it)->endNode() != node )
					addNode( (*it)->endNode(), true );
			}
		}

	}
}


void NodeGroup::translate( int dx, int dy )
{
	if ( (dx == 0) && (dy == 0) )
		return;
	
	m_conList.remove((Connector*)0l);
	m_nodeList.remove((Node*)0l);
	
	const ConnectorList::iterator conEnd = m_conList.end();
	for ( ConnectorList::iterator it = m_conList.begin(); it != conEnd; ++it )
	{
		(*it)->updateConnectorPoints(false);
		(*it)->translateRoute( dx, dy );
	}
	
	const NodeList::iterator nodeEnd = m_nodeList.end();
	for ( NodeList::iterator it = m_nodeList.begin(); it != nodeEnd; ++it )
		(*it)->moveBy( dx, dy );
}


void NodeGroup::updateRoutes()
{
	resetRoutedMap();
	
	// Basic algorithm used here: starting with the external nodes, find the
	// pair with the shortest distance between them. Route the connectors
	// between the two nodes appropriately. Remove that pair of nodes from the
	// list, and add the nodes along the connector route (which have been spaced
	// equally along the route). Repeat until all the nodes are connected.
	
	
	const ConnectorList::iterator conEnd = m_conList.end();
	for ( ConnectorList::iterator it = m_conList.begin(); it != conEnd; ++it )
	{
		if (*it) 
			(*it)->updateConnectorPoints(false);
	}
	
	Node *n1, *n2;
	NodeList currentList = m_extNodeList;
	while ( !currentList.isEmpty() )
	{
		findBestPair( &currentList, &n1, &n2 );
		if ( n1 == 0l || n2 == 0l ) {
			return;
		}
		NodeList route = findRoute( n1, n2 );
		currentList += route;
		
		ConRouter cr(p_icnDocument);
		cr.mapRoute( (int)n1->x(), (int)n1->y(), (int)n2->x(), (int)n2->y() );
        if (cr.pointList(false).size() <= 0) {
            qDebug() << Q_FUNC_INFO << "no ConRouter points, giving up";
            return; // continue might get to an infinite loop
        }
		QPointListList pl = cr.dividePoints( route.size()+1 );
		
		const NodeList::iterator routeEnd = route.end();
		const QPointListList::iterator plEnd = pl.end();
		Node *prev = n1;
		NodeList::iterator routeIt = route.begin();
		for ( QPointListList::iterator it = pl.begin(); it != plEnd; ++it )
		{
			Node *next = (routeIt == routeEnd) ? n2 : (Node*)*(routeIt++);
			removeRoutedNodes( &currentList, prev, next );
			QPointList pointList = *it;
			if ( prev != n1 )
			{
				QPoint first = pointList.first();
				prev->moveBy( first.x() - prev->x(), first.y() - prev->y() );
			}
			Connector *con = findCommonConnector( prev, next );
			if (con)
			{
				con->updateConnectorPoints(false);
				con->setRoutePoints( pointList, false, false );
				con->updateConnectorPoints(true);
// 				con->conRouter()->setPoints( &pointList, con->startNode() != prev );
// 				con->conRouter()->setPoints( &pointList, con->pointsAreReverse( &pointList ) );
// 				con->calcBoundingPoints();
			}
			prev = next;
		}
	}
}


NodeList NodeGroup::findRoute( Node *startNode, Node *endNode )
{
	NodeList nl;
	if ( !startNode || !endNode || startNode == endNode ) {
		return nl;
	}

	IntList temp;	
	IntList il = findRoute( temp, getNodePos(startNode), getNodePos(endNode) );
	
	const IntList::iterator end = il.end();
	for ( IntList::iterator it = il.begin(); it != end; ++it )
	{
		Node *node = getNodePtr(*it);
		if (node) {
			nl += node;
		}
	}
	
	nl.remove(startNode);
	nl.remove(endNode);
	
	return nl;
}


IntList NodeGroup::findRoute( IntList used, int currentNode, int endNode, bool *success )
{
	bool temp;
	if (!success) {
		success = &temp;
	}
	*success = false;
	
	if ( !used.contains(currentNode) ) {
		used.append(currentNode);
	}
	
	if ( currentNode == endNode ) {
		*success = true;
		return used;
	}
	
	const uint n = m_nodeList.size()+m_extNodeList.size();
	for ( uint i=0; i<n; ++i )
	{
		if ( b_routedMap[i*n+currentNode] && !used.contains(i) )
		{
			IntList il = findRoute( used, i, endNode, success );
			if (*success) {
				return il;
			}
		}
	}
	
	IntList il;
	return il;
}


Connector* NodeGroup::findCommonConnector( Node *n1, Node *n2 )
{
	if ( !n1 || !n2 || n1==n2 ) {
		return 0l;
	}
	
	ConnectorList n1Con = n1->getAllConnectors();
	ConnectorList n2Con = n2->getAllConnectors();
	
	const ConnectorList::iterator end = n1Con.end();
	for ( ConnectorList::iterator it = n1Con.begin(); it != end; ++it )
	{
		if ( n2Con.contains(*it) ) {
			return *it;
		}
	}
	return 0l;
}


void NodeGroup::findBestPair( NodeList *list, Node **n1, Node **n2 )
{
	*n1 = 0l;
	*n2 = 0l;
	
	if ( list->size() < 2 ) {
		return;
	}
	
	const NodeList::iterator end = list->end();
	int shortest = 1<<30;
	
	// Try and find any that are aligned horizontally
	for ( NodeList::iterator it1 = list->begin(); it1 != end; ++it1 )
	{
		NodeList::iterator it2 = it1;
		for ( ++it2; it2 != end; ++it2 )
		{
			if ( *it1 != *it2 && (*it1)->y() == (*it2)->y() && canRoute( *it1, *it2 ) )
			{
				const int distance = std::abs(int( (*it1)->x()-(*it2)->x() ));
				if ( distance < shortest )
				{
					shortest = distance;
					*n1 = *it1;
					*n2 = *it2;	
				}
			}
		}
	}
	if (*n1) {
		return;
	}
	
	// Try and find any that are aligned vertically
	for ( NodeList::iterator it1 = list->begin(); it1 != end; ++it1 )
	{
		NodeList::iterator it2 = it1;
		for ( ++it2; it2 != end; ++it2 )
		{
			if ( *it1 != *it2 && (*it1)->x() == (*it2)->x() && canRoute( *it1, *it2 ) )
			{
				const int distance = std::abs(int( (*it1)->y()-(*it2)->y() ));
				if ( distance < shortest )
				{
					shortest = distance;
					*n1 = *it1;
					*n2 = *it2;	
				}
			}
		}
	}
	if (*n1) {
		return;
	}
	
	// Now, lets just find the two closest nodes
	for ( NodeList::iterator it1 = list->begin(); it1 != end; ++it1 )
	{
		NodeList::iterator it2 = it1;
		for ( ++it2; it2 != end; ++it2 )
		{
			if ( *it1 != *it2 && canRoute( *it1, *it2 ) )
			{
				const int dx = (int)((*it1)->x()-(*it2)->x());
				const int dy = (int)((*it1)->y()-(*it2)->y());
				const int distance = std::abs(dx) + std::abs(dy);
				if ( distance < shortest )
				{
					shortest = distance;
					*n1 = *it1;
					*n2 = *it2;
				}
			}
		}
	}
	
	if (!*n1) {
		kError() << "NodeGroup::findBestPair: Could not find a routable pair of nodes!"<<endl;
	}
}


bool NodeGroup::canRoute( Node *n1, Node *n2 )
{
	if ( !n1 || !n2 ) {
		return false;
	}
	
	IntList reachable;
	getReachable( &reachable, getNodePos(n1) );
	return reachable.contains(getNodePos(n2));
}


void NodeGroup::getReachable( IntList *reachable, int node )
{
	if ( !reachable || reachable->contains(node) ) {
		return;
	}
	reachable->append(node);
	
	const uint n = m_nodeList.size() + m_extNodeList.size();
	assert( node < int(n) );
	for ( uint i=0; i<n; ++i )
	{
		if ( b_routedMap[i*n + node] ) {
			getReachable(reachable,i);
		}
	}
}


void NodeGroup::resetRoutedMap()
{
	const uint n = m_nodeList.size() + m_extNodeList.size();
	
	b_routedMap.resize(0);
	b_routedMap.resize( n*n, false );
	
	const ConnectorList::iterator end = m_conList.end();
	for ( ConnectorList::iterator it = m_conList.begin(); it != end; ++it )
	{
		Connector *con = *it;
		if (con)
		{
			int n1 = getNodePos(con->startNode());
			int n2 = getNodePos(con->endNode());
			if ( n1 != -1 && n2 != -1 )
			{
				b_routedMap[n1*n + n2] = b_routedMap[n2*n + n1] = true;
			}
		}
	}
}


void NodeGroup::removeRoutedNodes( NodeList *nodes, Node *n1, Node *n2 )
{
	if (!nodes) {
		return;
	}
	
	// Lets get rid of any duplicate nodes in nodes (as a general cleaning operation)
	const NodeList::iterator end = nodes->end();
	for ( NodeList::iterator it = nodes->begin(); it != end; ++it )
	{
		//if ( nodes->contains(*it) > 1 ) {
        if ( nodes->count(*it) > 1 ) {
			*it = 0l;
		}
	}
	nodes->remove((Node*)0l);
	
	const int n1pos = getNodePos(n1);
	const int n2pos = getNodePos(n2);
	
	if ( n1pos == -1 || n2pos == -1 ) {
		return;
	}
	
	const uint n = m_nodeList.size() + m_extNodeList.size();
	
	b_routedMap[n1pos*n + n2pos] = b_routedMap[n2pos*n + n1pos] = false;
	
	bool n1HasCon = false;
	bool n2HasCon = false;
	
	for ( uint i=0; i<n; ++i )
	{
		n1HasCon |= b_routedMap[n1pos*n + i];
		n2HasCon |= b_routedMap[n2pos*n + i];
	}
	
	if (!n1HasCon) {
		nodes->remove(n1);
	}
	if (!n2HasCon) {
		nodes->remove(n2);
	}
}


int NodeGroup::getNodePos( Node *n )
{
	if (!n) {
		return -1;
	}
	int pos = m_nodeList.findIndex(n);
	if ( pos != -1 ) {
		return pos;
	}
	pos = m_extNodeList.findIndex(n);
	if ( pos != -1 ) {
		return pos+m_nodeList.size();
	}
	return -1;
}


Node* NodeGroup::getNodePtr( int n )
{
	if ( n<0 ) {
		return 0l;
	}
	const int a = m_nodeList.size();
	if (n<a) {
		return m_nodeList[n];
	}
	const int b = m_extNodeList.size();
	if (n<a+b) {
		return m_extNodeList[n-a];
	}
	return 0l;
}


void NodeGroup::clearConList()
{
	const ConnectorList::iterator end = m_conList.end();
	for ( ConnectorList::iterator it = m_conList.begin(); it != end; ++it )
	{
		Connector *con = *it;
		if (con)
		{
			con->setNodeGroup(0l);
			disconnect( con, SIGNAL(removed(Connector*)), this, SLOT(connectorRemoved(Connector*)) );
		}
	}
	m_conList.clear();
}


void NodeGroup::init()
{
	NodeList::iterator xnEnd = m_extNodeList.end();
	for ( NodeList::iterator it = m_extNodeList.begin(); it != xnEnd; ++it )
	{
		(*it)->setNodeGroup(0l);
	}
	m_extNodeList.clear();
	
	clearConList();
	
	// First, lets get all of our external nodes and internal connectors
	const NodeList::iterator nlEnd = m_nodeList.end();
	for ( NodeList::iterator nodeIt = m_nodeList.begin(); nodeIt != nlEnd; ++nodeIt )
	{
		// 2. rewrite
		ConnectorList conList = ( *nodeIt )->getAllConnectors();
	
		ConnectorList::iterator conIt,
		conEnd = conList.end();
		for ( conIt = conList.begin(); conIt != conEnd; ++conIt )
		{
	
			Connector *con = *conIt;
	
			// possible check: only 1 of these ifs should be true
			if ( con->startNode() != *nodeIt )
			{
				addExtNode ( con->startNode() );
				if ( !m_conList.contains ( con ) )
				{
					m_conList += con;
					con->setNodeGroup ( this );
				}
			}
			if ( con->endNode() != *nodeIt )
			{
				addExtNode ( con->endNode() );
				if ( !m_conList.contains ( con ) )
				{
					m_conList += con;
					con->setNodeGroup ( this );
				}
			}
			connect ( con, SIGNAL ( removed ( Connector* ) ), this, SLOT ( connectorRemoved ( Connector* ) ) );
		}
	
		// Connect the node up to us
		connect ( *nodeIt, SIGNAL ( removed ( Node* ) ), this, SLOT ( nodeRemoved ( Node* ) ) );
	}
	
	// And connect up our external nodes
	xnEnd = m_extNodeList.end();
	for ( NodeList::iterator it = m_extNodeList.begin(); it != xnEnd; ++it )
	{
// 		connect( *it, SIGNAL(moved(Node*)), this, SLOT(extNodeMoved()) );
		connect( *it, SIGNAL(removed(Node*)), this, SLOT(nodeRemoved(Node*)) );
	}
}


void NodeGroup::nodeRemoved( Node *node )
{
	// We are probably about to get deleted by ICNDocument anyway...so no point in doing anything
	m_nodeList.remove(node);
	node->setNodeGroup(0l);
	node->setVisible(true);
	m_extNodeList.remove(node);
}


void NodeGroup::connectorRemoved( Connector *connector )
{
	m_conList.remove(connector);
}


void NodeGroup::addExtNode( Node *node )
{
	if ( !m_extNodeList.contains(node) && !m_nodeList.contains(node) )
	{
		m_extNodeList.append(node);
		node->setNodeGroup(this);
	}
}

#include "nodegroup.moc"
