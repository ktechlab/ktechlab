/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecnode.h"
#include "flipflop.h"
#include "icndocument.h"
#include "logic.h"
#include "libraryitem.h"
#include "node.h"
#include "simulator.h"

#include <kiconloader.h>
#include <klocale.h>
#include <Qt/qpainter.h>


//BEGIN class ClockedFlipFlop
ClockedFlipFlop::ClockedFlipFlop( ICNDocument *icnDocument, bool newItem, const char * id )
	: Component( icnDocument, newItem, id )
{
	createProperty( "trig", Variant::Type::Select );
	property("trig")->setCaption( i18n("Trigger Edge") );
	QStringMap allowed;
	allowed["Rising"] = i18n("Rising");
	allowed["Falling"] = i18n("Falling");
	property("trig")->setAllowed( allowed );
	property("trig")->setValue("Rising");
	m_edgeTrigger = Rising;
}


void ClockedFlipFlop::dataChanged()
{
	EdgeTrigger t = (dataString("trig") == "Rising") ? Rising : Falling;
	if ( t == m_edgeTrigger )
		return;
	
	m_edgeTrigger = t;
	initSymbolFromTrigger();
}
//END class ClockedFlipFlop



//BEGIN class ECDFlipFlop
Item* ECDFlipFlop::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECDFlipFlop( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECDFlipFlop::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/d_flipflop")),
		i18n("D Flip-Flop"),
		i18n("Integrated Circuits"),
		"ic3.png",
		LibraryItem::lit_component,
		ECDFlipFlop::construct );
}

ECDFlipFlop::ECDFlipFlop( ICNDocument *icnDocument, bool newItem, const char *id )
	: ClockedFlipFlop( icnDocument, newItem, id ? id : "d_flipflop" )
{
	m_name = i18n("D-Type Flip-Flop");
	
	setSize( -32, -24, 64, 48 );
	init2PinLeft( -8, 8 );
	init2PinRight( -8, 8 );
	initSymbolFromTrigger();
	
	m_prevD = false;
	m_pSimulator = Simulator::self();
	
	m_bPrevClock = false;
	m_pD = createLogicIn( m_pNNode[0] );
	m_pClock = createLogicIn( m_pNNode[1] );
	m_pQ = createLogicOut( m_pPNode[0], false );
	m_pQBar = createLogicOut( m_pPNode[1], false );
	
	setp = createLogicIn( createPin( 0, -32, 90, "set" ) );
	rstp = createLogicIn( createPin( 0, 32, 270, "rst" ) );
	
	// (The display text for D, >, Set, Rst is set in initSymbolFromTrigger
	addDisplayText( "Q",	QRect( 12,	-16,	20, 16 ), "Q" );
	addDisplayText( "Q'",	QRect( 12,	0,		20, 16 ), "Q'" );
	
	m_pD->setCallback( this, (CallbackPtr)(&ECDFlipFlop::inputChanged) );
	m_pClock->setCallback( this, (CallbackPtr)(&ECDFlipFlop::clockChanged) );
	setp->setCallback( this, (CallbackPtr)(&ECDFlipFlop::asyncChanged) );
	rstp->setCallback( this, (CallbackPtr)(&ECDFlipFlop::asyncChanged) );
	
	inStateChanged(false);
}

ECDFlipFlop::~ECDFlipFlop()
{
    m_pD->setCallback( NULL, (CallbackPtr)(NULL) );
    m_pClock->setCallback( NULL, (CallbackPtr)(NULL) );
    setp->setCallback( NULL, (CallbackPtr)(NULL) );
    rstp->setCallback( NULL, (CallbackPtr)(NULL) );
}

void ECDFlipFlop::initSymbolFromTrigger()
{
	int offset = (m_edgeTrigger == Rising) ? 0 : 6;
	
	int w = 64-offset;
	setSize( offset-32, -24, w, 48, true );
	m_pNNode[0]->setLength( 8+offset );
	addDisplayText( "D",	QRect( offset-28,	-16,	20, 16 ), "D", true, Qt::AlignLeft );
	addDisplayText( ">",	QRect( offset-28,	0,		20, 16 ), ">", true, Qt::AlignLeft ); 
	addDisplayText( "Set",	QRect( offset-28,	-20,	w-8, 16 ), "Set", true, Qt::AlignHCenter );
	addDisplayText( "Rst",	QRect( offset-28,	4,		w-8, 16 ), "Rst", true, Qt::AlignHCenter );
	
	updateAttachedPositioning();
}

void ECDFlipFlop::drawShape( QPainter & p )
{
	Component::drawShape( p );
	
	if ( m_edgeTrigger == Falling )
	{
		initPainter( p );
		p.drawEllipse( int(x()-32), int(y()+5), 6, 6 );
		deinitPainter( p );
	}
}

void ECDFlipFlop::asyncChanged(bool)
{
	bool set = setp->isHigh();
	bool rst = rstp->isHigh();
	if(set || rst)
	{
		m_pQ->setHigh(set);
		m_pQBar->setHigh(rst);
	}
}

void ECDFlipFlop::inputChanged( bool newState )
{
	if ( newState == m_prevD )
	{
		// Only record the time that the input state changes
		return;
	}
	
	m_prevD = newState;
	m_prevDChangeSimTime = m_pSimulator->time();
}

void ECDFlipFlop::clockChanged( bool newState )
{
	bool set = setp->isHigh();
	bool rst = rstp->isHigh();
	
	bool fallingEdge = m_bPrevClock && !newState;
	bool edge = (m_edgeTrigger == Falling) ? fallingEdge : !fallingEdge;
	
	m_bPrevClock = newState;
	
	if( set || rst ) return;
	
	if ( edge )
	{
		// The D Flip-Flop takes the input before the edge fall/rise - not after
		// the edge. So see if the input state last changed before now or at
		// now to work out if we should take the current value of m_prevD, or
		// its inverse.
		
		unsigned long long simTime = m_pSimulator->time();
		bool d = (simTime == m_prevDChangeSimTime) ? !m_prevD : m_prevD;
		
		m_pQ->setHigh( d );
		m_pQBar->setHigh( !d );
	}
}
	
void ECDFlipFlop::inStateChanged(bool)
{
	// Only called when the flipflop is created.
	m_pQ->setHigh(false);
	m_pQBar->setHigh(true);
}
//END class ECDFlipFlop


//BEGIN class ECJKFlipFlop
Item* ECJKFlipFlop::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECJKFlipFlop( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECJKFlipFlop::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/jk_flipflop")),
		i18n("JK Flip-Flop"),
		i18n("Integrated Circuits"),
		"ic3.png",
		LibraryItem::lit_component,
		ECJKFlipFlop::construct );
}

ECJKFlipFlop::ECJKFlipFlop( ICNDocument *icnDocument, bool newItem, const char *id )
	: ClockedFlipFlop( icnDocument, newItem, id ? id : "jk_flipflop" )
{
	m_name = i18n("JK-Type Flip-Flop");
	
	setSize( -32, -32, 64, 64 );
	init3PinLeft( -16, 0, 16 );
	init2PinRight( -16, 16 );
	initSymbolFromTrigger();
	
	m_bPrevClock = false;
	
	createProperty( "trig", Variant::Type::Select );
	property("trig")->setCaption( i18n("Trigger Edge") );
	QStringMap allowed;
	allowed["Rising"] = i18n("Rising");
	allowed["Falling"] = i18n("Falling");
	property("trig")->setAllowed( allowed );
	property("trig")->setValue("Rising");
	m_edgeTrigger = Rising;
	initSymbolFromTrigger();
	
	m_pJ = createLogicIn( m_pNNode[0] );
	m_pClock = createLogicIn( m_pNNode[1] );
	m_pK = createLogicIn( m_pNNode[2] );
	
	m_pQ = createLogicOut( m_pPNode[0], false );
	m_pQBar = createLogicOut( m_pPNode[1], false );

	setp = createLogicIn( createPin( 0, -40, 90, "set" ) );
	rstp = createLogicIn( createPin( 0, 40, 270, "rst" ) );
	
	addDisplayText( "Q",	QRect( 12,	-24,	20, 16 ), "Q" );
	addDisplayText( "Q'",	QRect( 12,	8,		20, 16 ), "Q'" );
		
	m_pClock->setCallback( this, (CallbackPtr)(&ECJKFlipFlop::clockChanged) );
	setp->setCallback( this, (CallbackPtr)(&ECJKFlipFlop::asyncChanged) );
	rstp->setCallback( this, (CallbackPtr)(&ECJKFlipFlop::asyncChanged) );
	
	inStateChanged(false);
}

ECJKFlipFlop::~ECJKFlipFlop()
{
    m_pClock->setCallback( NULL, (CallbackPtr)(NULL) );
    setp->setCallback( NULL, (CallbackPtr)(NULL) );
    rstp->setCallback( NULL, (CallbackPtr)(NULL) );
}

void ECJKFlipFlop::initSymbolFromTrigger()
{
	int offset = (m_edgeTrigger == Rising) ? 0 : 6;
	
	int w = 64-offset;
	setSize( offset-32, -32, w, 64, true );
	m_pNNode[0]->setLength( 8+offset );
	m_pNNode[2]->setLength( 8+offset );
	addDisplayText( "J",	QRect( offset-28,	-24,	20, 16 ), "J", true, Qt::AlignLeft );
	addDisplayText( ">",	QRect( offset-28,	-8,		20, 16 ), ">", true, Qt::AlignLeft );
	addDisplayText( "K",	QRect( offset-28,	8,		20, 16 ), "K", true, Qt::AlignLeft ); 
	addDisplayText( "Set",	QRect( offset-28,	-28,	w-8, 16 ), "Set", true, Qt::AlignHCenter );
	addDisplayText( "Rst",	QRect( offset-28,	12,		w-8, 16 ), "Rst", true, Qt::AlignHCenter );
	
	updateAttachedPositioning();
}

void ECJKFlipFlop::drawShape( QPainter & p )
{
	Component::drawShape( p );
	
	if ( m_edgeTrigger == Falling )
	{
		initPainter( p );
		p.drawEllipse( int(x()-32), int(y()-3), 6, 6 );
		deinitPainter( p );
	}
}

void ECJKFlipFlop::clockChanged(bool newvalue) 
{
	bool fallingEdge = (m_bPrevClock && !newvalue);
	bool edge = (m_edgeTrigger == Falling) ? fallingEdge : !fallingEdge;
	m_bPrevClock = newvalue;
	
	bool j = m_pJ->isHigh();
	bool k = m_pK->isHigh();
	bool set = setp->isHigh();
	bool rst = rstp->isHigh();
	
	if( set || rst ) return;
	
// a JK flip-flop change state when clock do 1->0
	if ( edge && (j || k)) {
		if ( j && k ) {
			m_pQ->setHigh(!prev_state);
			m_pQBar->setHigh(prev_state);
			prev_state = !prev_state;
		} else {
			// (J=1 && K=0) || (J=0 && K=1)
			m_pQ->setHigh(j);
			m_pQBar->setHigh(k);
			prev_state = j;
		}	
	}
}

void ECJKFlipFlop::asyncChanged(bool)
{
	bool set = setp->isHigh();
	bool rst = rstp->isHigh();
	
	if (set || rst) {
		m_pQ->setHigh(set);
		m_pQBar->setHigh(rst);
		prev_state = set;
	}
}
	
void ECJKFlipFlop::inStateChanged(bool)
{
	m_pQBar->setHigh(true);
	m_pQ->setHigh(false);
	prev_state = false;
}
//END class ECJKFlipFlop


//BEGIN class ECSRFlipFlop
Item* ECSRFlipFlop::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECSRFlipFlop( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECSRFlipFlop::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/sr_flipflop")),
		i18n("SR Flip-Flop"),
		i18n("Integrated Circuits"),
		"ic3.png",
		LibraryItem::lit_component,
		ECSRFlipFlop::construct );
}

ECSRFlipFlop::ECSRFlipFlop( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "sr_flipflop" )
{
	m_name = i18n("SR Flip-Flop");
	
	setSize( -24, -24, 48, 48 );

	init2PinLeft( -8, 8 );
	init2PinRight( -8, 8 );
	
	m_pS = createLogicIn( m_pNNode[0] );
	m_pR = createLogicIn( m_pNNode[1] );
	m_pQ = createLogicOut( m_pPNode[0], true );
	m_pQBar = createLogicOut( m_pPNode[1], false );
	
	old_q1 = true;
	old_q2 = false;
	m_pQ->setHigh(old_q1);
	m_pQBar->setHigh(old_q2);
	
	addDisplayText( "S", QRect( -24, -16, 20, 16 ), "S" );
	addDisplayText( "R", QRect( -24, 0, 20, 16 ), "R" );
	addDisplayText( "Q", QRect( 4, -16, 20, 16 ), "Q" );
	addDisplayText( "Q'", QRect( 4, 0, 20, 16 ), "Q'" );
	
	m_pS->setCallback( this, (CallbackPtr)(&ECSRFlipFlop::inStateChanged) );
	m_pR->setCallback( this, (CallbackPtr)(&ECSRFlipFlop::inStateChanged) );
	m_pQ->setCallback( this, (CallbackPtr)(&ECSRFlipFlop::inStateChanged) );
	m_pQBar->setCallback( this, (CallbackPtr)(&ECSRFlipFlop::inStateChanged) );
}

ECSRFlipFlop::~ECSRFlipFlop()
{
    m_pS->setCallback( NULL, (CallbackPtr)(NULL) );
    m_pR->setCallback( NULL, (CallbackPtr)(NULL) );
    m_pQ->setCallback( NULL, (CallbackPtr)(NULL) );
    m_pQBar->setCallback( NULL, (CallbackPtr)(NULL) );
}

void ECSRFlipFlop::inStateChanged(bool)
{
	// Q = v_q1, Q-bar = v_q2
	bool new_q1 = false;
	bool new_q2 = false;
	
	bool s = m_pS->isHigh();
	bool r = m_pR->isHigh();
	bool q1 = m_pQ->isHigh();
	bool q2 = m_pQBar->isHigh();
	
	// Easy ones to do :-)
	if (!q1) new_q2 = true;
	if (!q2) new_q1 = true;
	
	if ( q1 && q2 )
	{
		if ( s && !r )
		{
			new_q1 = true;
			new_q2 = false;
		}
		else if ( !s && r )
		{
			new_q1 = false;
			new_q2 = true;
		}
		else if ( s && r )
		{
			new_q1 = old_q1;
			new_q2 = old_q2;
		}
		else if ( !s && !r )
		{
			new_q1 = false;
			new_q2 = false;
		}
	}
	else if ( q1 && !q2 )
	{
		// Note: We only need to set the value of v_q2
		if ( r && !s ) new_q2 = true;
		else new_q2 = false;
	}
	else if ( !q1 && q2 )
	{
		// Note: We only need to set the value of v_q1
		if ( s && !r ) new_q1 = true;
		else new_q1 = false;
	}
	
	old_q1 = new_q1;
	old_q2 = new_q2;
	
	m_pQ->setHigh(new_q1);
	m_pQBar->setHigh(new_q2);
}
//END class ECSRFlipFlop
