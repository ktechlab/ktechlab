/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "discretelogic.h"
#include "logic.h"
#include "simulator.h"




//BEGIN class Inverter
Inverter::Inverter()
		: Component() {

	m_pOut.setState(true);
	m_pIn.setCallback(this, (CallbackPtr)(&Inverter::inStateChanged));
	inStateChanged(false);
}

Inverter::~Inverter() {
}

void Inverter::inStateChanged(bool newState) {
	m_pOut.setHigh(!newState);
}

//END class Inverter

//BEGIN class Buffer

Buffer::Buffer()
		: Component() {

	m_pIn.setCallback(this, (CallbackPtr)(&Buffer::inStateChanged));
	inStateChanged(false);
}

Buffer::~Buffer() {
}

void Buffer::inStateChanged(bool newState) {
	m_pOut.setHigh(newState);
}
//END class Buffer

//BEGIN class ECLogicInput

ECLogicInput::ECLogicInput()
		: Component() {

    Property *useToggle = new Property("useToggle", Variant::Type::Bool);
	useToggle->setCaption(tr("Use Toggle"));
	useToggle->setValue(true);
    addProperty(useToggle);

    Property *buttonPressed = new Property("buttonPressed", Variant::Type::Bool);
    buttonPressed->setValue(false);
    buttonPressed->setCaption(tr("Button pressed"));
    addProperty(buttonPressed);

    m_outState = false;
    m_toggleOn = true;
}

ECLogicInput::~ECLogicInput() {
}

void ECLogicInput::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(oldValue);

    if( theProperty.name() == "useToggle" ){
        m_toggleOn = newValue.asBool();
    }
    if( theProperty.name() == "buttonPressed" ){
        if(m_toggleOn)
            m_outState != m_outState;
        else
            m_outState = newValue.asBool();

        m_pOut.setHigh(m_outState);
    }
}

//END class ECLogicInput

//BEGIN class ECLogicOutput
ECLogicOutput::ECLogicOutput()
		: Component() {

    outValue = new Property("outputValue", Variant::Type::Double);
    outValue->setMinValue(0);
    outValue->setMaxValue(1);

    m_pIn.setCallback(this, (CallbackPtr)(&ECLogicOutput::inStateChanged));
}

ECLogicOutput::~ECLogicOutput() {
}

void ECLogicOutput::inStateChanged(bool newState) {
	if (m_bLastState == newState) return;

	if(newState)
        outValue->setValue(1.0);
    else
        outValue->setValue(0.0);
}

//END class ECLogicOutput

