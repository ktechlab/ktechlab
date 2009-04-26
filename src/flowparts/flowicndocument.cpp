//
// C++ Implementation: flowicndocument
//
// Description: 
//
//
// Author: Zoltan P <zoltan.padrah@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "flowicndocument.h"

#include "connector.h"
#include "conrouter.h"
#include "cnitemgroup.h"
#include "fpnode.h"
#include "flowcontainer.h"
#include "item.h"
#include "junctionflownode.h"
#include "nodegroup.h"

#include <kdebug.h>

#include "ktlcanvas.h"

FlowICNDocument::FlowICNDocument( const QString &caption, const char *name)
	: ICNDocument(caption, name )
{
}

FlowICNDocument::~FlowICNDocument()
{
	// Go to hell, QCanvas. I'm in charge of what gets deleted.
	QCanvasItemList all = m_canvas->allItems();
	const QCanvasItemList::Iterator end = all.end();
	for ( QCanvasItemList::Iterator it= all.begin(); it != end; ++it )
		(*it)->setCanvas(0l);
	
	// Remove all items from the canvas
	selectAll();
	deleteSelection();
	
	// Delete anything that got through the above couple of lines
	ConnectorList connectorsToDelete = m_connectorList;
	connectorsToDelete.clear();
	const ConnectorList::iterator connectorListEnd = connectorsToDelete.end();
	for ( ConnectorList::iterator it = connectorsToDelete.begin(); it != connectorListEnd; ++it )
		delete *it;
		
	deleteAllNodes();
}

void FlowICNDocument::deleteAllNodes() {
	FPNodeMap nodesToDelete = m_flowNodeList;
	m_flowNodeList.clear();
	const FPNodeMap::iterator nodeListEnd = nodesToDelete.end();
	for ( FPNodeMap::iterator it = nodesToDelete.begin(); it != nodeListEnd; ++it )
		delete *it;	
}


bool FlowICNDocument::canConnect( QCanvasItem *qcanvasItem1, QCanvasItem *qcanvasItem2 ) const
{
	// Rough outline of what can and can't connect:
	// * At most three connectors to a node
	// * Can't have connectors going between different levels (e.g. can't have
	//   a connector coming outside a FlowContainer from inside).
	// * Can't have more than one route between any two nodes
	// * In all connections between nodes, must have at least one input and one
	//   output node at the ends.
	
	FPNode *startNode = dynamic_cast<FPNode*>(qcanvasItem1);
	FPNode *endNode = dynamic_cast<FPNode*>(qcanvasItem2);
	
	if ( (startNode && startNode->numCon( true, false ) > 2) || (endNode && endNode->numCon( true, false ) > 2) )
		return false;
	
	
	Connector *startConnector = dynamic_cast<Connector*>(qcanvasItem1);
	Connector *endConnector = dynamic_cast<Connector*>(qcanvasItem2);
	
	// Can't have I-junction in flowcode document
	if ( startConnector && endConnector )
		return false;
	
	
	//BEGIN Change connectors to nodes
	FPNode * startNode1 = 0l;
	FPNode * startNode2 = 0l;
	if (startConnector)
	{
		startNode1 = dynamic_cast<FPNode*> ( startConnector->startNode() );
		startNode2 = dynamic_cast<FPNode*> ( startConnector->endNode() );
		
		if ( !startNode1 || !startNode2 )
			return false;
	}
	else if (!startNode)
		return false;
	
	FPNode * endNode1 = 0l;
	FPNode * endNode2 = 0l;
	if (endConnector)
	{
		endNode1 = dynamic_cast<FPNode*> ( endConnector->startNode() );
		endNode2 = dynamic_cast<FPNode*> ( endConnector->endNode() );
		
		if ( !endNode1 || !endNode2 )
			return false;
	}
	else if ( !endNode )
		return false;
	
	//END Change connectors to nodes
	
	
	//BEGIN Check we have appropriate input and output allowance
	if ( type() == Document::dt_flowcode ) // this is obviouse
	{
		if ( startNode1 && startNode2 && endNode1 && endNode2 )
		{
			// Can't have I-configuration
			return false;
		}
		
		if ( startNode && endNode )
		{
			// Nice and easy straight line to check
			
			if ( !startNode->acceptInput() && !endNode->acceptInput() )
				return false;
	
			if ( !startNode->acceptOutput() && !endNode->acceptOutput() )
				return false;
		}
		
		else
		{
			// We're in a T-configuration, we can only make this if the base of
			// the T is an output
			FPNode * base = startNode ? startNode : endNode;
			if ( !base->acceptOutput() )
				return false;
		}
	} else
		kdError() << k_funcinfo << "BUG: document type is not dt_flowcode" << endl;
	//END Check we have appropriate input and output allowance

	return ICNDocument::canConnect(qcanvasItem1, qcanvasItem2);
}


Connector *FlowICNDocument::createConnector( Connector *con1, Connector *con2, const QPoint &pos1, const QPoint &pos2, QPointList *pointList )
{
	// FIXME isn't all this dead code?
	/*
	if ( !canConnect( con1, con2 ) )
		return 0l;
	
	
	const bool con1UsedManual = con1->usesManualPoints();
	const bool con2UsedManual = con2->usesManualPoints();
	
	QValueList<QPointList> oldCon1Points = con1->splitConnectorPoints(pos1);
	QValueList<QPointList> oldCon2Points = con2->splitConnectorPoints(pos2);
	
	
	// FIXME dynamic_cast used because Connector doesn't know about FPNode
	
	FPNode *node1a = dynamic_cast<FPNode*> ( con1->startNode() );
	FPNode *node1b = dynamic_cast<FPNode*> ( con1->endNode() );
	
	FPNode *node2a = dynamic_cast<FPNode*> ( con2->startNode() );
	FPNode *node2b = dynamic_cast<FPNode*> ( con2->endNode() );
	
	if ( !node1a || !node1b || !node2a || !node2b )
		return 0l;
	*/
	con1->hide();	
	con2->hide();
	
	// if ( type() != Document::dt_circuit )
	return 0;
}


Connector * FlowICNDocument::createConnector( Node *node, Connector *con, const QPoint &pos2, QPointList *pointList )
{
	if ( !canConnect( node, con ) )
		return 0l;
	
	// FIXME dynamic_cast used, fix it in Connector class
	
	FPNode *conStartNode = dynamic_cast<FPNode *> ( con->startNode() );
	FPNode *conEndNode = dynamic_cast<FPNode *> ( con->endNode() );
	
	FPNode *fpNode = dynamic_cast<FPNode *> ( node );
	
	const bool usedManual = con->usesManualPoints();
	
	FPNode *newNode = new JunctionFlowNode( this, 0, pos2 );
	
	QPointList autoPoints;
	if (!pointList)
	{
		addAllItemConnectorPoints();
		ConRouter cr(this);
		cr.mapRoute( int(node->x()), int(node->y()), pos2.x(), pos2.y() );
		autoPoints = cr.pointList(false);
		pointList = &autoPoints;
	}
	
	QValueList<QPointList> oldConPoints = con->splitConnectorPoints(pos2);
	con->hide();
	
	// The actual new connector
	Connector *new1 = newNode->createInputConnector(node);
	fpNode->addOutputConnector(new1);
	new1->setRoutePoints(*pointList,usedManual);
	
	// The two connectors formed from the original one when split
	Connector *new2 = newNode->createInputConnector(conStartNode);
	conStartNode->addOutputConnector(new2);
	new2->setRoutePoints( *oldConPoints.at(0), usedManual );
	
	Connector *new3 = conEndNode->createInputConnector(newNode);
	newNode->addOutputConnector(new3);
	new3->setRoutePoints( *oldConPoints.at(1), usedManual );
	
	// Avoid flicker: tell them to update their draw lists now
	con->updateConnectorPoints(false);
	new1->updateDrawList();
	new2->updateDrawList();
	new3->updateDrawList();
	
	// Now it's safe to remove the connector...
	con->removeConnector();
	flushDeleteList();
	
	deleteNodeGroup(conStartNode);
	deleteNodeGroup(conEndNode);
	createNodeGroup(newNode)->init();
	
	return new1;
}

Connector* FlowICNDocument::createConnector( const QString &startNodeId, const QString &endNodeId, QPointList *pointList )
{
	FPNode *startNode = getFPnodeWithID(startNodeId);
	FPNode *endNode = getFPnodeWithID(endNodeId);
	
	if ( !startNode || !endNode )
	{
		kdDebug() << "Either/both the connector start node and end node could not be found" << endl;
		return 0L;
	}
	
	if ( !canConnect( startNode, endNode ) )
		return 0l;	
	
	Connector *connector = endNode->createInputConnector(startNode);
	if (!connector)
	{
		kdError() << k_funcinfo << "End node did not create the connector" << endl;
		return 0l;
	}
	startNode->addOutputConnector(connector);
	flushDeleteList(); // Delete any connectors that might have been removed by the nodes
	
	// Set the route to the manual created one if the user created such a route
	if (pointList)
		connector->setRoutePoints(*pointList,true);
	
	// FIXME WTF is going on here? Redundant/meaningless code?
	ConnectorList connectorList;
	connectorList.append(connector);
	
	setModified(true);
	
	requestRerouteInvalidatedConnectors();
	return connector;
}



Node *FlowICNDocument::nodeWithID( const QString &id )
{
	if ( m_flowNodeList.contains( id ) )
		return m_flowNodeList[id];
	else	return 0l;
}

FPNode *FlowICNDocument::getFPnodeWithID( const QString &id )
{
	if ( m_flowNodeList.contains( id ) )
		return m_flowNodeList[id];
	else	return 0l;
}

void FlowICNDocument::slotAssignNodeGroups()
{
	ICNDocument::slotAssignNodeGroups();

	const FPNodeMap::iterator end = m_flowNodeList.end();
	for ( FPNodeMap::iterator it = m_flowNodeList.begin(); it != end; ++it )
	{
		NodeGroup *ng = createNodeGroup ( *it );
		if ( ng )
			ng->init();
	}

	// We've destroyed the old node groups, so any collapsed flowcontainers
	// containing new node groups need to update them to make them invisible.
	const ItemMap::const_iterator itemListEnd = m_itemList.end();
	for ( ItemMap::const_iterator it = m_itemList.begin(); it != itemListEnd; ++it )
	{
		if ( FlowContainer * fc = dynamic_cast<FlowContainer*> ( *it ) )
			fc->updateContainedVisibility();
	}
}

	
void FlowICNDocument::flushDeleteList()
{
	// Remove duplicate items in the delete list
	QCanvasItemList::iterator end = m_itemDeleteList.end();
	for ( QCanvasItemList::iterator it = m_itemDeleteList.begin(); it != end; ++it )
	{
		if ( *it && m_itemDeleteList.contains ( *it ) > 1 )
		{
			*it = 0l;
		}
	}
	m_itemDeleteList.remove ( 0l );

	end = m_itemDeleteList.end();
	for ( QCanvasItemList::iterator it = m_itemDeleteList.begin(); it != end; ++it )
	{
		QCanvasItem *qcanvasItem = *it;
		m_selectList->removeQCanvasItem ( *it );

		if ( Item *item = dynamic_cast<Item*> ( qcanvasItem ) )
			m_itemList.remove ( item->id() );

		else if ( FPNode * node = dynamic_cast<FPNode*> ( qcanvasItem ) )
			m_flowNodeList.remove ( node->id() );

		else if ( Connector * con = dynamic_cast<Connector*> ( qcanvasItem ) )
			m_connectorList.remove ( con );

		else
			kdError() << k_funcinfo << "Unknown qcanvasItem! "<<qcanvasItem << endl;

		qcanvasItem->setCanvas ( 0l );

		delete qcanvasItem;
		*it = 0l;
	}

 	// Check connectors for merging
	bool doneJoin = false;
	const FPNodeMap::iterator nlEnd = m_flowNodeList.end();
	for ( FPNodeMap::iterator it = m_flowNodeList.begin(); it != nlEnd; ++it )
	{
		( *it )->removeNullConnectors();
		int conCount = ( *it )->getAllConnectors().count();
		if ( conCount == 2 && ! ( *it )->parentItem() )
		{
			if ( joinConnectors ( *it ) )
				doneJoin = true;
		}
	}

	if ( doneJoin )
		flushDeleteList();

	requestRerouteInvalidatedConnectors();
}

bool FlowICNDocument::registerItem( QCanvasItem *qcanvasItem )
{
	if (!qcanvasItem)
		return false;
	
	if ( !ItemDocument::registerItem(qcanvasItem) )
	{
		if ( FPNode * node = dynamic_cast<FPNode*>(qcanvasItem) )
		{
			m_flowNodeList[ node->id() ] = node;
			emit nodeAdded( (Node*)node );
		}
		else if ( Connector * connector = dynamic_cast<Connector*>(qcanvasItem) )
		{
			m_connectorList.append(connector);
			emit connectorAdded(connector);
		}
		else
		{
			kdError() << k_funcinfo << "Unrecognised item"<<endl;
			return false;
		}
	}
	
	requestRerouteInvalidatedConnectors();
	
	return true;
}

void FlowICNDocument::unregisterUID( const QString & uid )
{
	m_flowNodeList.remove( uid );
	ICNDocument::unregisterUID( uid );
}

NodeList FlowICNDocument::nodeList( ) const
{
	NodeList l;
	
	FPNodeMap::const_iterator end = m_flowNodeList.end();
	for ( FPNodeMap::const_iterator it = m_flowNodeList.begin(); it != end; ++it )
		l << it.data();
	
	return l;
}

void FlowICNDocument::selectAllNodes() {
	const FPNodeMap::iterator nodeEnd = m_flowNodeList.end();
	for ( FPNodeMap::iterator nodeIt = m_flowNodeList.begin(); nodeIt != nodeEnd; ++nodeIt )
	{
		if (*nodeIt)
			select(*nodeIt);
	}
}

bool FlowICNDocument::joinConnectors( FPNode *node )
{
	// We don't want to destroy the node if it has a parent
	if ( node->parentItem() )
		return false;
	
	node->removeNullConnectors();
	
	int conCount = node->getAllConnectors().count();
	if ( conCount != 2 )
		return false;
	
	Connector *con1, *con2;
	Node *startNode, *endNode;
	QPointList conPoints;
	
	
	if ( node->inputConnectorList().count() == 0 )
	{
		// Both connectors emerge from node - output - i.e. node is pure start node
		con1 = *node->outputConnectorList().at(0);
		con2 = *node->outputConnectorList().at(1);
		if ( con1 == con2 ) {
			return false;
		}
		
		startNode = con1->endNode();
		endNode = con2->endNode();
		conPoints = con1->connectorPoints(true) + con2->connectorPoints(false);
	}
	else if ( node->inputConnectorList().count() == 1 )
	{
		// Ont input, one output
		con1 = *node->inputConnectorList().at(0);
		con2 = *node->outputConnectorList().at(0);
		if ( con1 == con2 ) {
			return false;
		}
		
		startNode = con1->startNode();
		endNode = con2->endNode();
		conPoints = con1->connectorPoints(false) + con2->connectorPoints(false);
	}
	else
	{
		// Both input - i.e. node is pure end node
		con1 = *node->inputConnectorList().at(0);
		con2 = *node->inputConnectorList().at(1);
		if ( con1 == con2 ) {
			return false;
		}
		
		startNode = con1->startNode();
		endNode = con2->startNode();
		conPoints = con1->connectorPoints(false) + con2->connectorPoints(true);
	}
	
	if ( !startNode || !endNode )
		return false;
	
	// HACK // FIXME // dynamic_cast used
	FPNode 	*startFpNode, *endFpNode;
	startFpNode = dynamic_cast<FPNode *> (startNode);
 	endFpNode = dynamic_cast<FPNode *> (endNode);
	
	Connector *newCon = endFpNode->createInputConnector(startFpNode);
	if (!newCon)
		return false;
	
	startFpNode->addOutputConnector(newCon);
	newCon->setRoutePoints( conPoints, con1->usesManualPoints() || con2->usesManualPoints() );
	
	// Avoid flicker: update draw lists now
	con1->updateConnectorPoints(false);
	con2->updateConnectorPoints(false);
	newCon->updateDrawList();
	
	node->removeNode();
	con1->removeConnector();
	con2->removeConnector();
	
	return true;
}

#include "flowicndocument.moc"
