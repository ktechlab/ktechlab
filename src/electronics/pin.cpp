/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <cassert>
#include <kdebug.h>

#include "pin.h"
#include "switch.h"

Pin::Pin() :
	m_voltage(0), m_sourceCurrent(0), m_eqId(-2), m_groundType(Pin::gt_never)
{}

Pin::~Pin() {
	WireList::iterator end = m_wireList.end();

	while(!m_wireList.empty()) {
		delete (Wire *)(*(m_wireList.begin()));
	}
}

void Pin::setVoltage(double v)
{
    m_voltage = v;
}

double Pin::voltage() const
{
    return m_voltage;
}

PinSet Pin::localConnectedPins() //const 
{
	PinSet pins;

	WireList::iterator end = m_wireList.end();
	for(WireList::iterator it = m_wireList.begin(); it != end; ++it) {
		assert(*it);

		Pin *op = (*it)->otherPin(this);

		pins.insert(op);
	}

	return pins;
}

/**/
void Pin::addCircuitDependentPin(Pin *pin) {
	if(pin) m_circuitDependentPins.insert(pin);
}

void Pin::addGroundDependentPin(Pin *pin) {
	if(pin) m_groundDependentPins.insert(pin);
}

void Pin::removeDependentPins() {
	m_circuitDependentPins.clear();
	m_groundDependentPins.clear();
}


/// Element add and remove... What is this really for? 
void Pin::addElement(Element *e) {
	assert(e);
	m_elementList.insert(e);
}

void Pin::removeElement(Element *e) {
	m_elementList.erase(e);
}
// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

void Pin::addWire(Wire *wire) {
	assert(wire);

	m_wireList.append(wire);
}

void Pin::removeWire(Wire *aWire) {
	m_wireList.removeAll(aWire);
}

bool Pin::currentIsKnown() const
{
    return m_currentIsKnown;
}

bool Pin::setCurrentKnown(bool known)
{
    m_currentIsKnown = known;
}

double Pin::sourceCurrent() const
{
    if(!currentIsKnown()){
        qWarning() << "BUG: asking for a current which is not known!";
    }
    if( m_elementList.empty())
        return 0;
    return m_sourceCurrent;
}

void Pin::setSourceCurrent(double current) 
{
    m_currentIsKnown = true;
    m_sourceCurrent = current;
}


const WireList Pin::wires() const
{
    return m_wireList;
}


void Pin::setEqId(int id)
{
    m_eqId = id;
}


int Pin::eqId() const
{
    return m_eqId;
}
