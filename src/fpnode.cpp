/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "icndocument.h"
#include "connector.h"
#include "flowpart.h"
#include "fpnode.h"

#include <kdebug.h>
#include <qpainter.h>

FPNode::FPNode( ICNDocument *icnDocument, Node::node_type type, node_dir dir, const QPoint &pos, QString *id )
	: Node( icnDocument, type, dir, pos, id )
{
	icnDocument->registerItem(this);
}


FPNode::~FPNode()
{
}


FlowPart *FPNode::outputFlowPart() const
{
	FlowPart *flowPart = dynamic_cast<FlowPart*>(parentItem());
	
	if ( type() == fp_in )
		return flowPart;
	
	if ( m_outputConnectorList.size() > 1 )
		kdError() << "FpNode::outputFlowPart(): outputConnectorList() size is greater than 1"<<endl;
	
	else if ( m_outputConnectorList.size() < 1 )
		return 0;
	
	ConnectorList::const_iterator it = m_outputConnectorList.begin();
	if ( it == m_outputConnectorList.end() || !*it || !(*it)->endNode()  )
		return 0;
	
	return (dynamic_cast<FPNode*>((*it)->endNode()))->outputFlowPart();
}


FlowPartList FPNode::inputFlowParts() const
{
	FlowPartList list;
	FlowPart *flowPart = dynamic_cast<FlowPart*>(parentItem());
	if ( type() != fp_in && flowPart )
	{
		list.append(flowPart);
		return list;
	}
	const ConnectorList::const_iterator end = m_inputConnectorList.end();
	for ( ConnectorList::const_iterator it = m_inputConnectorList.begin(); it != end; ++it )
	{
		if (*it)
		{
			Node *startNode = (*it)->startNode();
			FlowPart *flowPart = startNode ? dynamic_cast<FlowPart*>(startNode->parentItem()) : 0;
			if (flowPart)
				list.append(flowPart);
		}
	}
	return list;
}


inline QPointArray arrowPoints( Node::node_dir dir )
{
	QPointArray pa(3);
	switch (dir)
	{
		case Node::dir_right:
			pa[0] = QPoint( 3, 0 );
			pa[1] = QPoint( 0, 2 );
			pa[2] = QPoint( 0, -2 );
			break;
		case Node::dir_left:
			pa[0] = QPoint( -3, 0 );
			pa[1] = QPoint( 0, 2 );
			pa[2] = QPoint( 0, -2 );
			break;
		case Node::dir_down:
			pa[0] = QPoint( 2, 0 );
			pa[1] = QPoint( -2, 0 );
			pa[2] = QPoint( 0, 3 );
			break;
		case Node::dir_up:
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
	
	if ( type() == fp_junction && !m_inputConnectorList.isEmpty() )
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
			if ( end_0.x() < end_1.x() )
			{
				pa = arrowPoints( Node::dir_left );
				pa.translate( 4, 0 );
			}
			else if ( end_0.x() > end_1.x() )
			{
				pa = arrowPoints( Node::dir_right );
				pa.translate( -4, 0 );
			}
			else if ( end_0.y() < end_1.y() )
			{
				pa = arrowPoints( Node::dir_up );
				pa.translate( 0, 4 );
			}
			else if ( end_0.y() > end_1.y() )
			{
				pa = arrowPoints( Node::dir_down );
				pa.translate( 0, -4 );
			}
			else
				continue;
			
			pa.translate( _x, _y );
			p.setPen( connector->isSelected() ? m_selectedColor : Qt::black );
			p.drawPolygon(pa);
		}
		return;
	}
	
	if		( m_dir == Node::dir_right )	p.drawLine( _x, _y, _x-8, _y );
	else if ( m_dir == Node::dir_down )		p.drawLine( _x, _y, _x, _y-8 );
	else if ( m_dir == Node::dir_left )		p.drawLine( _x, _y, _x+8, _y );
	else if ( m_dir == Node::dir_up )		p.drawLine( _x, _y, _x, _y+8 );
	
	QPointArray pa(3);	
	
	// Right facing arrow
	if ( (type() == fp_out && m_dir == Node::dir_right) ||
			 (type() == fp_in && m_dir == Node::dir_left ) )
		pa = arrowPoints( Node::dir_right );
	
	// Left facing arrow
	else if ( (type() == fp_out && m_dir == Node::dir_left) ||
				  (type() == fp_in && m_dir == Node::dir_right) )
		pa = arrowPoints( Node::dir_left );
	
	// Down facing arrow
	else if ( (type() == fp_out && m_dir == Node::dir_down) ||
				  (type() == fp_in && m_dir == Node::dir_up) )
		pa = arrowPoints( Node::dir_down );
	
	// Up facing arrow
	else
		pa = arrowPoints( Node::dir_up );
	
	
	// Note: I have not tested the positioning of the arrows for all combinations.
	// In fact, most almost definitely do not work. So feel free to change the code
	// as you see fit if necessary.
	
	if ( type() == fp_out )
	{
		if		( m_dir == Node::dir_right ) pa.translate( -5, 0 );
		else if ( m_dir == Node::dir_down ) pa.translate( 0, -5 );
		else if ( m_dir == Node::dir_left ) pa.translate( 5, 0 );
		else if ( m_dir == Node::dir_up ) pa.translate( 0, 5 );
	}
	else if ( type() == fp_in )
	{
		if		( m_dir == Node::dir_right );
		else if ( m_dir == Node::dir_down );
		else if ( m_dir == Node::dir_left ) pa.translate( 3, 0 );
		else if ( m_dir == Node::dir_up ) pa.translate( 0, 3 );
	}
	else return;
	
	pa.translate( _x, _y );
	p.drawPolygon(pa);
}

#include "fpnode.moc"
