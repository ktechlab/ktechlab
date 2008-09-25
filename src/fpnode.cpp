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
	// for InputFlowNode this member is overridden
	
	if( m_outputConnector)
		return 0;
	if( m_outputConnector->endNode() == 0)
		return 0;
	
	return (dynamic_cast<FPNode*>(m_outputConnector->endNode()))->outputFlowPart();
}


FlowPartList FPNode::inputFlowParts() const
{
	// for InputFlowNode it's overridden
	
	FlowPartList list;
	FlowPart *flowPart = dynamic_cast<FlowPart*>(parentItem());

	if( flowPart )
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

void FPNode::addOutputConnector( Connector * const connector )
{
	// for Junction and output flownodes
	if( !handleNewConnector(connector) )
		return ;

	if( m_outputConnector)
		kdError() << k_funcinfo << "BUG: adding an output connector when we already have one" << endl;
				
	m_outputConnector = connector;

}


void FPNode::addInputConnector( Connector * const connector )
{
	// for Junction and Input flownodes
	if( !handleNewConnector(connector) )
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
	if( (!acceptInput()) || !startNode )
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
	// for JunctionFlowNode this method is overridden!
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

