/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "binarycounter.h"

#include <cstdlib>
#include <kiconloader.h>
#include <klocale.h>

#include "logic.h"
#include "libraryitem.h"

Item* BinaryCounter::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new BinaryCounter( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* BinaryCounter::libraryItem()
{
	QStringList ids;
	ids << "ec/binary_counter" << "ec/4_bit_binary_counter";
	return new LibraryItem(
		ids,
		i18n("Binary Counter"),
		i18n("Integrated Circuits"),
		"ic1.png",
		LibraryItem::lit_component,
		BinaryCounter::construct
			);
}

BinaryCounter::BinaryCounter( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "binary_counter" )
{
	m_name = i18n("Binary Counter");

	enLogic = inLogic = rLogic = udLogic = 0L;
	
	b_reset = false;
	b_triggerHigh = true;
	b_oldIn = false;
	m_value = 0;
	b_en = false;
	b_ud = false;
	m_numBits = 0;
	m_maxValue = false;
	m_bDoneLogicIn = false;
	
	createProperty( "trig", Variant::Type::Select );
	property("trig")->setCaption( i18n("Trigger Edge") );
	QStringMap allowed;
	allowed["Rising"] = i18n("Rising");
	allowed["Falling"] = i18n("Falling");
	property("trig")->setAllowed( allowed );
	property("trig")->setValue("Falling");
	
	createProperty( "bitcount", Variant::Type::Int );
	property("bitcount")->setCaption( i18n("Bit Count") );
	property("bitcount")->setMinValue(1);
	property("bitcount")->setMaxValue(26);
	property("bitcount")->setValue(4);
}


BinaryCounter::~BinaryCounter()
{
}


void BinaryCounter::dataChanged()
{
	initPins( dataInt("bitcount") );
	
	b_triggerHigh = dataString("trig") == "Rising";
	setDisplayText( ">", b_triggerHigh ? "^>" : "_>" );
}


void BinaryCounter::initPins( unsigned numBits )
{
	if ( m_numBits == numBits )
		return;
	
	QStringList pins;
	pins << "en" << ">" << "u/d" << "r";

	{
		int np = abs(4-int(numBits));
		for(int i = 0; i < np; i++)
			pins << " ";
	}
	
	for(int i = numBits-1; i >= 0; i-- )
		pins << QChar('A' + i);
	
	initDIPSymbol(pins, 64);
	initDIP(pins);
	
	if(m_numBits < numBits)
	{
		for(unsigned i = m_numBits; i < numBits; i++) {
//			m_pLogicOut[i] = createLogicOut(ecNodeWithID(QChar('A' + i))->pin(), false);

			m_pLogicOut[i] = new LogicOut(LogicIn::getConfig(), false);
			setup1pinElement(m_pLogicOut[i], ecNodeWithID(QChar('A' + i))->pin());
		}
	} else {
		for(unsigned i = numBits; i < m_numBits; i++)
		{
			QString id = QChar('A' + i);
			removeElement(m_pLogicOut[i], false);
			removeDisplayText(id);
			removeNode(id);
		}
	}

	m_numBits = numBits;
	m_maxValue = (1 << m_numBits) - 1;

	if(!m_bDoneLogicIn)
	{
//		enLogic = createLogicIn(ecNodeWithID("en")->pin() );

		enLogic = new LogicIn(LogicIn::getConfig());
		setup1pinElement(enLogic, ecNodeWithID("en")->pin());
		enLogic->setCallback(this, (CallbackPtr)(&BinaryCounter::enStateChanged));

//		inLogic = createLogicIn(ecNodeWithID(">")->pin()  );

		inLogic = new LogicIn(LogicIn::getConfig());
		setup1pinElement(inLogic, ecNodeWithID(">")->pin());
		inLogic->setCallback(this, (CallbackPtr)(&BinaryCounter::inStateChanged));

//		rLogic  = createLogicIn( ecNodeWithID("r")->pin() );

		rLogic = new LogicIn(LogicIn::getConfig());
		setup1pinElement(rLogic, ecNodeWithID("r")->pin());
		rLogic->setCallback(this, (CallbackPtr)(&BinaryCounter::rStateChanged));

//		udLogic = createLogicIn(ecNodeWithID("u/d")->pin());

		udLogic = new LogicIn(LogicIn::getConfig());
		setup1pinElement(udLogic, ecNodeWithID("u/d")->pin());
		udLogic->setCallback(this, (CallbackPtr)(&BinaryCounter::udStateChanged));

		m_bDoneLogicIn = true;
	}
	
	outputValue();
}

void BinaryCounter::inStateChanged(bool state)
{
	if((state != b_oldIn) && b_en && !b_reset && state == b_triggerHigh)
	{
		m_value += (b_ud) ? 1 : -1;
		
		if(m_value < 0)
			m_value = m_maxValue;
		
		else if(m_value > m_maxValue)
			m_value = 0;
		
		outputValue();
	}
	
	b_oldIn = state;
}

void BinaryCounter::rStateChanged(bool state)
{
	b_reset = state;
	if(b_reset) {
		m_value = 0;
		outputValue();
	}
}

void BinaryCounter::enStateChanged(bool state)
{
	b_en = state;
}

void BinaryCounter::udStateChanged(bool state)
{
	b_ud = state;
}


void BinaryCounter::outputValue()
{
	for ( unsigned i = 0; i < m_numBits; i++ )
		m_pLogicOut[i]->setHigh( m_value & (1 << i) );
}

