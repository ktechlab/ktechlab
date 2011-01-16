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
// #include "ecnode.h"
#include "circuit.h"
#include "logic.h"
// #include "libraryitem.h"
#include "pin.h"

#include <variant.h>

#include <QStringList>

#include <cmath>
// #include <kiconloader.h>
// #include <klocale.h>

//BEGIN class ADDAC
ADDAC::ADDAC(Circuit& ownerCircuit) : Component(ownerCircuit)
{
	m_numBits = 0;
	m_range = 0;

    Property *bits = new Property("numBits", Variant::Type::Int);
    bits->setCaption( tr("Number Bits") );
    bits->setMinValue(2);
    bits->setMaxValue(max_ADDAC_bits);
    bits->setValue(2);
    addProperty(bits);

    Property *range = new Property("range", Variant::Type::Double);
    range->setCaption( tr("Input Range") );
    range->setUnit("V");
    range->setMinValue(-1e12);
    range->setMaxValue(1e12);
    range->setValue(5);
    addProperty(range);

    /*
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
    */
    ownerCircuit.addComponent(*this);
}

ADDAC::~ADDAC()
{
}

void ADDAC::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    if(theProperty.name() == "range"){
        m_range = newValue.asDouble();
    }
    if(theProperty.name() == "numBits"){
        initPins();
    }
    Q_UNUSED(oldValue);
}

/*
void ADDAC::dataChanged()
{
	m_range = dataDouble("range");
	initPins();
}
*/

//END class ADDAC

//BEGIN class ADC
ADC::~ADC()
{
}

ADC::ADC(Circuit& ownerCircuit) : ADDAC(ownerCircuit)
{
	
	for ( int i=0; i<max_ADDAC_bits; ++i )
		m_logic[i] = 0;
	
	m_realNode = 0;
}

void ADC::stepNonLogic()
{
	double floatBitValue = m_realNode->voltage() * (std::pow( 2, double(m_numBits) )-1.) / m_range;
	double roundBitValue = std::floor( floatBitValue + 0.5 );
	
	if ( roundBitValue < 0 )
	{
		for ( int i = 0; i<m_numBits; ++i )
			m_logic[i]->setHigh(false);
		return;
	}
	
	uint roundedBitValue = uint(roundBitValue);
	for(int i = 0; i < m_numBits; ++i)
		m_logic[i]->setHigh(roundedBitValue & (1 << i));
}

void ADC::initPins()
{

    int numBits = propertyByName("numBits")->value().asInt();
        // dataInt("numBits");
	
	if ( numBits < 2 )
		numBits = 2;
	else if ( numBits > max_ADDAC_bits )
		numBits = max_ADDAC_bits;
	
	if ( numBits == m_numBits )
		return;
	
	QStringList pins;
	
	int inPos = (numBits - 1 + (numBits % 2)) / 2;
	for(int i=0; i < inPos; ++i)
		pins += "";
	
	pins += "In";
	
	for(int i = inPos + 1; i < numBits; ++i)
		pins += "";

	for(int i = numBits - 1; i >= 0; --i)
		pins += QString::number(i);

//	if(!m_realNode)
//		m_realNode =  ecNodeWithID("In");

	if(numBits > m_numBits) {
		for(int i = m_numBits; i < numBits; ++i) {

			m_logic[i] = new LogicOut(LogicConfig(), false);
		}
	} else {
		for(int i = numBits; i < m_numBits; ++i) {
			QString id = QString::number(i);
            // FIXME cleanup of elements: logic outs and pins
            /*
			removeDisplayText(id);
			removeElement(m_logic[i], false);
			removeNode(id);
            */
			m_logic[i] = 0;
		}
	}
	m_numBits = numBits;
}
//END class ADC

//BEGIN class DAC
DAC::DAC(Circuit& ownerCircuit) : ADDAC(ownerCircuit)
{
	for ( int i=0; i<max_ADDAC_bits; ++i )
		m_logic[i] = 0;
	
	m_voltagePoint = 0;
}

DAC::~DAC()
{
}

void DAC::stepNonLogic()
{
	uint value = 0;
	for ( int i=0; i < m_numBits; ++i )
		value |= ( m_logic[i]->isHigh() ? 1 : 0 ) << i;
	
// 	double valueAsDouble = double(value);
// 	double powChange = std::pow( double(m_numBits), 2 )-1.;
// 	m_voltagePoint->setVoltage( m_range * valueAsDouble / powChange );
	m_voltagePoint.setVoltage( m_range * double(value) / (std::pow( 2, double(m_numBits) )-1.) );
}

void DAC::initPins()
{
    int numBits = propertyByName("numBits")->value().asInt();
            // FIXME dataInt("numBits");
	
	if(numBits < 2) numBits = 2;
	else if(numBits > max_ADDAC_bits)
		numBits = max_ADDAC_bits;
	
	if(numBits == m_numBits) return;
	
	QStringList pins;
	
	for(int i = 0; i < numBits; ++i)
		pins += QString::number(i);
	
	int inPos = (numBits + 1 + (numBits % 2)) / 2;
	for(int i = numBits - 1; i >= inPos; --i)
		pins += "";

	pins += "Out";

	for(int i = inPos - 2; i >= 0; --i)
		pins += "";
	
	if(numBits > m_numBits) {
		for(int i = m_numBits; i < numBits; ++i)
		{
			m_logic[i] = new LogicIn(LogicConfig());
		}
	} else {
		for(int i = numBits; i < m_numBits; ++i)
		{
			QString id = QString::number(i);
            // FIXME element removal
            /*
			removeDisplayText(id);
			removeElement(m_logic[i], false);
			removeNode(id);
            */
			m_logic[i] = 0;
		}
	}
	
	m_numBits = numBits;
}
//END class DAC

