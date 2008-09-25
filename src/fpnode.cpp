/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "connector.h"
#include "flowpart.h"
#include "fpnode.h"
#include "icndocument.h"
#include "inputflownode.h"
#include "junctionflownode.h"
#include "outputflownode.h"

#include <kdebug.h>
#include <qpainter.h>

FPNode::FPNode( ICNDocument *icnDocument, Node::node_type type, int dir, const QPoint &pos, QString *id )
	: Node( icnDocument, type, dir, pos, id )
{
	if ( icnDocument )
		icnDocument->registerItem(this);
	m_outputConnector = 0;
}


FPNode::~FPNode()
{
}


FlowPart *FPNode::outputFlowPart() const
{
	FlowPart *flowPart = dynamic_cast<FlowPart*>(parentItem());

		// FIXME dynamic_cast used
	if( dynamic_cast<const InputFlowNode*>(this) != 0)
		return flowPart;
	
	if( m_outputConnector)
		return 0;
	if( m_outputConnector->endNode() == 0)
		return 0;
	
	return (dynamic_cast<FPNode*>(m_outputConnector->endNode()))->outputFlowPart();
}


FlowPartList FPNode::inputFlowParts() const
{
	FlowPartList list;
	FlowPart *flowPart = dynamic_cast<FlowPart*>(parentItem());

		// FIXME dynamic_cast used
	if( flowPart && ( dynamic_cast<const InputFlowNode*>(this) == 0 ))
	{
		list.append(flowPart);
		return list;
	}

	const ConnectorList::const_iterator end = m_inputConnectorList.end();
	for ( ConnectorList::const_iterator it = m_inputConnectorList.begin(); it != end; ++it )
	{
		if (*it) {
			Node *startNode = (*it)->startNode();
			FlowPart *flowPart = startNode ? dynamic_cast<FlowPart*>(startNode->parentItem()) : 0l;
			if (flowPart)
				list.append(flowPart);
		}
	}

	return list;
}


inline QPointArray arrowPoints( int dir )
{
	QPointArray pa(3);
	switch ( dir ) {
		case 0:
			pa[0] = QPoint( 3, 0 );
			pa[1] = QPoint( 0, 2 );
			pa[2] = QPoint( 0, -2 );
			break;
		case 180:
			pa[0] = QPoint( -3, 0 );
			pa[1] = QPoint( 0, 2 );
			pa[2] = QPoint( 0, -2 );
			break;
		case 90:
			pa[0] = QPoint( 2, 0 );
			pa[1] = QPoint( -2, 0 );
			pa[2] = QPoint( 0, 3 );
			break;
		case 270:
			pa[0] = QPoint( 2, 0 );
			pa[1] = QPoint( -2, 0 );
			pa[2] = QPoint( 0, -3 );
			break;
	};
	return pa;
}


void FPNode::drawShape( QPainter &p )
{
	const int _x = (int)x();
	const int _y = (int)y();
	
		// FIXME dynamic_cast used
	if( (dynamic_cast<JunctionFlowNode*>(this) != 0) && ( !m_inputConnectorList.isEmpty() ) )
	{
		const ConnectorList::iterator end = m_inputConnectorList.end();
		for ( ConnectorList::iterator it = m_inputConnectorList.begin(); it != end; ++ it)
		{
			Connector * connector = *it;
			if (!connector)
				continue;

			// Work out the direction of the connector
			const QPointList points = connector->connectorPoints(false);

			const int count = points.size();
			if ( count < 2 )
				continue;

			QPoint end_0 = points[count-1];
			QPoint end_1 = points[count-2];

			QPointArray pa;
			if ( end_0.x() < end_1.x() ) {
				pa = arrowPoints( 180 );
				pa.translate( 4, 0 );
			} else if ( end_0.x() > end_1.x() ) {
				pa = arrowPoints( 0 );
				pa.translate( -4, 0 );
			} else if ( end_0.y() < end_1.y() ) {
				pa = arrowPoints( 270 );
				pa.translate( 0, 4 );
			} else if ( end_0.y() > end_1.y() ) {
				pa = arrowPoints( 90 );
				pa.translate( 0, -4 );
			} else	continue;
			
			pa.translate( _x, _y );
			p.setPen( connector->isSelected() ? m_selectedColor : Qt::black );
			p.drawPolygon(pa);
		}
		return;
	}
	
	if	( m_dir == 0 )		p.drawLine( _x, _y, _x-8, _y );
	else if ( m_dir == 90 )		p.drawLine( _x, _y, _x, _y-8 );
	else if ( m_dir == 180 )	p.drawLine( _x, _y, _x+8, _y );
	else if ( m_dir == 270 )	p.drawLine( _x, _y, _x, _y+8 );
	
	QPointArray pa(3);	
	
	// FIXME 2 dynamic_cast-s

	// input node
	if( dynamic_cast<InputFlowNode*>(this) != 0 ){
		switch( m_dir ){
			case 180: // right
				pa = arrowPoints( 0 );
				break;
			case 0: // left
				pa = arrowPoints( 180 );
				break;
			case 270: // down
				pa = arrowPoints( 90 );
				break;
			case 90: // up
				pa = arrowPoints( 270 );
				break;
			default:
				kdError() << k_funcinfo << "BUG: m_dir = " << m_dir << endl;
		}
	}
	//output node
	if( dynamic_cast<OutputFlowNode*>(this) != 0){
		switch( m_dir ){
			case 0: // right
				pa = arrowPoints( 0 );
				break;
			case 180: // left
				pa = arrowPoints( 180 );
				break;
			case 90: // down
				pa = arrowPoints( 90 );
				break;
			case 270: // up
				pa = arrowPoints( 270 );
				break;
			default:
				kdError() << k_funcinfo << "BUG: m_dir = " << m_dir << endl;
		}
	}
	
	// Note: I have not tested the positioning of the arrows for all combinations.
	// In fact, most almost definitely do not work. So feel free to change the code
	// as you see fit if necessary.
	
	// FIXME 2 dynamic_cast-s
	
	if ( dynamic_cast<OutputFlowNode*>(this) != 0 )
	{
		if		( m_dir == 0 ) pa.translate( -5, 0 );
		else if ( m_dir == 90 ) pa.translate( 0, -5 );
		else if ( m_dir == 180 ) pa.translate( 5, 0 );
		else if ( m_dir == 270 ) pa.translate( 0, 5 );
	} else if ( dynamic_cast<InputFlowNode*>(this) != 0 ) {
		if		( m_dir == 0 );
		else if ( m_dir == 90 );
		else if ( m_dir == 180 ) pa.translate( 3, 0 );
		else if ( m_dir == 270 ) pa.translate( 0, 3 );
	} else return;
	
	pa.translate( _x, _y );
	p.drawPolygon(pa);
}

// -- functionality from node.cpp --

bool FPNode::acceptInput() const
{
	// FIXME dynamic_cast
	// return type() != fp_out;
	return (  dynamic_cast<const OutputFlowNode*>(this) == 0 );
}


bool FPNode::acceptOutput() const
{
	// FIXME dynamic_cast
	return (  dynamic_cast<const InputFlowNode*>(this) == 0 );
	// return type() != fp_in;
}


void FPNode::addOutputConnector( Connector * const connector )
{
	// FIXME dynamic_cast-s
	if( (dynamic_cast<InputFlowNode*>(this) != 0 ) || !handleNewConnector(connector) )
		return ;

	m_outputConnector = connector;

}


void FPNode::addInputConnector( Connector * const connector )
{
	// FIXME dynamic_cast
	if( (dynamic_cast<OutputFlowNode*>(this) != 0) || !handleNewConnector(connector) )
		return;
	
	m_inputConnectorList.append(connector);
}


bool FPNode::handleNewConnector( Connector * connector )
{
	if (!connector)
		return false;

	if ( m_inputConnectorList.contains(connector) || ((Connector*)m_outputConnector == connector) )
	{
		kdWarning() << k_funcinfo << " Already have connector = " << connector << endl;
		return false;
	}

	connect( this, SIGNAL(removed(Node*)), connector, SLOT(removeConnector(Node*)) );
	connect( connector, SIGNAL(removed(Connector*)), this, SLOT(checkForRemoval(Connector*)) );
	connect( connector, SIGNAL(selected(bool)), this, SLOT(setNodeSelected(bool)) );

	if ( !isChildNode() )
		p_icnDocument->slotRequestAssignNG();

	return true;
}


Connector* FPNode::createInputConnector( Node * startNode )
{
	// FIXME dynamic_cast
	if( (dynamic_cast<OutputFlowNode*>(this) != 0) || !startNode )
		return 0l;
	
	Connector *connector = new Connector( startNode, this, p_icnDocument );
	addInputConnector(connector);
	
	return connector;
}


int FPNode::numCon( bool includeParentItem, bool includeHiddenConnectors ) const
{
	unsigned count = 0;
	
	ConnectorList connectors = m_inputConnectorList;
	if ( m_outputConnector )
		connectors.append ( m_outputConnector );
	
	ConnectorList::const_iterator end = connectors.end();
	for ( ConnectorList::const_iterator it = connectors.begin(); it != end; ++it )
	{
		if ( *it && ( includeHiddenConnectors || ( *it )->canvas() ) )
			count++;
	}
	
	if ( isChildNode() && includeParentItem )
		count++;

	return count;
}


void FPNode::removeConnector( Connector *connector )
{
	if (!connector) return;
	
	ConnectorList::iterator it;
	
	it = m_inputConnectorList.find(connector);
	if ( it != m_inputConnectorList.end() )
	{
		(*it)->removeConnector();
		(*it) = 0L;
	}
	
	if((Connector *)m_outputConnector == connector)
	{
		connector->removeConnector();
		m_outputConnector = 0;
	}
}


void FPNode::checkForRemoval( Connector *connector )
{
	removeConnector(connector);
	setNodeSelected(false);
	
	removeNullConnectors();
	
	if (!p_parentItem) {
		int conCount = m_inputConnectorList.count();
		if( m_outputConnector)
			conCount++;
		if ( conCount < 2 )
			removeNode();
	}
	
	// FIXME dynamic_cast again
	if( (dynamic_cast<JunctionFlowNode*>(this) != 0) && (!m_outputConnector) )
		removeNode();
}


void FPNode::removeNullConnectors()
{
	m_inputConnectorList.remove((Connector*)0L);
}


QPoint FPNode::findConnectorDivergePoint( bool * found )
{
	bool temp;
	if (!found)
		found = &temp;
	*found = false;

	if ( numCon( false, false ) != 2 )
		return QPoint(0,0);

	QPointList p1;
	QPointList p2;

	int inSize = m_inputConnectorList.count();

	ConnectorList connectors = m_inputConnectorList;
	if(m_outputConnector)
		connectors.append(m_outputConnector);
	
	const ConnectorList::const_iterator end = connectors.end();
	bool gotP1 = false;
	bool gotP2 = false;
	int at = -1;
	for ( ConnectorList::const_iterator it = connectors.begin(); it != end && !gotP2; ++it )
	{
		at++;
		if ( !(*it) || !(*it)->canvas() )
			continue;

		if (gotP1) {
			p2 = (*it)->connectorPoints( at < inSize );
			gotP2 = true;
		} else {
			p1 = (*it)->connectorPoints( at < inSize );
			gotP1 = true;
		}
	}

	if ( !gotP1 || !gotP2 )
		return QPoint(0,0);
	
	unsigned maxLength = p1.size() > p2.size() ? p1.size() : p2.size();
	
	for ( unsigned i = 1; i < maxLength; ++i )
	{
		if ( p1[i] != p2[i] ) {
			*found = true;
			return p1[i-1];
		}
	}
	return QPoint(0, 0);
}


void FPNode::setVisible( bool yes )
{
	if ( isVisible() == yes ) return;
	
	QCanvasPolygon::setVisible(yes);
	
	const ConnectorList::iterator inputEnd = m_inputConnectorList.end();
	for ( ConnectorList::iterator it = m_inputConnectorList.begin(); it != inputEnd; ++it )
	{
		Connector *connector = *it;
		if ( connector )
		{
			if ( isVisible() )
				connector->setVisible ( true );
			else
			{
				Node *node = connector->startNode();
				connector->setVisible ( node && node->isVisible() );
			}
		}
	}
	
	Connector *connector = m_outputConnector; 
	if ( connector )
	{
		if ( isVisible() )
			connector->setVisible ( true );
		else
		{
			Node *node = connector->endNode();
			connector->setVisible ( node && node->isVisible() );
		}
	}
}

bool FPNode::isConnected( Node *node, NodeList *checkedNodes )
{
	if ( this == node )
		return true;

	bool firstNode = !checkedNodes;
	if (firstNode)
		checkedNodes = new NodeList();

	else if ( checkedNodes->contains(this) )
		return false;

	checkedNodes->append(this);

	const ConnectorList::const_iterator inputEnd = m_inputConnectorList.end();
	for ( ConnectorList::const_iterator it = m_inputConnectorList.begin(); it != inputEnd; ++it )
	{
		Connector *connector = *it;
		if (connector) {
			Node *startNode = connector->startNode();
			if ( startNode && startNode->isConnected( node, checkedNodes ) ) {
				if (firstNode) {
					delete checkedNodes;
				}
				return true;
			}
		}	
	}
	
	Connector *connector = m_outputConnector;
	if ( connector )
	{
		Node *endNode = connector->endNode();
		if ( endNode && endNode->isConnected ( node, checkedNodes ) )
		{
			if ( firstNode )
			{
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

ConnectorList FPNode::outputConnectorList() const 
{  
	ConnectorList out;
	if( m_outputConnector)
		out.append(m_outputConnector);
	return out;
}

ConnectorList FPNode::getAllConnectors() const
{
	ConnectorList all = m_inputConnectorList ;
	if ( m_outputConnector )
		all.append ( m_outputConnector );
	return all;
}

Connector* FPNode::getAConnector() const {
	if( ! m_inputConnectorList.isEmpty() )
		return *m_inputConnectorList.begin();

	if( m_outputConnector)
		return m_outputConnector;
	
	return 0l;
}

#include "fpnode.moc"

