//
// C++ Implementation: inputflownode
//
// Description: 
//
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "connector.h"
#include "inputflownode.h"
#include "flowconnector.h"
#include "flowpart.h"

#include <kdebug.h>
#include <Qt/qpainter.h>

InputFlowNode::InputFlowNode(ICNDocument *icnDocument, int dir, const QPoint &pos, QString *id)
 : FPNode(icnDocument, Node::fp_in, dir, pos, id)
{
}


InputFlowNode::~InputFlowNode()
{
}

FlowPart *InputFlowNode::outputFlowPart() const
{
	return dynamic_cast<FlowPart*>(parentItem());
}


FlowPartList InputFlowNode::inputFlowParts() const
{
	FlowPartList list;

	const FlowConnectorList::const_iterator end = m_inFlowConnList.end();
	for ( FlowConnectorList::const_iterator it = m_inFlowConnList.begin(); it != end; ++it )
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


bool InputFlowNode::acceptInput() const
{
	return true;
}


bool InputFlowNode::acceptOutput() const
{
	return false;
}

void InputFlowNode::addOutputConnector( Connector * const connector )
{
	kdError() << k_funcinfo << "BUG: adding output connector to an input node" << endl;
}


inline Q3PointArray arrowPoints( int dir )
{
	Q3PointArray pa(3);
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

void InputFlowNode::drawShape ( QPainter &p )
{
	const int _x = ( int ) x();
	const int _y = ( int ) y();

	if	( m_dir == 0 )		p.drawLine ( _x, _y, _x-8, _y );
	else if ( m_dir == 90 )		p.drawLine ( _x, _y, _x, _y-8 );
	else if ( m_dir == 180 )	p.drawLine ( _x, _y, _x+8, _y );
	else if ( m_dir == 270 )	p.drawLine ( _x, _y, _x, _y+8 );

	Q3PointArray pa ( 3 );

	switch ( m_dir )
	{
		case 180: // right
			pa = arrowPoints ( 0 );
			break;
		case 0: // left
			pa = arrowPoints ( 180 );
			break;
		case 270: // down
			pa = arrowPoints ( 90 );
			break;
		case 90: // up
			pa = arrowPoints ( 270 );
			break;
		default:
			kdError() << k_funcinfo << "BUG: m_dir = " << m_dir << endl;
	}

	// Note: I have not tested the positioning of the arrows for all combinations.
	// In fact, most almost definitely do not work. So feel free to change the code
	// as you see fit if necessary.

	if ( m_dir == 0 );
	else if ( m_dir == 90 );
	else if ( m_dir == 180 ) pa.translate ( 3, 0 );
	else if ( m_dir == 270 ) pa.translate ( 0, 3 );

	pa.translate ( _x, _y );
	p.drawPolygon ( pa );
}

