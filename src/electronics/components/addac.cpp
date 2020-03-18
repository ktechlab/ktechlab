/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "addac.h"
#include "ecnode.h"
#include "logic.h"
#include "libraryitem.h"
#include "pin.h"
#include "voltagepoint.h"

#include <cmath>
#include <klocalizedstring.h>


Item* ADC::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ADC( (ICNDocument*)itemDocument, newItem, id );
}


Item* DAC::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new DAC( (ICNDocument*)itemDocument, newItem, id );
}


LibraryItem* ADC::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/adc")),
		i18n("Analog-Digital"),
		i18n("Integrated Circuits"),
		"ic1.png",
		LibraryItem::lit_component,
		ADC::construct
			);
}


LibraryItem* DAC::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/dac")),
		i18n("Digital-Analog"),
		i18n("Integrated Circuits"),
		"ic1.png",
		LibraryItem::lit_component,
		DAC::construct
			);
}


//BEGIN class ADDAC
ADDAC::ADDAC( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id )
{
	m_numBits = 0;
	m_range = 0;
	
	createProperty( "numBits",  Variant::Type::Int );
	property("numBits")->setCaption( i18n("Number Bits") );
	property("numBits")->setMinValue(2);
	property("numBits")->setMaxValue(max_ADDAC_bits);
	property("numBits")->setValue(2);
	
	createProperty( "range", Variant::Type::Double );
	property("range")->setCaption( i18n("Input Range") );
	property("range")->setUnit("V");
	property("range")->setMinValue(-1e12);
	property("range")->setMaxValue(1e12);
	property("range")->setValue(5);
}

ADDAC::~ADDAC()
{
}


void ADDAC::dataChanged()
{
	m_range = dataDouble("range");
	initPins();
}

//END class ADDAC




//BEGIN class ADC
ADC::ADC( ICNDocument *icnDocument, bool newItem, const char *id )
	: ADDAC( icnDocument, newItem, id ? id : "adc" )
{
	m_name = i18n("ADC");
	
	for ( int i=0; i<max_ADDAC_bits; ++i )
		m_logic[i] = nullptr;
	
	m_realNode = nullptr;
}

ADC::~ADC()
{
}


void ADC::stepNonLogic()
{
	double floatBitValue = m_realNode->pin()->voltage() * (std::pow( 2, double(m_numBits) )-1.) / m_range;
	double roundBitValue = std::floor( floatBitValue+0.5 );
	
	if ( roundBitValue < 0 )
	{
		for ( int i = 0; i<m_numBits; ++i )
			m_logic[i]->setHigh(false);
		return;
	}
	
	uint roundedBitValue = uint(roundBitValue);
	for ( int i = 0; i<m_numBits; ++i )
		m_logic[i]->setHigh( roundedBitValue & ( 1 << i ) );
}


void ADC::initPins()
{
	int numBits = dataInt("numBits");
	
	if ( numBits < 2 )
		numBits = 2;
	else if ( numBits > max_ADDAC_bits )
		numBits = max_ADDAC_bits;
	
	if ( numBits == m_numBits )
		return;
	
	QStringList pins;
	
	int inPos = (numBits-1+(numBits%2))/2;
	for ( int i=0; i<inPos; ++i )
		pins += "";
	
	pins += "In";
	
	for ( int i=inPos+1; i<numBits; ++i )
		pins += "";
	
	for ( int i=numBits-1; i>=0; --i )
		pins += QString::number(i);
	
	initDIPSymbol( pins, 64 );
	initDIP(pins);
	
	if (!m_realNode)
		m_realNode =  ecNodeWithID("In");
	
	
	if ( numBits > m_numBits )
	{
		for ( int i=m_numBits; i<numBits; ++i )
		{
			ECNode *node = ecNodeWithID( QString::number(i) );
			m_logic[i] = createLogicOut( node, false );
		}
	}
	else
	{
		for ( int i=numBits; i<m_numBits; ++i )
		{
			QString id = QString::number(i);
			removeDisplayText(id);
			removeElement( m_logic[i], false );
			removeNode(id);
			m_logic[i] = nullptr;
		}
	}
	
	m_numBits = numBits;
}
//END class ADC




//BEGIN class DAC
DAC::DAC( ICNDocument *icnDocument, bool newItem, const char *id )
	: ADDAC( icnDocument, newItem, id ? id : "dac" )
{
	m_name = i18n("DAC");
	
	for ( int i=0; i<max_ADDAC_bits; ++i )
		m_logic[i] = nullptr;
	
	m_voltagePoint = nullptr;
}


DAC::~DAC()
{
}


void DAC::stepNonLogic()
{
	uint value = 0;
	for ( int i=0; i<m_numBits; ++i )
		value |= ( m_logic[i]->isHigh() ? 1 : 0 ) << i;
	
// 	double valueAsDouble = double(value);
// 	double powChange = std::pow( double(m_numBits), 2 )-1.;
// 	m_voltagePoint->setVoltage( m_range * valueAsDouble / powChange );
	m_voltagePoint->setVoltage( m_range * double(value) / (std::pow( 2, double(m_numBits) )-1.) );
}


void DAC::initPins()
{
	int numBits = dataInt("numBits");
	
	if ( numBits < 2 )
		numBits = 2;
	else if ( numBits > max_ADDAC_bits )
		numBits = max_ADDAC_bits;
	
	if ( numBits == m_numBits )
		return;
	
	QStringList pins;
	
	for ( int i=0; i<numBits; ++i )
		pins += QString::number(i);
	
	int inPos = (numBits+1+(numBits%2))/2;
	for ( int i=numBits-1; i>=inPos; --i )
		pins += "";
	
	pins += "Out";
	
	for ( int i=inPos-2; i>=0; --i )
		pins += "";
	
	initDIPSymbol( pins, 64 );
	initDIP(pins);
	
	if (!m_voltagePoint)
		m_voltagePoint = createVoltagePoint( ecNodeWithID("Out"), 0. );
	
	if ( numBits > m_numBits )
	{
		for ( int i=m_numBits; i<numBits; ++i )
		{
			ECNode *node = ecNodeWithID( QString::number(i) );
			m_logic[i] = createLogicIn(node);
		}
	}
	else
	{
		for ( int i=numBits; i<m_numBits; ++i )
		{
			QString id = QString::number(i);
			removeDisplayText(id);
			removeElement( m_logic[i], false );
			removeNode(id);
			m_logic[i] = nullptr;
		}
	}
	
	m_numBits = numBits;
}
//END class DAC

