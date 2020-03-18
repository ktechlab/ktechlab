/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecbcdto7segment.h"

#include "logic.h"
#include "libraryitem.h"

#include <klocalizedstring.h>

// Values for a,b,c,d,e,f,g of common-anode 7 segment display
static bool numbers[16][7] =
	{ { 1, 1, 1, 1, 1, 1, 0 }, // 0
	  { 0, 1, 1, 0, 0, 0, 0 }, // 1
	  { 1, 1, 0, 1, 1, 0, 1 }, // 2
	  { 1, 1, 1, 1, 0, 0, 1 }, // 3
	  { 0, 1, 1, 0 ,0, 1, 1 }, // 4
	  { 1, 0, 1, 1, 0, 1, 1 }, // 5
	  { 1, 0, 1, 1, 1, 1, 1 }, // 6
	  { 1, 1, 1, 0, 0, 0, 0 }, // 7
	  { 1, 1, 1, 1, 1, 1, 1 }, // 8
	  { 1, 1, 1, 0, 0, 1, 1 }, // 9
	  { 1, 1, 1, 0, 1, 1, 1 }, // A
	  { 0, 0, 1, 1, 1, 1, 1 }, // b
	  { 1, 0, 0, 1, 1, 1, 0 }, // C
	  { 0, 1, 1, 1, 1, 0, 1 }, // d
	  { 1, 0, 0, 1, 1, 1, 1 }, // E
	  { 1, 0, 0, 0, 1, 1, 1 } }; // F

Item* ECBCDTo7Segment::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECBCDTo7Segment( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECBCDTo7Segment::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/bcd_to_seven_segment")),
		i18n("BCD to 7 Segment"),
		i18n("Integrated Circuits"),
		"ic2.png",
		LibraryItem::lit_component,
		ECBCDTo7Segment::construct
			);
}

ECBCDTo7Segment::ECBCDTo7Segment( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "bcd_to_seven_segment" )
{
	m_name = i18n("BCD to Seven Segment");
				
	ALogic = BLogic = CLogic = DLogic = nullptr;
	ltLogic = rbLogic = enLogic = nullptr;
	
	for ( int i=0; i<7; i++ )
	{
		outLogic[i] = nullptr;
		oldOut[i] = false;
	}

	//QStringList pins = QStringList::split( ',', "A,B,C,D,,lt,rb,en,d,e,f,g,,a,b,c", true ); // 2018.12.01
    QStringList pins = QString("A,B,C,D,,lt,rb,en,d,e,f,g,,a,b,c").split(',', QString::KeepEmptyParts);
	initDIPSymbol( pins, 48 );
	initDIP(pins);
	
	ALogic = createLogicIn( ecNodeWithID("A") );
	BLogic = createLogicIn( ecNodeWithID("B") );
	CLogic = createLogicIn( ecNodeWithID("C") );
	DLogic = createLogicIn( ecNodeWithID("D") );
	ltLogic = createLogicIn( ecNodeWithID("lt") );
	rbLogic = createLogicIn( ecNodeWithID("rb") );
	enLogic = createLogicIn( ecNodeWithID("en") );
	
	ALogic->setCallback( this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged) );
	BLogic->setCallback( this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged) );
	CLogic->setCallback( this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged) );
	DLogic->setCallback( this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged) );
	ltLogic->setCallback( this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged) );
	rbLogic->setCallback( this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged) );
	enLogic->setCallback( this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged) );
	
	for ( uint i=0; i<7; ++i )
	{
		outLogic[i] = createLogicOut( ecNodeWithID( QChar('a'+i) ), false );
		outLogic[i]->setCallback( this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged) );
	}
	inStateChanged(false);
}

ECBCDTo7Segment::~ECBCDTo7Segment()
{}

void ECBCDTo7Segment::inStateChanged(bool)
{
	bool A = ALogic->isHigh();
	bool B = BLogic->isHigh();
	bool C = CLogic->isHigh();
	bool D = DLogic->isHigh();
	bool lt = ltLogic->isHigh(); // Lamp test
	bool rb = rbLogic->isHigh(); // Ripple Blank
	bool en = enLogic->isHigh(); // Enable (store)
	
	int n = A + 2*B + 4*C + 8*D;
// 	if ( n > 9 ) n = 0;
	
	bool out[7];
	
	if (lt) // Lamp test
	{
		if (rb) // Ripple blanking
		{
			if (en) // Enable (store)
			{
				for ( int i=0; i<7; i++ ) {
					out[i] = oldOut[i];
				}
			} else {
				for ( int i=0; i<7; i++ ) {
					out[i] = numbers[n][i];
					oldOut[i] = out[i];
				}
			}
		} else {
			for ( int i=0; i<7; i++ ) {
				out[i] = false;
			}
		}
	} else {
		for ( int i=0; i<7; i++ ) {
			out[i] = true;
		}
	}
	
	for ( int i=0; i<7; i++ ) {
		outLogic[i]->setHigh( out[i] );
	}
}
