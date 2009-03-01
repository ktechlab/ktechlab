/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <cmath>
#include <stdlib.h> // for rand
#include <time.h>

#include <kdebug.h>
#include <qtimer.h>

#include "circuitdocument.h"
#include "component.h"
#include "ecnode.h"
#include "pin.h"
#include "resistance.h"
#include "simulator.h"
#include "switch.h"

Switch::Switch(Component *parent, Pin *p1, Pin *p2, State state) {
	m_bouncePeriod_ms = 5;
	m_bBounce = false;
	m_bounceStart = 0;
	m_pBounceResistance = 0;
	m_pP1 = p1;
	m_pP2 = p2;
	m_pComponent = parent;
	m_pStopBouncingTimer = new QTimer(this);
	connect(m_pStopBouncingTimer, SIGNAL(timeout()), this, SLOT(stopBouncing()));

	// Force update
	m_state = (state == Open) ? Closed : Open;
	setState(state);
}

Switch::~ Switch() {
	if (m_pP1) m_pP1->setSwitchConnected(m_pP2, false);
	if (m_pP2) m_pP2->setSwitchConnected(m_pP1, false);
}

void Switch::setState(State state) {
	if (m_state == state) return;

	m_state = state;

	if (m_bBounce) startBouncing();
	else {
		// I'm being lazy...calling stopBouncing will connect the stuff
		stopBouncing();
	}
}

void Switch::setBounce(bool bounce, int msec) {
	m_bBounce = bounce;
	m_bouncePeriod_ms = msec;
}

void Switch::startBouncing() {
	if (m_pBounceResistance) {
		// Already active?
		return;
	}

	if (!m_pComponent->circuitDocument()) return;

// 	kdDebug() << k_funcinfo << endl;

//	m_pBounceResistance = m_pComponent->createResistance(m_pP1, m_pP2, 10000);
	m_pBounceResistance = new Resistance(10000);
	m_pComponent->setup2pinElement(m_pBounceResistance, m_pP1, m_pP2);

	m_bounceStart = Simulator::self()->time();

//FIXME: I broke the bounce feature when I cleaned this out of the simulator,
// should probably be put into circuit document or some other solution which doesn't
// contaminate that many other classes.

//	Simulator::self()->attachSwitch( this );
// 	kdDebug() << "m_bounceStart="<<m_bounceStart<<" m_bouncePeriod_ms="<<m_bouncePeriod_ms<<endl;

	// initialize random generator
	srand(time(NULL));

	// Give our bounce resistor an initial value
	bounce();
}

void Switch::bounce() {
	int bounced_ms = ((Simulator::self()->time() - m_bounceStart) * 1000) / LOGIC_UPDATE_RATE;

	if (bounced_ms >= m_bouncePeriod_ms) {
		if (!m_pStopBouncingTimer->isActive())
			m_pStopBouncingTimer->start(0, true);

		return;
	}

	double g = double(rand()) / double(RAND_MAX);

	// 4th power of the conductance seems to give a nice distribution
	g = pow(g, 4);
	m_pBounceResistance->setConductance(g);
}

void Switch::stopBouncing() {
//	Simulator::self()->detachSwitch( this );
	m_pComponent->removeElement(m_pBounceResistance, true);
	m_pBounceResistance = 0;

	bool connected = (m_state == Closed);

	if (m_pP1 && m_pP2) {
		m_pP1->setSwitchConnected(m_pP2, connected);
		m_pP2->setSwitchConnected(m_pP1, connected);
	}

	if (CircuitDocument *cd = m_pComponent->circuitDocument())
		cd->requestAssignCircuits();
}

bool Switch::calculateCurrent() {
	if (!m_pP1 || !m_pP2) return false;

	if (state() == Open) {
		m_pP1->setSwitchCurrentKnown(this);
		m_pP2->setSwitchCurrentKnown(this);
		return true;
	}

	if(!(*m_pP1).calculateCurrentFromWires() || !(*m_pP2).calculateCurrentFromWires())
		return false;

// are we supposed to use + or - here? 
	double current = (*m_pP1).current() + (*m_pP2).current();

	m_pP1->setSwitchCurrentKnown(this);
	m_pP2->setSwitchCurrentKnown(this);

// same question about the +es and -es.
	m_pP1->mergeCurrent(current);
	m_pP2->mergeCurrent(-current);

	return true;
}

#include "switch.moc"

