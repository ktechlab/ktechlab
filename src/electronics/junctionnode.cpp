//
// C++ Implementation: junctionnode
//
// Description: 
//
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "junctionnode.h"

#include "pin.h"
#include "component.h"

#include <qpainter.h>


JunctionNode::JunctionNode(ICNDocument* icnDocument, int dir, const QPoint& pos, QString* id): 
		ECNode(icnDocument, Node::ec_junction, dir, pos, id)
{
    QString name("JunctionNode");
    if (id) {
        name.append(QString("-%1").arg(*id));
    } else {
        name.append("-Unknown");
    }
    setObjectName( name.toLatin1().data() );
}


JunctionNode::~JunctionNode()
{
}


void JunctionNode::drawShape( QPainter & p )
{
	initPainter( p );

	double v = pin() ? pin()->voltage() : 0.0;
	QColor voltageColor = Component::voltageColor( v );

	QPen pen = p.pen();

	if ( isSelected() )
		pen = m_selectedColor;
	else if ( m_bShowVoltageColor )
		pen = voltageColor;

	
	p.setPen( pen );
	p.setBrush( pen.color() );
	p.drawRect( -1, -1, 3, 3 );
	deinitPainter( p );
}


void JunctionNode::initPoints()
{
	setPoints( QPolygon( QRect( -4, -4, 8, 8 ) ) );
}
