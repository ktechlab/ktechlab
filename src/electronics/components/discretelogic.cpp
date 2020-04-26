/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "canvasitemparts.h"
#include "discretelogic.h"
#include "ecnode.h"
#include "logic.h"
#include "libraryitem.h"
#include "simulator.h"

#include <KLocalizedString>
#include <QPainter>


//BEGIN class Inverter
Item* Inverter::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new Inverter( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* Inverter::libraryItem()
{
	QStringList ids;
	ids << "ec/inverter" << "ec/not";
	return new LibraryItem(
		ids,
		i18n("Inverter"),
		i18n("Logic"),
		"not.png",
		LibraryItem::lit_component,
		Inverter::construct );
}

Inverter::Inverter( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "not" )
{
	m_name = i18n("Inverter");
	setSize( -8, -8, 16, 16 );

	init1PinLeft();
	init1PinRight();
	
	m_pIn = createLogicIn(m_pNNode[0]);
	m_pOut = createLogicOut( m_pPNode[0], true );
	
	m_pIn->setCallback( this, (CallbackPtr)(&Inverter::inStateChanged) );
	inStateChanged(false);
}


Inverter::~Inverter()
{
}


void Inverter::inStateChanged( bool newState )
{
	(static_cast<LogicOut*>(m_pOut))->setHigh( !newState );
}


void Inverter::drawShape( QPainter &p )
{
	initPainter(p);
	int _x = (int)x()-8;
	int _y = (int)y()-8;
	QPolygon pa(3);
	pa[0] = QPoint( _x, _y );
	pa[1] = QPoint( _x+width()-6, _y+(height()/2) );
	pa[2] = QPoint( _x, _y+height() );
	p.drawPolygon(pa);
	p.drawPolyline(pa);
	p.drawEllipse( _x+width()-6, _y+(height()/2)-3, 7, 7 );
	deinitPainter(p);
}
//END class Inverter


//BEGIN class Buffer
Item* Buffer::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new Buffer( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* Buffer::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/buffer")),
		i18n("Buffer"),
		i18n("Logic"),
		"buffer.png",
		LibraryItem::lit_component,
		Buffer::construct );
}

Buffer::Buffer( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "buffer" )
{
	m_name = i18n("Buffer");
	setSize( -8, -8, 16, 16 );

	init1PinLeft();
	init1PinRight();
	
	m_pIn = createLogicIn(m_pNNode[0]);
	m_pOut = createLogicOut( m_pPNode[0], true );
	
	m_pIn->setCallback( this, (CallbackPtr)(&Buffer::inStateChanged) );
	inStateChanged(false);
}


Buffer::~Buffer()
{
}


void Buffer::inStateChanged( bool newState )
{
	m_pOut->setHigh(newState);
}


void Buffer::drawShape( QPainter &p )
{
	initPainter(p);
	int _x = (int)x()-8;
	int _y = (int)y()-8;
	QPolygon pa(3);
	pa[0] = QPoint( _x, _y );
	pa[1] = QPoint( _x+width(), _y+(height()/2) );
	pa[2] = QPoint( _x, _y+height() );
	p.drawPolygon(pa);
	p.drawPolyline(pa);
	deinitPainter(p);
}
//END class Buffer


//BEGIN class ECLogicInput
Item* ECLogicInput::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECLogicInput( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECLogicInput::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/logic_input")),
		i18n("Logic Input"),
		i18n("Logic"),
		"logic_input.png",
		LibraryItem::lit_component,
		ECLogicInput::construct );
}

ECLogicInput::ECLogicInput( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, (id) ? id : "logic_input" )
{
	m_name = i18n("Logic Input");
	setSize( -8, -8, 16, 16 );
	
	b_state = false;
	addButton( "button", QRect( -24, -8, 16, 16 ), "", true );
	
	createProperty( "useToggle", Variant::Type::Bool );
	property("useToggle")->setCaption( i18n("Use Toggle") );
	property("useToggle")->setValue(true);

	init1PinRight();
	
	m_pOut = createLogicOut( m_pPNode[0], false );
}


ECLogicInput::~ECLogicInput()
{
}


void ECLogicInput::dataChanged()
{
	button("button")->setToggle( dataBool("useToggle") );
}


void ECLogicInput::drawShape( QPainter &p )
{
	initPainter(p);
	if (b_state)
		p.setBrush( QColor( 255, 166, 0 ) );
	else
		p.setBrush( Qt::white );
	p.drawEllipse( (int)x()-4, (int)y()-6, 12, 12 );
	deinitPainter(p);
}


void ECLogicInput::buttonStateChanged( const QString &, bool state )
{
	b_state = state;
	m_pOut->setHigh(b_state);
}
//END class ECLogicInput


//BEGIN class ECLogicOutput
Item* ECLogicOutput::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECLogicOutput( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECLogicOutput::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/logic_output")),
		i18n("Logic Output"),
		i18n("Logic"),
		"logic_output.png",
		LibraryItem::lit_component,
		ECLogicOutput::construct );
}

ECLogicOutput::ECLogicOutput( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "logic_output" )
{
	m_name = i18n("Logic Output");
	setSize( -8, -8, 16, 16 );
	
	init1PinLeft();
	m_pIn = createLogicIn(m_pNNode[0]);
	
	m_pSimulator = Simulator::self();
	
	m_lastDrawState = 0.0;
	m_lastSwitchTime = m_lastDrawTime = m_pSimulator->time();
	m_highTime = 0;
	m_bLastState = false;
	m_bDynamicContent = true;
	
	m_pIn->setCallback( this, (CallbackPtr)(&ECLogicOutput::inStateChanged) );
}

ECLogicOutput::~ECLogicOutput()
{
}

void ECLogicOutput::inStateChanged( bool newState )
{
	if ( m_bLastState == newState )
		return;
	
	unsigned long long newTime = m_pSimulator->time();
	unsigned long long dt = newTime - m_lastSwitchTime;
	
	m_lastSwitchTime = newTime;
	
	m_bLastState = newState;
	if (!newState)
	{
		// Gone from high to low
		m_highTime += dt;
	}
}


void ECLogicOutput::drawShape( QPainter &p )
{
	unsigned long long newTime = m_pSimulator->time();
	unsigned long long runTime = newTime - m_lastDrawTime;
	m_lastDrawTime = newTime;
	
	if (m_bLastState)
	{
		// Logic in is currently high
		m_highTime += newTime - m_lastSwitchTime;
	}
	
	double state;
	
	if ( runTime == 0 )
		state = m_lastDrawState;
	
	else
		state = m_lastDrawState = double(m_highTime)/double(runTime);
	
	initPainter(p);
	p.setBrush( QColor( 255, uint(255-state*(255-166)), uint((1-state)*255) ) );
	p.drawEllipse( int(x()-8), int(y()-8), width(), height() );
	deinitPainter(p);
	
	m_lastSwitchTime = newTime;
	m_highTime = 0;
}
//END class ECLogicOutput

