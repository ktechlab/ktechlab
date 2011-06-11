/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecclockinput.h"

#include "logic.h"
#include "simulator.h"
#include "variant.h"

#include <cmath>

// was a constant, this is my guess for an appropriate name.
#define TIME_INTERVAL 100

static inline uint roundDouble(const double x) {
	return uint(std::floor(x + 0.5));
}


ECClockInput::ECClockInput(Circuit& ownerCircuit)
		: Component(ownerCircuit) {

	m_lastSetTime = 0;
	m_high_time = 0;
	m_low_time  = 0;
	m_pSimulator = Simulator::self();

	for (unsigned i = 0; i < 1000; i++) {
		ComponentCallback *ccb = new ComponentCallback(this, (VoidCallbackPtr)(&ECClockInput::stepCallback));
		m_pComponentCallback[i] = new list<ComponentCallback>;
		m_pComponentCallback[i]->push_back(*ccb);
		delete ccb;
	}


    Property *p = 0;
	p = new Property("low-time", Variant::Type::Double);
	p->setUnit("S");
	p->setCaption(tr("Low Time"));
	p->setMinValue(1.0 / LOGIC_UPDATE_RATE);
	p->setValue(0.5);
    addProperty(p);

	p = new Property("high-time", Variant::Type::Double);
	p->setUnit("S");
	p->setCaption(tr("High Time"));
	p->setMinValue(1.0 / LOGIC_UPDATE_RATE);
	p->setValue(0.5);

}

ECClockInput::~ECClockInput() {
	for (unsigned i = 0; i < 1000; i++) {
		delete m_pComponentCallback[i];
	}
}

void ECClockInput::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(theProperty);
    Q_UNUSED(newValue);
    Q_UNUSED(oldValue);

    m_high_time = roundDouble(property("high-time").toDouble() * LOGIC_UPDATE_RATE);
    m_low_time = (property("low-time").toDouble() * LOGIC_UPDATE_RATE);

    m_lastSetTime = m_pSimulator->time();
}


void ECClockInput::stepLogic() {
	m_pOut.setHigh(m_pSimulator->time() > m_low_time);
}

void ECClockInput::stepCallback() {
	m_pOut.setHigh(!m_pOut.isHigh());
}

void ECClockInput::stepNonLogic() {

	bool addingHigh = m_pOut.isHigh();

	long long lowerTime = m_pSimulator->time();
	long long upperTime = lowerTime + TIME_INTERVAL;
	long long upTo = m_lastSetTime;

	while (upTo + (addingHigh ? m_high_time : m_low_time) < upperTime) {
		upTo += addingHigh ? m_high_time : m_low_time;
		addingHigh = !addingHigh;

		long long at = upTo - lowerTime;

		if (at >= 0 && at < TIME_INTERVAL)
			m_pSimulator->addStepCallback(at, &m_pComponentCallback[at]->front());
	}

	m_lastSetTime = upTo;
}
