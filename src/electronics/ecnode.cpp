/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitdocument.h"
#include "src/core/ktlconfig.h"
#include "component.h"
#include "connector.h"
#include "ecnode.h"
#include "pin.h"

#include <kdebug.h>
#include <qpainter.h>

#include <cmath>

// The voltage at the middle of the voltage indicator
const double vMidPoint = 5.;

// The maximum length of the voltage indiactor
const int vLength = 10;

// The current at the middle of the current indicator
const double iMidPoint = 0.05;

// The maximum thicnkess of the current indicator
const int iLength = 6;

inline double calcVProp( const double v)
{
	return 1 - vMidPoint/(vMidPoint+std::abs(v));
}

inline double calcIProp( const double i)
{
	return 1 - iMidPoint/(iMidPoint+std::abs(i));
}

inline int calcThickness( const double prop)
{
	return (int)((iLength-2)*prop+2);
}

inline int calcLength( const double prop, const double v)
{ 
	return (v>0) ? -int(vLength*prop) : int(vLength*prop);
}
 
ECNode::ECNode( ICNDocument *icnDocument, Node::node_type _type, node_dir dir, const QPoint &pos, QString *_id)
	: Node( icnDocument, _type, dir, pos, _id)
{
	m_prevV = 0;
	m_prevI = 0;
	m_pinPoint = 0;
	m_bShowVoltageBars = KTLConfig::showVoltageBars();

	icnDocument->registerItem(this);

	if( type() == ec_pin) {
		m_pinPoint = new QCanvasRectangle( 0, 0, 3, 3, canvas());
		m_pinPoint->setBrush(Qt::black);
		m_pinPoint->setPen(Qt::black);
	}
	
	m_pins.resize(1);
	m_pins[0] = new Pin(this);
}


ECNode::~ECNode()
{
	if(m_pinPoint) m_pinPoint->setCanvas(0);
	delete m_pinPoint;

	for(unsigned i = 0; i < m_pins.size(); i++)
		delete m_pins[i];

	m_pins.resize(0);
}


void ECNode::setNumPins( unsigned num)
{
	unsigned oldNum = m_pins.size();

	if( num == oldNum) return;
	
	if( num > oldNum) {
		m_pins.resize(num);
		for( unsigned i = oldNum; i < num; i++)
			m_pins[i] = new Pin(this);
	} else {
		for( unsigned i = num; i < oldNum; i++)
			delete m_pins[i];
		m_pins.resize(num);
	}

	emit numPinsChanged(num);
}

void ECNode::setNodeChanged()
{
	if( !canvas() || numPins() != 1) return;

	Pin * pin = m_pins[0];

	double v = pin->voltage();
	double i = pin->current();

	if( v != m_prevV || i != m_prevI) {
		QRect r = boundingRect();
		r.setCoords( r.left()+(r.width()/2)-1, r.top()+(r.height()/2)-1, r.right()-(r.width()/2)+1, r.bottom()-(r.height()/2)+1);
		canvas()->setChanged(r);
		m_prevV = v;
		m_prevI = i;
	}
}

void ECNode::setParentItem( CNItem * parentItem)
{
	Node::setParentItem(parentItem);
	
	if( Component * component = dynamic_cast<Component*>(parentItem))
	{
		connect( component, SIGNAL(elementDestroyed(Element*)), this, SLOT(removeElement(Element*)));
		connect( component, SIGNAL(switchDestroyed( Switch*)), this, SLOT(removeSwitch( Switch*)));
	}
}

void ECNode::removeElement( Element * e)
{
	for( unsigned i = 0; i < m_pins.size(); i++)
		m_pins[i]->removeElement(e);
}

void ECNode::removeSwitch( Switch * sw)
{
	for( unsigned i = 0; i < m_pins.size(); i++)
		m_pins[i]->removeSwitch( sw);
}

void ECNode::drawShape( QPainter &p)
{
	const int _x = int(x());
	const int _y = int(y());

	if( type() == ec_junction) {
// 		p.drawRect( _x-2, _y-1, 5, 3);
// 		p.drawRect( _x-1, _y-2, 3, 5);
		p.drawRect( _x-1, _y-1, 3, 3);
		return;
	}
	
	if(m_pinPoint) {
		bool drawDivPoint;
		QPoint divPoint = findConnectorDivergePoint(&drawDivPoint);
		m_pinPoint->setVisible(drawDivPoint);
		m_pinPoint->move( divPoint.x()-1, divPoint.y()-1);
	}
	
	// Now to draw on our current/voltage bar indicators
	
	if( numPins() == 1)
	{
		double v = pin()->voltage();
		double vProp = calcVProp(v);
		int length = calcLength( vProp, v);
	
		if( m_bShowVoltageBars && length != 0)
		{
		// we can assume that v != 0 as length != 0

			QPen oldPen = p.pen();

			double i = pin()->current();
			double iProp = calcIProp(i);
			int thickness = calcThickness(iProp);
	
			if( v > 0)
	 			p.setPen( QPen( QColor( 255, 166, 0), thickness));
			else p.setPen( QPen( QColor( 0, 136, 255), thickness));
		
			// The node line (drawn at the end of this function) will overdraw
			// some of the voltage bar, so we need to adapt the length
			if( v > 0 && (m_dir == Node::dir_up || m_dir == Node::dir_down))
				length--;
			else if( v < 0 && (m_dir == Node::dir_left || m_dir == Node::dir_right))
				length++;

			if( m_dir == Node::dir_right)
				p.drawLine( _x+3, _y, _x+3, _y+length);

			else if( m_dir == Node::dir_down)
				p.drawLine( _x, _y+3, _x-length, _y+3);

			else if( m_dir == Node::dir_left)
				p.drawLine( _x-3, _y, _x-3, _y+length);

			else if( m_dir == Node::dir_up)
				p.drawLine( _x, _y-3, _x-length, _y-3);

			p.setPen(oldPen);
		}
	}

	QPen pen( p.pen());
	pen.setWidth( (numPins() > 1) ? 2 : 1);
	p.setPen(pen);

	if( m_dir == Node::dir_right)	p.drawLine( _x, _y, _x+8, _y);
	else if( m_dir == Node::dir_down) p.drawLine( _x, _y, _x, _y+8);
	else if( m_dir == Node::dir_left) p.drawLine( _x, _y, _x-8, _y);
	else if( m_dir == Node::dir_up)   p.drawLine( _x, _y, _x, _y-8);
}

#include "ecnode.moc"

