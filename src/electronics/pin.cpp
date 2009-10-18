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
	m_voltage(0), m_eqId(-2), m_groundType(Pin::gt_never)
{}

Pin::~Pin() {
	WireList::iterator end = m_wireList.end();
	for (WireList::iterator it = m_wireList.begin(); it != end; ++it)
		delete(Wire *)(*it);
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

	m_wireList.insert(wire);
}

void Pin::removeWire(Wire *aWire) {
	m_wireList.erase(aWire);
}

bool Pin::currentIsKnown() const {
	return ((m_wireList.size() == 2 && m_elementList.empty())
		|| (m_wireList.size() < 2 && !m_elementList.empty()));
}

double Pin::calculateCurrentFromWires(Wire *aWire) const {

	double current = 0;

	WireList::const_iterator end = m_wireList.end();
	for(WireList::const_iterator it = m_wireList.begin(); it != end; ++it) {

// might have to do some vodo to figure out which end of the wire we're on and add/subtract as appropriate.
		if(*it != aWire) { 

// TODO: throw an exception here, means that this pin should be moved 
// to the bottom of the working list and tried again after others are checked. 
			if(!(*it)->currentIsKnown())
				return 0;

			current += (*it)->currentFor(this);
		}
	}

	return current;
}

/*!
    \fn Pin::setCurrentIfOneWire(double current)
 */
bool Pin::setCurrentIfOneWire(double current)
{
	if(m_wireList.size() == 1) {
		(*(m_wireList.begin()))->setCurrent(current);
		return true;
	} else { // inform wires that they don't know their current 
		// and have to figure it out for themselves.
		WireList::iterator end = m_wireList.end();
		for(WireList::iterator it = m_wireList.begin(); it != end; ++it) {
			(*it)->setCurrentKnown(false);
		}
		return false;
	}
}

