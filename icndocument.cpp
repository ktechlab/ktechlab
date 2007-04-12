/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "canvasmanipulator.h"
#include "component.h"
#include "connector.h"
#include "conrouter.h"
#include "cnitemgroup.h"
#include "ecnode.h"
#include "flowcontainer.h"
#include "fpnode.h"
#include "icndocument.h"
#include "icnview.h"
#include "itemdocumentdata.h"
#include "itemlibrary.h"
#include "ktechlab.h"
#include "nodegroup.h"

#include <kapplication.h>
#include <kdebug.h>
#include <qclipboard.h>
#include <qtimer.h>

ICNDocument::ICNDocument( const QString &caption, KTechlab *ktechlab, const char *name )
	: ItemDocument( caption, ktechlab, name ),
	m_cells(0)
{
	m_canvas->retune(48);
	m_selectList = new CNItemGroup(this);

	createCellMap();
	
	m_cmManager->addManipulatorInfo( CMItemMove::manipulatorInfo() );
	m_cmManager->addManipulatorInfo( CMAutoConnector::manipulatorInfo() );
	m_cmManager->addManipulatorInfo( CMManualConnector::manipulatorInfo() );
}


ICNDocument::~ICNDocument()
{
	m_bDeleted = true;

	// Go to hell, QCanvas. I'm in charge of what gets deleted.
	QCanvasItemList all = m_canvas->allItems();
	const QCanvasItemList::Iterator end = all.end();
	for ( QCanvasItemList::Iterator it= all.begin(); it != end; ++it )
		(*it)->setCanvas(0);

	// Remove all items from the canvas
	selectAll();
	deleteSelection();

	// Delete anything that got through the above couple of lines
	ConnectorList connectorsToDelete = m_connectorList;
	const ConnectorList::iterator connectorListEnd = connectorsToDelete.end();
	for ( ConnectorList::iterator it = connectorsToDelete.begin(); it != connectorListEnd; ++it )
		delete *it;
	
	NodeList nodesToDelete = m_nodeList;
	const NodeList::iterator nodeListEnd = nodesToDelete.end();
	for ( NodeList::iterator it = nodesToDelete.begin(); it != nodeListEnd; ++it )
		delete *it;
	
	GuardedNodeGroupList ngToDelete = m_nodeGroupList;
	const GuardedNodeGroupList::iterator nglEnd = ngToDelete.end();
	for ( GuardedNodeGroupList::iterator it = ngToDelete.begin(); it != nglEnd; ++it )
		delete *it;

	delete m_cells;
	delete m_selectList;
}

View *ICNDocument::createView( ViewContainer *viewContainer, uint viewAreaId, const char *name )
{
	ICNView *icnView = new ICNView( this, viewContainer, viewAreaId, name );
	handleNewView(icnView);
	return icnView;
}


ItemGroup* ICNDocument::selectList() const
{
	return m_selectList;
}


void ICNDocument::fillContextMenu( const QPoint &pos )
{
	ItemDocument::fillContextMenu(pos);
	slotInitItemActions( dynamic_cast<CNItem*>(m_selectList->activeItem()) );
}


CNItem* ICNDocument::cnItemWithID( const QString &id )
{
	return dynamic_cast<CNItem*>(itemWithID(id));
}


Node *ICNDocument::nodeWithID( const QString &id )
{
	const NodeList::iterator end = m_nodeList.end();
	for ( NodeList::iterator it = m_nodeList.begin(); it != end; ++it )
	{
		if ( (*it)->id() == id )
			return *it;
	}
	return 0;
}


Connector *ICNDocument::connectorWithID( const QString &id )
{
	const ConnectorList::iterator end = m_connectorList.end();
	for ( ConnectorList::iterator it = m_connectorList.begin(); it != end; ++it )
	{
		if ( (*it)->id() == id )
			return *it;
	}
	return 0;
}


FlowContainer *ICNDocument::flowContainer( const QPoint &pos )
{
	QCanvasItemList collisions = m_canvas->collisions(pos);
	FlowContainer *flowContainer = 0;
	int currentLevel = -1;
	const QCanvasItemList::iterator end = collisions.end();
	for ( QCanvasItemList::iterator it = collisions.begin(); it != end; ++it )
	{
		if ( FlowContainer *container = dynamic_cast<FlowContainer*>(*it) )
		{
			if ( container->level() > currentLevel && !m_selectList->contains(container) )
			{
				currentLevel = container->level();
				flowContainer = container;
			}
		}
	}
	return flowContainer;
}


bool ICNDocument::canConnect( QCanvasItem *qcanvasItem1, QCanvasItem *qcanvasItem2 ) const
{
	// Rough outline of what can and can't connect:
	// * At most three connectors to a node
	// * Can't have connectors going between different levels (e.g. can't have
	//   a connector coming outside a FlowContainer from inside).
	// * Can't have more than one route between any two nodes
	// * In all connections between nodes, must have at least one input and one
	//   output node at the ends.
	
	Node *startNode = dynamic_cast<Node*>(qcanvasItem1);
	Node *endNode = dynamic_cast<Node*>(qcanvasItem2);
	
	if ( (startNode && startNode->numCon( true, false ) > 2) || (endNode && endNode->numCon( true, false ) > 2) )
		return false;
	
	
	Connector *startConnector = dynamic_cast<Connector*>(qcanvasItem1);
	Connector *endConnector = dynamic_cast<Connector*>(qcanvasItem2);
	
	// Can't have T- or I- junction in PinMapEditor document
	if ( type() == Document::dt_pinMapEditor && (startConnector || endConnector) )
		return false;
	
	// Can't have I-junction in flowcode document
	if ( type() == Document::dt_flowcode && startConnector && endConnector )
		return false;
	
	
	//BEGIN Change connectors to nodes
	Node * startNode1 = 0;
	Node * startNode2 = 0;
	if (startConnector)
	{
		startNode1 = startConnector->startNode();
		startNode2 = startConnector->endNode();
		
		if ( !startNode1 || !startNode2 )
			return false;
	}
	else if (!startNode)
		return false;
	
	Node * endNode1 = 0;
	Node * endNode2 = 0;
	if (endConnector)
	{
		endNode1 = endConnector->startNode();
		endNode2 = endConnector->endNode();
		
		if ( !endNode1 || !endNode2 )
			return false;
	}
	else if ( !endNode )
		return false;
	
	Node * start[3];
	start[0] = startNode;
	start[1] = startNode1;
	start[2] = startNode2;
	
	Node * end[3];
	end[0] = endNode;
	end[1] = endNode1;
	end[2] = endNode2;
	//END Change connectors to nodes
	
	
	//BEGIN Check nodes aren't already connected
	for ( unsigned i = 0; i < 3; i++ )
	{
		for ( unsigned j = 0; j < 3; j++ )
		{
			if ( start[i] && end[j] && start[i]->isConnected(end[j]) )
				return false;
		}
	}
	//END Check nodes aren't already connected together
	
	
	//BEGIN Check we have appropriate input and output allowance
	if ( type() == Document::dt_flowcode )
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
			Node * base = startNode ? startNode : endNode;
			if ( !base->acceptOutput() )
				return false;
		}
	}
	//END Check we have appropriate input and output allowance

	
	//BEGIN Simple level check
	for ( unsigned i = 0; i < 3; i++ )
	{
		for ( unsigned j = 0; j < 3; j++ )
		{
			if ( start[i] && end[j] && start[i]->level() != end[j]->level() )
				return false;
		}
	}
	//END Simple level check
	
	
	//BEGIN Advanced level check
	CNItem * startParentItem[3];
	for ( unsigned i = 0; i < 3; i++ )
		startParentItem[i] = start[i] ? start[i]->parentItem() : 0;
	
	CNItem * endParentItem[3];
	for ( unsigned i = 0; i < 3; i++ )
		endParentItem[i] = end[i] ? end[i]->parentItem() : 0;
	
	Item * container[6] = {0};
	
	for ( unsigned i = 0; i < 3; i++ )
	{
		if (startParentItem[i])
		{
			int dl = start[i]->level() - startParentItem[i]->level();
			if ( dl == 0 )
				container[i] = startParentItem[i]->parentItem();
			else if ( dl == 1 )
				container[i] = startParentItem[i];
			else
				kdError() << k_funcinfo << " start, i="<<i<<" dl="<<dl<<endl;
		}
		if (endParentItem[i])
		{
			int dl = end[i]->level() - endParentItem[i]->level();
			if ( dl == 0 )
				container[i+3] = endParentItem[i]->parentItem();
			else if ( dl == 1 )
				container[i+3] = endParentItem[i];
			else
				kdError() << k_funcinfo << " end, i="<<i<<" dl="<<dl<<endl;
		}
	}
	
	// Everything better well have the same container...
	for ( unsigned i = 0; i < 6; ++i )
	{
		for ( unsigned j = 0; j < i; ++j )
		{
			Node * n1 = i < 3 ? start[i] : end[i-3];
			Node * n2 = j < 3 ? start[j] : end[j-3];
			if ( n1 && n2 && (container[i] != container[j]) )
				return false;
		}
	}
	//END Advanced level check
	
	
	// Well, it looks like we can, afterall, connect them...
	return true;
}



Connector * ICNDocument::createConnector( Node *startNode, Node *endNode, QPointList *pointList )
{
	if ( !canConnect( startNode, endNode ) )
		return 0;
	
	QPointList autoPoints;
	if (!pointList)
	{
		addAllItemConnectorPoints();
		ConRouter cr(this);
		cr.mapRoute( int(startNode->x()), int(startNode->y()), int(endNode->x()), int(endNode->y()) );
		autoPoints = cr.pointList(false);
		pointList = &autoPoints;
	}
	
	Connector * con = 0;
	
	// Check if we need to swap the ends around, and create the connector
	if ( endNode->type() == Node::fp_out )
		con = createConnector( endNode->id(), startNode->id(), pointList );
	else
		con = createConnector( startNode->id(), endNode->id(), pointList );
	
	bool startInGroup = deleteNodeGroup(startNode);
	bool endInGroup = deleteNodeGroup(endNode);
	if ( startInGroup || endInGroup )
	{
		NodeGroup *ng = createNodeGroup(startNode);
		ng->addNode( endNode, true );
		ng->init();
	}
	
	flushDeleteList();
	return con;
}

Connector * ICNDocument::createConnector( Node *node, Connector *con, const QPoint &pos2, QPointList *pointList )
{
	if ( !canConnect( node, con ) )
		return 0;
	
	Node *conStartNode = con->startNode();
	Node *conEndNode = con->endNode();
	
	const bool usedManual = con->usesManualPoints();
	
	Node *newNode = 0;
	if ( type() == Document::dt_circuit )
		newNode = new ECNode( this, Node::ec_junction, Node::dir_right, pos2 );
	
	else if ( type() == Document::dt_flowcode )
		newNode = new FPNode( this, Node::fp_junction, Node::dir_right, pos2 );
	
	else
		return 0;
	
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
	node->addOutputConnector(new1);
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

Connector * ICNDocument::createConnector( Connector *con1, Connector *con2, const QPoint &pos1, const QPoint &pos2, QPointList *pointList )
{
	if(!canConnect( con1, con2 )) return 0;

	const bool con1UsedManual = con1->usesManualPoints();
	const bool con2UsedManual = con2->usesManualPoints();

	QValueList<QPointList> oldCon1Points = con1->splitConnectorPoints(pos1);
	QValueList<QPointList> oldCon2Points = con2->splitConnectorPoints(pos2);

	Node *node1a = con1->startNode();
	Node *node1b = con1->endNode();

	Node *node2a = con2->startNode();
	Node *node2b = con2->endNode();

	if(!node1a || !node1b || !node2a || !node2b ) return 0;

	con1->hide();	
	con2->hide();
	
	if(type() != Document::dt_circuit) return 0;
	
	ECNode * newNode1 = new ECNode( this, Node::ec_junction, Node::dir_right, pos1 );
	ECNode * newNode2 = new ECNode( this, Node::ec_junction, Node::dir_right, pos2 );
	
	Connector *con1a = newNode1->createInputConnector(node1a);
	node1a->addOutputConnector(con1a);
	Connector *con1b = newNode1->createInputConnector(node1b);
	node1b->addOutputConnector(con1b);

	Connector *newCon = newNode1->createInputConnector(newNode2);
	newNode2->addOutputConnector(newCon);

	Connector *con2a = node2a->createInputConnector(newNode2);
	newNode2->addOutputConnector(con2a);
	Connector *con2b = node2b->createInputConnector(newNode2);
	newNode2->addOutputConnector(con2b);
	
	if ( !con1a || !con1b || !con2a || !con2b ) {
		// This should never happen, as the canConnect function should strictly
		// determine whether the connectors could be created before hand.
		kdWarning() << k_funcinfo << "Not all the connectors were created, this should never happen" << endl;
		
		if(con1a) con1a->removeConnector();
		if(con1b) con1b->removeConnector();
		if(con2a) con2a->removeConnector();
		if(con2b) con2b->removeConnector();
		
		newNode1->removeNode();
		newNode2->removeNode();
		
		flushDeleteList();
		return 0;
	}
	
	con1a->setRoutePoints( *oldCon1Points.at(0), con1UsedManual );
	con1b->setRoutePoints( *oldCon1Points.at(1), con1UsedManual );
	
	con2a->setRoutePoints( *oldCon2Points.at(0), con2UsedManual );
	con2b->setRoutePoints( *oldCon2Points.at(1), con2UsedManual );
	
	QPointList autoPoints;
	if (!pointList) {
		addAllItemConnectorPoints();
		ConRouter cr(this);
		cr.mapRoute( pos1.x(), pos1.y(), pos2.x(), pos2.y() );
		autoPoints = cr.pointList(false);
		pointList = &autoPoints;
	}
	newCon->setRoutePoints(*pointList,true);

	// Avoid flicker: tell them to update their draw lists now
	con1->updateConnectorPoints(false);
	con2->updateConnectorPoints(false);
	newCon->updateDrawList();
	con1a->updateDrawList();
	con1b->updateDrawList();
	con2a->updateDrawList();
	con2b->updateDrawList();

	// Now it's safe to remove the connectors
	con1->removeConnector();
	con2->removeConnector();

	flushDeleteList();

	deleteNodeGroup(node1a);
	deleteNodeGroup(node1b);
	deleteNodeGroup(node2a);
	deleteNodeGroup(node2b);
	NodeGroup *ng = createNodeGroup(newNode1);
	ng->addNode( newNode2, true );
	ng->init();

	return newCon;
}

NodeGroup* ICNDocument::createNodeGroup( Node *node )
{
	if(!node || node->isChildNode()) return 0;

	const GuardedNodeGroupList::iterator end = m_nodeGroupList.end();
	for ( GuardedNodeGroupList::iterator it = m_nodeGroupList.begin(); it != end; ++it )
	{
		if(*it && (*it)->contains(node)) return *it;

	}

	NodeGroup *group = new NodeGroup(this);
	m_nodeGroupList += group;
	group->addNode( node, true );

	return group;
}

bool ICNDocument::deleteNodeGroup( Node *node )
{
	if(!node || node->isChildNode()) return false;

	const GuardedNodeGroupList::iterator end = m_nodeGroupList.end();
	for ( GuardedNodeGroupList::iterator it = m_nodeGroupList.begin(); it != end; ++it ) {
		if ( *it && (*it)->contains(node) ) {
			delete *it;
			m_nodeGroupList.remove(it);
			return true;
		}
	}

	return false;
}

void ICNDocument::slotRequestAssignNG()
{
	requestEvent( ItemDocumentEvent::UpdateNodeGroups );
}

void ICNDocument::slotAssignNodeGroups()
{
	const GuardedNodeGroupList::iterator nglEnd = m_nodeGroupList.end();
	for ( GuardedNodeGroupList::iterator it = m_nodeGroupList.begin(); it != nglEnd; ++it ) delete *it;
	m_nodeGroupList.clear();

	const NodeList::iterator end = m_nodeList.end();
	for(NodeList::iterator it = m_nodeList.begin(); it != end; ++it) {
		NodeGroup *ng = createNodeGroup(*it);
		if (ng) ng->init();
	}
	
	// We've destroyed the old node groups, so any collapsed flowcontainers
	// containing new node groups need to update them to make them invisible.
	const ItemList::const_iterator itemListEnd = m_itemList.end();
	for ( ItemList::const_iterator it = m_itemList.begin(); it != itemListEnd; ++it )
	{
		if ( FlowContainer * fc = dynamic_cast<FlowContainer*>((Item*)*it) )
			fc->updateContainedVisibility();
	}
}

void ICNDocument::getTranslatable( const ItemList & itemList, ConnectorList * fixedConnectors, ConnectorList * translatableConnectors, NodeGroupList * translatableNodeGroups )
{
	ConnectorList tempCL1;
	if(!fixedConnectors) fixedConnectors = &tempCL1;

	ConnectorList tempCL2;
	if(!translatableConnectors ) translatableConnectors = &tempCL2;

	NodeGroupList tempNGL;
	if(!translatableNodeGroups) translatableNodeGroups = &tempNGL;

	// We record the connectors attached to the items, and
	// the number of times an item in the list is connected to
	// it - i.e. 1 or 2. For those with 2, it is safe to update their
	// route as it simply involves shifting the route
	typedef QMap< Connector*, int > ConnectorMap;
	ConnectorMap fixedConnectorMap;
	
	// This list of nodes is built up, used for later in determining fixed NodeGroups
	NodeList itemNodeList;
	{
		const ItemList::const_iterator itemListEnd = itemList.end();
		for ( ItemList::const_iterator it = itemList.begin(); it != itemListEnd; ++it ) {
			CNItem *cnItem = dynamic_cast<CNItem*>((Item*)*it);
			if ( !cnItem || !cnItem->canvas() ) continue;
			
			NodeMap nodeMap = cnItem->nodeMap();
			const NodeMap::iterator nlEnd = nodeMap.end();
			for ( NodeMap::iterator nlIt = nodeMap.begin(); nlIt != nlEnd; ++nlIt ) {
				itemNodeList.append(nlIt.data().node);
			}
			
			ConnectorList conList = cnItem->connectorList();
			conList.remove((Connector*)0);
			const ConnectorList::iterator clEnd = conList.end();
			for ( ConnectorList::iterator clit = conList.begin(); clit != clEnd; ++clit )
			{
				ConnectorMap::iterator cit = fixedConnectorMap.find(*clit);
				if ( cit != fixedConnectorMap.end() ) {
					cit.data()++;
				} else fixedConnectorMap[*clit] = 1;

			}
		}
	}
	
	// We now look through the NodeGroups to see if we have all the external
	// nodes for a given nodeGroup - if so, then the connectors in the fixed
	// connectors are ok to be moved
	ConnectorList fixedNGConnectors;
	{
		translatableNodeGroups->clear();
		const GuardedNodeGroupList::const_iterator end = m_nodeGroupList.end();
		for ( GuardedNodeGroupList::const_iterator it = m_nodeGroupList.begin(); it != end; ++it ) {
			NodeGroup *ng = *it;
			if (!ng) continue;

			NodeList externalNodeList = ng->externalNodeList();
			const NodeList::iterator itemNodeListEnd = itemNodeList.end();
			for ( NodeList::iterator inlIt = itemNodeList.begin(); inlIt != itemNodeListEnd; ++inlIt )
				externalNodeList.remove(*inlIt);

			if ( externalNodeList.isEmpty() ) {
				translatableNodeGroups->append(ng);

				const ConnectorList ngConnectorList = ng->connectorList();
				const ConnectorList::const_iterator ngConnectorListEnd = ngConnectorList.end();
				for ( ConnectorList::const_iterator ngclIt = ngConnectorList.begin(); ngclIt != ngConnectorListEnd; ++ngclIt ) {
					if (*ngclIt) fixedNGConnectors += *ngclIt;
				}
			}
		}
	}

	translatableConnectors->clear();
	const ConnectorMap::iterator fcEnd = fixedConnectorMap.end();
	for ( ConnectorMap::iterator it = fixedConnectorMap.begin(); it != fcEnd; ++it ) {
		// We allow it to be fixed if it is connected to two of the CNItems in the
		// select list, or is connected to itself (hence only appears to be connected to one,
		// but is fixed anyway
		Node *startNode = it.key()->endNode();
		Node *endNode = it.key()->startNode();
		if ( (it.data() > 1) || (startNode && endNode && startNode->parentItem() == endNode->parentItem())) {
			translatableConnectors->append( const_cast<Connector*>(it.key()) );
		} else if(!fixedNGConnectors.contains(it.key()) && !fixedConnectors->contains(it.key())) {
			fixedConnectors->append(it.key());
		}
	}
}

void ICNDocument::addCPenalty( int x, int y, int score )
{
	if ( isValidCellReference(x,y)) {
		(*m_cells)[x][y].Cpenalty += score;
	}
}

void ICNDocument::createCellMap()
{
	unsigned newCellsX = QMAX( canvas()->width()/cellSize, 1 );
	unsigned newCellsY = QMAX( canvas()->height()/cellSize, 1 );
	
	if ( m_cells && newCellsX == m_cellsX && newCellsY == m_cellsY )
		return;
	
	const ItemList::iterator ciEnd = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != ciEnd; ++it )
	{
		CNItem *cnItem = dynamic_cast<CNItem*>((Item*)(*it));
		if (cnItem)
			cnItem->updateConnectorPoints(false);
	}
	const ConnectorList::iterator conEnd = m_connectorList.end();
	for ( ConnectorList::iterator it = m_connectorList.begin(); it != conEnd; ++it ) {
		(*it)->updateConnectorPoints(false);
	}

	delete m_cells; // re-used two lines down. 
	m_cellsX = newCellsX;
	m_cellsY = newCellsY;
	m_cells = new Cells( m_cellsX, m_cellsY );

	for ( ConnectorList::iterator it = m_connectorList.begin(); it != conEnd; ++it )
		(*it)->updateConnectorPoints(true);
}


int ICNDocument::gridSnap( int pos )
{
	return pos-(pos%8)+4;
// 	return int((floor(pos/8))*8)+4;
}

QPoint ICNDocument::gridSnap( const QPoint &pos )
{
	return QPoint( gridSnap( pos.x() ), gridSnap( pos.y() ) );
}


void ICNDocument::appendDeleteList( QCanvasItem *qcanvasItem )
{
	if ( !qcanvasItem || m_itemDeleteList.findIndex(qcanvasItem) != -1 ) {
		return;
	}
	
	m_itemDeleteList.append(qcanvasItem);
	
	if ( qcanvasItem->rtti() == ItemDocument::RTTI::Node )
	{
		Node *node = dynamic_cast<Node*>(qcanvasItem);
		node->removeNode();
	}
	else if ( qcanvasItem->rtti() == ItemDocument::RTTI::CNItem ||
				 qcanvasItem->rtti() == ItemDocument::RTTI::DrawPart )
	{
		Item *item = dynamic_cast<Item*>(qcanvasItem);
		item->removeItem();
	}
	else if ( qcanvasItem->rtti() == ItemDocument::RTTI::Connector ||
				 qcanvasItem->rtti() == ItemDocument::RTTI::ConnectorLine )
	{
		Connector *connector = dynamic_cast<Connector*>(qcanvasItem);
		if (!connector)
			connector = (dynamic_cast<ConnectorLine*>(qcanvasItem))->parent();
		connector->removeConnector();
	}
	else
	{
		kdDebug() << k_funcinfo << "unrecognised QCanvasItem rtti " << QString::number(qcanvasItem->rtti()) << endl;
	}
}

void ICNDocument::flushDeleteList()
{
	// Remove duplicate items in the delete list
	QCanvasItemList::iterator end = m_itemDeleteList.end();
	for ( QCanvasItemList::iterator it = m_itemDeleteList.begin(); it != end; ++it )
	{
		if ( *it && m_itemDeleteList.contains(*it) > 1 ) {
			*it = 0;
		}
	}
	m_itemDeleteList.remove(0);
	
	end = m_itemDeleteList.end();
	for ( QCanvasItemList::iterator it = m_itemDeleteList.begin(); it != end; ++it )
	{
		QCanvasItem *qcanvasItem = *it;
		m_selectList->removeQCanvasItem(*it);
		
		if ( Item *item = dynamic_cast<Item*>(qcanvasItem) )
			m_itemList.remove(item);
		
		else if ( qcanvasItem->rtti() == ItemDocument::RTTI::Node )
			m_nodeList.remove( dynamic_cast<Node*>(qcanvasItem) );
		
		else if ( qcanvasItem->rtti() == ItemDocument::RTTI::Connector )
			m_connectorList.remove( dynamic_cast<Connector*>(qcanvasItem) );
		
		else
			kdError() << k_funcinfo << "Unknown qcanvasItem! "<<qcanvasItem << endl;
		
		qcanvasItem->setCanvas(0);
// FIXME: possible memory/pointer leak. Check code and explain in comment if
// correct. 
		delete qcanvasItem;
		*it = 0;
	}
	
// 	// Check connectors for merging
	bool doneJoin = false;
	const NodeList::iterator nlEnd = m_nodeList.end();
	for ( NodeList::iterator it = m_nodeList.begin(); it != nlEnd; ++it )
	{
		(*it)->removeNullConnectors();
		int conCount = (*it)->inputConnectorList().count() + (*it)->outputConnectorList().count();
		if ( conCount == 2 && !(*it)->parentItem() )
		{
			if ( joinConnectors(*it) )
				doneJoin = true;
		}
	}
	
	if (doneJoin)
		flushDeleteList();
	
	requestRerouteInvalidatedConnectors();
}



bool ICNDocument::joinConnectors( Node *node )
{
	// We don't want to destroy the node if it has a parent
	if ( node->parentItem() )
		return false;
	
	node->removeNullConnectors();
	
	int conCount = node->inputConnectorList().count() + node->outputConnectorList().count();
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
	
	Connector *newCon = endNode->createInputConnector(startNode);
	if (!newCon)
		return false;
	
	startNode->addOutputConnector(newCon);
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


bool ICNDocument::registerItem( QCanvasItem *qcanvasItem )
{
	if (!qcanvasItem)
		return false;
	
	if ( !ItemDocument::registerItem(qcanvasItem) )
	{
		switch (qcanvasItem->rtti())
		{
			case ItemDocument::RTTI::Node:
			{
				Node *node = (Node*)qcanvasItem;
				m_nodeList.append(node);
				connect( node, SIGNAL(removed(Node*)), this, SLOT(requestRerouteInvalidatedConnectors()) );
				emit nodeAdded(node);
				break;
			}
			case ItemDocument::RTTI::Connector:
			{
				Connector *connector = dynamic_cast<Connector*>(qcanvasItem);
				m_connectorList.append(connector);
				connect( connector, SIGNAL(removed(Connector*)), this, SLOT(requestRerouteInvalidatedConnectors()) );
				emit connectorAdded(connector);
				break;
			}
			default:
			{
				kdError() << k_funcinfo << "Unrecognised item rtti"<<endl;
				return false;
			}
		}
	}
	
	requestRerouteInvalidatedConnectors();
	
	return true;
}


void ICNDocument::copy()
{
	if ( m_selectList->isEmpty() )
		return;
	
	ItemDocumentData data( type() );
	
	// We only want to copy the connectors who have all ends attached to something in the selection
	ConnectorList connectorList = m_selectList->connectors(false);
	
	typedef QMap< Node*, ConnectorList > NCLMap;
	NCLMap nclMap;
	
	ConnectorList::iterator end = connectorList.end();
	for ( ConnectorList::iterator it = connectorList.begin(); it != end; ++it )
	{
		Node *startNode = (*it)->startNode();
		if ( startNode && !startNode->isChildNode() )
			nclMap[startNode].append(*it);
		
		Node *endNode = (*it)->endNode();
		if ( endNode && !endNode->isChildNode() )
			nclMap[endNode].append(*it);
	}
	
	NodeList nodeList;
	// Remove those connectors (and nodes) which are dangling on an orphan node
	NCLMap::iterator nclEnd = nclMap.end();
	for ( NCLMap::iterator it = nclMap.begin(); it != nclEnd; ++it )
	{
		if ( it.data().size() > 1 )
			nodeList.append(it.key());
		
		else if ( it.data().size() > 0 )
			connectorList.remove( it.data().at(0) );
	}
	
	data.addItems( m_selectList->items(false) );
	data.addNodes( nodeList );
	data.addConnectors( connectorList );
	
	KApplication::clipboard()->setText( data.toXML(), QClipboard::Clipboard );
}

void ICNDocument::selectAll()
{
	const NodeList::iterator nodeEnd = m_nodeList.end();
	for ( NodeList::iterator nodeIt = m_nodeList.begin(); nodeIt != nodeEnd; ++nodeIt )
	{
		if (*nodeIt)
			select(*nodeIt);
	}
	const ItemList::iterator itemEnd = m_itemList.end();
	for ( ItemList::iterator itemIt = m_itemList.begin(); itemIt != itemEnd; ++itemIt )
	{
		if (*itemIt)
			select(*itemIt);
	}
	const ConnectorList::iterator conEnd = m_connectorList.end();
	for ( ConnectorList::iterator connectorIt = m_connectorList.begin(); connectorIt != conEnd; ++connectorIt )
	{
		if (*connectorIt)
			select(*connectorIt);
	}
}


Item* ICNDocument::addItem( const QString &id, const QPoint &p, bool newItem )
{
	if ( !isValidItem(id) ) {
		return 0;
	}
	
	// First, we need to tell all containers to go to full bounding so that
	// we can detect a "collision" with them
	const ItemList::iterator end = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
	{
		if ( FlowContainer *flowContainer = dynamic_cast<FlowContainer*>((Item*)(*it)) )
			flowContainer->setFullBounds(true);
	}
	QCanvasItemList preCollisions = canvas()->collisions(p);
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
	{
		if ( FlowContainer *flowContainer = dynamic_cast<FlowContainer*>((Item*)(*it)) )
			flowContainer->setFullBounds(false);
	}
	
	
	Item *item = itemLibrary()->createItem( id, this, newItem );
	if (!item) return 0;
	
	// Look through the CNItems at the given point (sorted by z-coordinate) for
	// a container item.
	FlowContainer *container = 0;
	const QCanvasItemList::iterator pcEnd = preCollisions.end();
	for ( QCanvasItemList::iterator it = preCollisions.begin(); it != pcEnd && !container; ++it )
	{
		if ( FlowContainer *flowContainer = dynamic_cast<FlowContainer*>(*it) )
			container = flowContainer;
	}
	
	// We want to check it is not a special item first as
	// isValidItem may prompt the user about his bad choice
	if ( !isValidItem(item) )
	{
		item->removeItem();
		flushDeleteList();
		return 0;
	}
	
	int x = int(p.x());
	int y = int(p.y());
	
	if ( x < 16 || x > canvas()->width() )
		x = 16;
	if ( y < 16 || y > canvas()->height() )
		y = 16;
	
	if ( CNItem *cnItem = dynamic_cast<CNItem*>(item) )
	{
		cnItem->snap( x, y );
		
		if (container)
			container->addChild(cnItem);
	}
	else
		item->move( x, y );
	
	item->show();
	requestStateSave();
	return item;
}


void ICNDocument::addAllItemConnectorPoints()
{
	// FIXME The next line crashes sometimes??!
	const ItemList::iterator ciEnd = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != ciEnd; ++it )
	{
		if ( CNItem *cnItem = dynamic_cast<CNItem*>((Item*)(*it)) )
			cnItem->updateConnectorPoints(true);
	}
}


void ICNDocument::requestRerouteInvalidatedConnectors()
{
	requestEvent( ItemDocumentEvent::RerouteInvalidatedConnectors );
}
void ICNDocument::rerouteInvalidatedConnectors()
{
	qApp->processEvents(300);
	
	// We only ever need to add the connector points for CNItem's when we're about to reroute...
	addAllItemConnectorPoints();
	
	// List of connectors which are to be determined to need rerouting (and whose routes aren't controlled by NodeGroups)
	ConnectorList connectorRerouteList;
	
	// For those connectors that are controlled by node groups
	NodeGroupList nodeGroupRerouteList;
	
	const ConnectorList::iterator connectorListEnd = m_connectorList.end();
	for ( ConnectorList::iterator it = m_connectorList.begin(); it != connectorListEnd; ++it )
	{
		Connector *connector = *it;
		if ( connector && connector->isVisible() && connector->startNode() && connector->endNode() )
		{
			// Perform a series of tests to see if the connector needs rerouting
			bool needsRerouting = false;
			
			// Test to see if we actually have any points
			const QPointList pointList = connector->connectorPoints();
			if ( pointList.isEmpty() )
				needsRerouting = true;
			
			// Test to see if the route doesn't match up with the node positions at either end
			if (!needsRerouting)
			{
				const QPoint listStart = pointList.first();
				const QPoint listEnd = pointList.last();
				const QPoint nodeStart = QPoint( int(connector->startNode()->x()), int(connector->startNode()->y()) );
				const QPoint nodeEnd = QPoint( int(connector->endNode()->x()), int(connector->endNode()->y()) );
				
				if ( ((listStart != nodeStart) || (listEnd != nodeEnd)) &&
								   ((listStart != nodeEnd) || (listEnd != nodeStart)) )
				{
					needsRerouting = true;
// 					kdDebug() << "listStart=("<<listStart.x()<<","<<listStart.y()<<") nodeStart=("<<nodeStart.x()<<","<<nodeStart.y()<<") listEnd=("<<listEnd.x()<<","<<listEnd.y()<<") nodeEnd=("<<nodeEnd.x()<<","<<nodeEnd.y()<<")"<<endl;
				}
			}
			
			// Test to see if the route intersects any Items (we ignore if it is a manual route)
			if ( !needsRerouting && !connector->usesManualPoints() )
			{
				const QCanvasItemList collisions = connector->collisions(true);
				const QCanvasItemList::const_iterator collisionsEnd = collisions.end();
				for ( QCanvasItemList::const_iterator collisionsIt = collisions.begin(); (collisionsIt != collisionsEnd) && !needsRerouting; ++collisionsIt )
				{
					if ( dynamic_cast<Item*>(*collisionsIt) )
						needsRerouting = true;
				}
			}
			
			if (needsRerouting)
			{
				NodeGroup *nodeGroup = connector->nodeGroup();
				
				if ( !nodeGroup && !connectorRerouteList.contains(connector) )
					connectorRerouteList.append(connector);
				
				else if ( nodeGroup && !nodeGroupRerouteList.contains(nodeGroup) )
					nodeGroupRerouteList.append(nodeGroup);
			}
		}
	}
	
	// To allow proper rerouting, we want to start with clean routes for all of the invalidated connectors
	const NodeGroupList::iterator nodeGroupRerouteEnd = nodeGroupRerouteList.end();
	for ( NodeGroupList::iterator it = nodeGroupRerouteList.begin(); it != nodeGroupRerouteEnd; ++it )
	{
		const ConnectorList contained = (*it)->connectorList();
		const ConnectorList::const_iterator end = contained.end();
		for ( ConnectorList::const_iterator it = contained.begin(); it != end; ++it )
			(*it)->updateConnectorPoints(false);
	}
	
	const ConnectorList::iterator connectorRerouteEnd = connectorRerouteList.end();
	for ( ConnectorList::iterator it = connectorRerouteList.begin(); it != connectorRerouteEnd; ++it )
		(*it)->updateConnectorPoints(false);
	
	// And finally, reroute the connectors
	for ( NodeGroupList::iterator it = nodeGroupRerouteList.begin(); it != nodeGroupRerouteEnd; ++it )
		(*it)->updateRoutes();
	
	for ( ConnectorList::iterator it = connectorRerouteList.begin(); it != connectorRerouteEnd; ++it )
		(*it)->rerouteConnector();
	
	for ( ConnectorList::iterator it = m_connectorList.begin(); it != connectorListEnd; ++it )
	{
		if (*it)
			(*it)->updateDrawList();
	}
}


Connector* ICNDocument::createConnector( const QString &startNodeId, const QString &endNodeId, QPointList *pointList )
{
	Node *startNode = nodeWithID(startNodeId);
	Node *endNode = nodeWithID(endNodeId);
	
	if ( !startNode || !endNode )
	{
		kdDebug() << "Either/both the connector start node and end node could not be found" << endl;
		return 0;
	}
	
	Connector *connector = endNode->createInputConnector(startNode);
	if (!connector)
	{
		kdError() << k_funcinfo << "End node did not create the connector" << endl;
		return 0;
	}
	startNode->addOutputConnector(connector);
	flushDeleteList(); // Delete any connectors that might have been removed by the nodes
	
	// Set the route to the manual created one if the user created such a route
	if (pointList)
		connector->setRoutePoints(*pointList,true);
	
	ConnectorList connectorList;
	connectorList.append(connector);
	
	setModified(true);
	
	requestRerouteInvalidatedConnectors();
	return connector;
}

void ICNDocument::deleteSelection()
{
	// End whatever editing mode we are in, as we don't want to start editing
	// something that is about to no longer exist...
	m_cmManager->cancelCurrentManipulation();
	
	if ( m_selectList->isEmpty() )
		return;
	
	m_selectList->deleteAllItems();
	flushDeleteList();
	setModified(true);
	
	// We need to emit this so that property widgets etc...
	// can clear themselves.
	emit itemUnselected(0);
	
	requestRerouteInvalidatedConnectors();
	requestStateSave();
}


ConnectorList ICNDocument::getCommonConnectors( const ItemList &list )
{
	NodeList nodeList = getCommonNodes(list);
	
	// Now, get all the connectors, and remove the ones that don't have both end
	// nodes in the above generated list
	ConnectorList connectorList = m_connectorList;
	const ConnectorList::iterator connectorListEnd = connectorList.end();
	for ( ConnectorList::iterator it = connectorList.begin(); it != connectorListEnd; ++it )
	{
		Connector *con = *it;
		if ( !con || !nodeList.contains(con->startNode()) || !nodeList.contains(con->endNode()) ) {
			*it = 0;
		}
	}
	connectorList.remove((Connector*)0);
	return connectorList;
}


NodeList ICNDocument::getCommonNodes( const ItemList &list )
{
	NodeList nodeList;
	
	const ItemList::const_iterator listEnd = list.end();
	for ( ItemList::const_iterator it = list.begin(); it != listEnd; ++it )
	{
		NodeMap nodeMap;
		CNItem *cnItem = dynamic_cast<CNItem*>((Item*)*it);
		if (cnItem)
			nodeMap = cnItem->nodeMap();
		const NodeMap::iterator nodeMapEnd = nodeMap.end();
		for ( NodeMap::iterator it = nodeMap.begin(); it != nodeMapEnd; ++it )
		{
			Node *node = it.data().node;
			
			if ( !nodeList.contains(node) ) {
				nodeList += node;
			}
			
			NodeGroup *ng = node->nodeGroup();
			if (ng)
			{
				NodeList intNodeList = ng->internalNodeList();
				const NodeList::iterator intNodeListEnd = intNodeList.end();
				for ( NodeList::iterator it = intNodeList.begin(); it != intNodeListEnd; ++it )
				{
					Node *intNode = *it;
					if ( !nodeList.contains(intNode) ) {
						nodeList += intNode;
					}
				}
			}
		}
	}
	
	return nodeList;
}



DirCursor *DirCursor::m_self = 0;

DirCursor::DirCursor()
{
	initCursors();
}

DirCursor::~DirCursor()
{
}

DirCursor* DirCursor::self()
{
  if (!m_self) m_self = new DirCursor;
  return m_self;
}

void DirCursor::initCursors()
{
// 	QCursor c(Qt::ArrowCursor);
// 	QBitmap bitmap = *c.bitmap();
// 	QBitmap mask = *c.mask();
// 	QPixmap pm( bitmap->width(), bitmap->height() );
// 	pm.setMask(mask);
// 	pm = c.pi
	// @todo finish
}


#include "icndocument.moc"
