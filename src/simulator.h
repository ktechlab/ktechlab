/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <list>

#include "circuit.h"
#include "logic.h"

using namespace std;

/**
This should be a multiple of 1000. It is the number of times a second that
linear elements are updated.
*/
const int LINEAR_UPDATE_RATE = int(1e4);
const double LINEAR_UPDATE_PERIOD = 1.0 / LINEAR_UPDATE_RATE;

/**
This should be a multiple of 1000. It is the number of times a second that
logic elements are updated.
*/
const int LOGIC_UPDATE_RATE = int(1e6);

class Circuit;

class CircuitDocument;

class Component;

class ComponentCallback;

class ECNode;

class GpsimProcessor;

class LogicIn;

class LogicOut;

class Switch;

class Wire;

typedef QList<ECNode*> ECNodeList;
typedef QList<LogicIn*> LogicInList;
typedef void(Component::*VoidCallbackPtr)();

class ComponentCallback {

public:
	ComponentCallback(Component *component, VoidCallbackPtr function) {
		m_pComponent = component;
		m_pFunction = function;
	}

	void callback() {
		(m_pComponent->*m_pFunction)();
	}

	Component *component() const {
		return m_pComponent;
	}

protected:
	Component *m_pComponent;
	VoidCallbackPtr m_pFunction;
};

/**
This singleton class oversees all simulation (keeping in sync linear, nonlinear,
logic, external simulators (such as gpsim), mechanical simulation, etc).
@author David Saxton
*/

class Simulator : public QObject {
	Q_OBJECT

public:
	static Simulator *self();
	~Simulator();

	/**
	 * Number of (1/LOGIC_UPDATE_RATE) intervals that the simulator has been
	 * stepping for.
	 */
	long long time() const {
		return m_stepNumber * (long long)(LOGIC_UPDATE_RATE / LINEAR_UPDATE_RATE) + m_llNumber;
	}

	/**
	 * Initializes a new logic chain.
	 */
	void createLogicChain(LogicOut *logicOut, const LogicInList &logicInList, const PinList &pinList);
	/**
	 * Adds the given LogicOut to the list of changed LogicOuts
	 */
	void addChangedLogic(LogicOut *changed) {
		m_pChangedLogicLast->setNextChanged(changed, m_currentChain);
		m_pChangedLogicLast = changed;
	}

	/**
	 * Remove pointers to the given LogicOut, called when it is deleted for
	 * safety reasons.
	 */
	void removeLogicOutReferences(LogicOut *logic);
	/**
	 * Remove pointers to the given LogicIn, called when it is deleted for
	 * safety reasons. Simulator does not have any references to LogicIns
	 * itself - instead, they are removed from logic chains which are
	 * currently marked as changed.
	 */
	void removeLogicInReferences(LogicIn *logic);
	/**
	 * Adds the given Circuit to the list of changed Circuits
	 */
	void addChangedCircuit(Circuit *changed) {
		m_pChangedCircuitLast->setNextChanged(changed, m_currentChain);
		m_pChangedCircuitLast = changed;
	}

	inline void addStepCallback(int at, ComponentCallback *ccb);
	/**
	 * Add the given processor to the simulator. GpsimProcessor::step will
	 * be called while present in the simulator (it is at GpsimProcessor's
	 * disgression whether to actually step, depending on its running
	 * status).
	 * @see detachGpsimProcessor( GpsimProcessor * cpu );
	 */
	void attachGpsimProcessor(GpsimProcessor *cpu);
	/**
	 * Remove the given processor from the simulation loop
	 */
	void detachGpsimProcessor(GpsimProcessor *cpu);
	/**
	 * Attach the component callback to the simulator. This will be called
	 * during the logic update loop, at LOGIC_UPDATE_RATE times per second (so
	 * make sure the function passed is an efficient one!).
	 */
	void attachComponentCallback(Component *component, VoidCallbackPtr function);
	/**
	 * Removes the callbacks for the given component from the simulator.
	 */
	void detachComponentCallbacks(Component &component);
	/**
	 * Attach the component to the simulator.
	 */
	void attachComponent(Component *component);
	/**
	 * Detaches the component from the simulator.
	 */
	void detachComponent(Component *component);
	/**
	 * Attach a circuit to the simulator
	 */
	void attachCircuit(Circuit *circuit);
	/**
	 * Detach a circuit from the simulator.
	 */
	void detachCircuit(Circuit *circuit);

	/**
	 * @return whether or not we are currently simulating stuff
	 * @see slotSetSimulating
	 */
	bool isSimulating() const {
		return m_bIsSimulating;
	}

signals:
	/**
	 * Emitted when the simulating state changes.
	 * @see slotSetSimulating
	 */
	void simulatingStateChanged(bool isSimulating);

public slots:
	/**
	 * Set whether or not to simulate at the moment.
	 * @see isSimulating
	 */
	void slotSetSimulating(bool simulate);

private slots:
	void step();

private:
	bool m_bIsSimulating;
	static Simulator *m_pSelf;

	///List of LogicOuts that are at the start of a LogicChain
	QList<LogicOut*> m_logicChainStarts;
	list<GpsimProcessor*> *m_gpsimProcessors;

// doesn't look too appropriate.
// essentially a grab bag of every odd *component* that answers "true" to does step non-logic,
// Which is every component that has special UI-related code that needs to be called every time the simulator steps.
// this is not to be confused with elements which have nonLinear and Reactive components. =P
	list<Component*> *m_components;
	list<ComponentCallback> *m_componentCallbacks;
	list<Circuit*> *m_ordinaryCircuits;

// allow a variable number of callbacks be scheduled at each possible time. 
	list<ComponentCallback *> *m_pStartStepCallback[LOGIC_UPDATE_RATE/LINEAR_UPDATE_RATE];

	Circuit *m_pChangedCircuitStart;
	Circuit *m_pChangedCircuitLast;

	LogicOut *m_pChangedLogicStart;
	LogicOut *m_pChangedLogicLast;

	Simulator();
	unsigned long m_llNumber; // simulation clock; Exists only to support the time() callback.
	long long m_stepNumber;

// looks like there are only ever two chains, 0 and 1, code elsewhere toggles between the two...
	unsigned char m_currentChain;
};

inline void Simulator::addStepCallback(int at, ComponentCallback *ccb) {
// code was buggy[er], don't really know what variables are for, rewritten to make it work,
// OK for now.
	if (!m_pStartStepCallback[at]) {
		m_pStartStepCallback[at] = new list<ComponentCallback*>;
	}

	m_pStartStepCallback[at]->push_back(ccb);
}

#endif
