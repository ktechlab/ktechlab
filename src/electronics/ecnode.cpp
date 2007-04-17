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

// The maximum length of the voltage indiactor
const int vLength = 8;

// The current at the middle of the current indicator
const double iMidPoint = 0.03;

// The maximum thicnkess of the current indicator
const int iLength = 6;

inline double calcIProp( const double i )
{
	return 1 - iMidPoint/(iMidPoint+std::abs(i));
}

inline int calcThickness( const double prop )
{
	return (int)((iLength-2)*prop+2);
}

inline int calcLength( double v )
{
	double prop = Component::voltageLength( v );
	if ( v > 0 )
		prop *= -1.0;
	
	return int(vLength * prop);
}

ECNode::ECNode( ICNDocument *icnDocument, Node::node_type _type, int dir, const QPoint &pos, QString *_id )
	: Node( icnDocument, _type, dir, pos, _id )
{
	m_prevV = 0;
	m_prevI = 0;
	m_pinPoint = 0l;
	m_bShowVoltageBars = KTLConfig::showVoltageBars();
	m_bShowVoltageColor = KTLConfig::showVoltageColor();
	
	if ( icnDocument )
		icnDocument->registerItem(this);
	
	if ( type() == ec_pin )
	{
		m_pinPoint = new QCanvasRectangle( 0, 0, 3, 3, canvas() );
		m_pinPoint->setBrush(Qt::black);
		m_pinPoint->setPen(Qt::black);
	}
	
	m_pins.resize(1);
	m_pins[0] = new Pin(this);
}


ECNode::~ECNode()
{
	if (m_pinPoint)
		m_pinPoint->setCanvas(0l);
	delete m_pinPoint;
	m_pinPoint = 0l;
	
	for ( unsigned i = 0; i < m_pins.size(); i++ )
		delete m_pins[i];
	m_pins.resize(0);
}


void ECNode::setNumPins( unsigned num )
{
	unsigned oldNum = m_pins.size();
	
	if ( num == oldNum )
		return;
	
	if ( num > oldNum )
	{
		m_pins.resize(num);
		for ( unsigned i = oldNum; i < num; i++ )
			m_pins[i] = new Pin(this);
	}
	else
	{
		for ( unsigned i = num; i < oldNum; i++ )
			delete m_pins[i];
		m_pins.resize(num);
	}
	
	emit numPinsChanged(num);
}


void ECNode::setNodeChanged()
{
	if ( !canvas() || numPins() != 1 )
		return;
	
	Pin * pin = m_pins[0];
	
	double v = pin->voltage();
	double i = pin->current();
	
	if ( v != m_prevV || i != m_prevI )
	{
		QRect r = boundingRect();
// 		r.setCoords( r.left()+(r.width()/2)-1, r.top()+(r.height()/2)-1, r.right()-(r.width()/2)+1, r.bottom()-(r.height()/2)+1 );
		canvas()->setChanged(r);
		m_prevV = v;
		m_prevI = i;
	}
}


void ECNode::setParentItem( CNItem * parentItem )
{
	Node::setParentItem(parentItem);
	
	if ( Component * component = dynamic_cast<Component*>(parentItem) )
	{
		connect( component, SIGNAL(elementDestroyed(Element* )), this, SLOT(removeElement(Element* )) );
		connect( component, SIGNAL(switchDestroyed( Switch* )), this, SLOT(removeSwitch( Switch* )) );
	}
}


void ECNode::removeElement( Element * e )
{
	for ( unsigned i = 0; i < m_pins.size(); i++ )
		m_pins[i]->removeElement(e);
}


void ECNode::removeSwitch( Switch * sw )
{
	for ( unsigned i = 0; i < m_pins.size(); i++ )
		m_pins[i]->removeSwitch( sw );
}


void ECNode::drawShape( QPainter & p )
{
	initPainter( p );
	
	double v = pin() ? pin()->voltage() : 0.0;
	QColor voltageColor = Component::voltageColor( v );
	
	QPen pen = p.pen();
	if ( isSelected() )
		pen = m_selectedColor;
	else if ( m_bShowVoltageColor )
		pen = voltageColor;
	
	if ( type() == ec_junction )
	{
		p.setPen( pen );
		p.setBrush( pen.color() );
		p.drawRect( -1, -1, 3, 3 );
		deinitPainter( p );
		return;
	}
	
	if (m_pinPoint)
	{
		bool drawDivPoint;
		QPoint divPoint = findConnectorDivergePoint(&drawDivPoint);
		m_pinPoint->setVisible(drawDivPoint);
		m_pinPoint->move( divPoint.x()-1, divPoint.y()-1 );
		m_pinPoint->setBrush( pen.color() );
		m_pinPoint->setPen( pen.color() );
	}
	
	// Now to draw on our current/voltage bar indicators
	int length = calcLength( v );
	
	if ( (numPins() == 1) && m_bShowVoltageBars && length != 0 )
	{
		// we can assume that v != 0 as length != 0
		double i = pin()->current();
		double iProp = calcIProp(i);
		int thickness = calcThickness(iProp);
			
		p.setPen( QPen( voltageColor, thickness ) );
		
		// The node line (drawn at the end of this function) will overdraw
		// some of the voltage bar, so we need to adapt the length
		if ( (v > 0) && (((225 < m_dir) && (m_dir < 315)) || ((45 < m_dir) && (m_dir < 135))) )
			length--;
			
		else if ( (v < 0) && (((135 < m_dir) && (m_dir < 225)) || ((315 < m_dir) || (m_dir < 45))) )
			length++;
		
		if ( (m_dir > 270) || (m_dir <= 90) )
			p.drawLine( 3, 0, 3, length );
		else
			p.drawLine( 3, 0, 3, -length );
	}
	
	pen.setWidth( (numPins() > 1) ? 2 : 1 );
	p.setPen( pen );
	
	p.drawLine( 0, 0, m_length, 0 );
	
	deinitPainter( p );
}

#include "ecnode.moc"
