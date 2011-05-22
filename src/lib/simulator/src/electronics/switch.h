/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SWITCH_H
#define SWITCH_H

#include <qobject.h>
#include "resistance.h"
#include "simulatorexport.h"

class CircuitDocument;
class Component;
class Pin;
class QTimer;
class Wire;

/**
@author David Saxton
*/
class SIMULATOR_EXPORT Switch : public QObject {
	Q_OBJECT
public:
	enum State {
		Open,
		Closed };

	Switch(Component *parent, Pin &p1, Pin &p2, State state);
	~Switch();
	/**
	 * If bouncing has been set to true, then the state will not switch
	 * immediately to that given.
	 */
	void setState(State state);
	State state() const { return m_state; }

	/**
	Obtain the switch's other pin, if its closed. 
	*/
	Pin *otherPinIfClosed(const Pin *aPin);

	/**
	 * Tell the switch whether to bounce or not, for the given duration,
	 * when the state is changed.
	 */
	void setBounce(bool bounce, int msec = 5);
	/**
	 * Tell the switch to continue bouncing (updates the resistance value).
	 * Called from the simulator.
	 */
	void bounce();

protected slots:
	/**
	 * Called from a QTimer timeout - our bouncing period has come to an
	 * end. This will then fully disconnect or connect the pins depending
	 * on the current state.
	 */
	void stopBouncing();

protected:
	void startBouncing();

	bool m_bBounce;
	int m_bouncePeriod_ms;
	unsigned long long m_bounceStart; // Simulator time that bouncing started
	Resistance m_pBounceResistance;
	Wire *m_pJumper;

// ***
	State m_state;
// ***
	Component *m_pComponent;

// because we aren't a component, we can't own our pins, 
// because we aren't an element, we can't pretend pins don't exist. =(
	Pin *m_pP1;
	Pin *m_pP2;
	QTimer *m_pStopBouncingTimer;
};

#endif
