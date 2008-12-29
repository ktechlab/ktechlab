/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "component.h"
#include "gpsimprocessor.h"
#include "pin.h"
#include "simulator.h"
#include "switch.h"

#include <kstaticdeleter.h>
#include <qtimer.h>
#include <cassert>

//BEGIN class Simulator
Simulator *Simulator::m_pSelf = 0;
static KStaticDeleter<Simulator> staticSimulatorDeleter;

Simulator *Simulator::self() {
	if (!m_pSelf)
		staticSimulatorDeleter.setObject(m_pSelf, new Simulator());

	return m_pSelf;
}

Simulator::Simulator()
		:  m_stepNumber(0), m_currentChain(0), m_bIsSimulating(true) {
	m_gpsimProcessors = new list<GpsimProcessor*>;
	m_componentCallbacks = new list<ComponentCallback>;
	m_components	   = new list<Component*>;
	m_ordinaryCircuits = new list<Circuit*>;

// use integer math for these, update period is double. 
	unsigned max = unsigned(LOGIC_UPDATE_RATE / LINEAR_UPDATE_RATE);

	for (unsigned i = 0; i < max; i++) {
		m_pStartStepCallback[i] = 0;
	}

	LogicConfig lc;

	m_pChangedLogicStart = new LogicOut(lc, false);
	m_pChangedLogicLast  = m_pChangedLogicStart;

	m_pChangedCircuitStart = new Circuit;
	m_pChangedCircuitLast  = m_pChangedCircuitStart;

	QTimer *stepTimer = new QTimer(this);
	connect(stepTimer, SIGNAL(timeout()), this, SLOT(step()));
	stepTimer->start(1);
}

Simulator::~Simulator() {
	delete m_pChangedLogicStart;
	delete m_pChangedCircuitStart;

	delete m_gpsimProcessors;
	delete m_components;
	delete m_componentCallbacks;
	delete m_ordinaryCircuits;
}

void Simulator::step() {
	if (!m_bIsSimulating) return;

	// We are called a thousand times a second (the maximum allowed by QTimer),
	// so divide the LINEAR_UPDATE_RATE by 1e3 for the number of loops we need
	// to do.
	const unsigned maxSteps = unsigned(LINEAR_UPDATE_RATE / 1e3);

	for (unsigned i = 0; i < maxSteps; ++i) {
		m_stepNumber++;

		// Update the non-logic parts of the simulation
		{
			list<Component*>::iterator components_end = m_components->end();

			for (list<Component*>::iterator component = m_components->begin(); component != components_end; component++) {
				(*component)->stepNonLogic();
			}
		}

		{
			list<Circuit*>::iterator circuits_end = m_ordinaryCircuits->end();

			for (list<Circuit*>::iterator circuit = m_ordinaryCircuits->begin(); circuit != circuits_end; circuit++) {
				(*circuit)->doNonLogic();
			}
		}

		// Update the logic parts of our simulation
		const unsigned max = unsigned(LOGIC_UPDATE_RATE / LINEAR_UPDATE_RATE);

		for (m_llNumber = 0; m_llNumber < max; ++m_llNumber) {
			// Update the logic components
			{
				list<ComponentCallback>::iterator callbacks_end = m_componentCallbacks->end();

				for (list<ComponentCallback>::iterator callback = m_componentCallbacks->begin(); callback != callbacks_end; callback++) {
					callback->callback();
				}
			}

			if (m_pStartStepCallback[m_llNumber]) {
				list<ComponentCallback*>::iterator callbacks_end = m_pStartStepCallback[m_llNumber]->end();

				for (list<ComponentCallback*>::iterator callback = m_pStartStepCallback[m_llNumber]->begin(); callback != callbacks_end; callback++) {
					(*callback)->callback();
					// should we delete the list entry?
				}
			}

			delete m_pStartStepCallback[m_llNumber];
			m_pStartStepCallback[m_llNumber] = 0;

#ifndef NO_GPSIM
			// Update the gpsim processors
			{
				list<GpsimProcessor*>::iterator processors_end = m_gpsimProcessors->end();

				for (list<GpsimProcessor*>::iterator processor = m_gpsimProcessors->begin(); processor != processors_end; processor++) {
					(*processor)->executeNext();
				}
			}
#endif

			// why do we change this here instead of later?
			int prevChain = m_currentChain;
			m_currentChain ^= 1;

			// Update the non-logic circuits
			if (Circuit *changed = m_pChangedCircuitStart->nextChanged(prevChain)) {
				for (Circuit *circuit = changed; circuit; circuit = circuit->nextChanged(prevChain))
					circuit->setCanAddChanged(true);

				m_pChangedCircuitStart->setNextChanged(0, prevChain);
				m_pChangedCircuitLast = m_pChangedCircuitStart;

				do {
					Circuit *next = changed->nextChanged(prevChain);
					changed->setNextChanged(0, prevChain);
					changed->doLogic();
					changed = next;
				} while (changed);
			}

			// Call the logic callbacks
			if (LogicOut *changed = m_pChangedLogicStart->nextChanged(prevChain)) {
				for (LogicOut *out = changed; out; out = out->nextChanged(prevChain))
					out->setCanAddChanged(true);

				m_pChangedLogicStart->setNextChanged(0, prevChain);
				m_pChangedLogicLast = m_pChangedLogicStart;

				do {
					LogicOut *next = changed->nextChanged(prevChain);
					changed->setNextChanged(0, prevChain);

					double v = changed->isHigh() ? changed->outputHighVoltage() : 0.0;

					for (PinList::iterator it = changed->pinListBegin; it != changed->pinListEnd; ++it) {
						if (Pin *pin = *it)
							pin->setVoltage(v);
					}

					LogicIn *logicCallback = changed;

					while (logicCallback) {
						logicCallback->callCallback();
						logicCallback = logicCallback->nextLogic();
					}

					changed = next;
				} while (changed);
			}
		}
	}
}

void Simulator::slotSetSimulating(const bool simulate) {
	if (m_bIsSimulating == simulate) return;

	m_bIsSimulating = simulate;
	emit simulatingStateChanged(simulate);
}

void Simulator::createLogicChain(LogicOut *logicOut, const LogicInList &logicInList, const PinList &pinList) {
	if (!logicOut) return;

	bool state = logicOut->outputState();

	logicOut->setUseLogicChain(true);
	logicOut->pinList = pinList;
	logicOut->pinListBegin = logicOut->pinList.begin();
	logicOut->pinListEnd = logicOut->pinList.end();

	LogicIn *last = logicOut;

	const LogicInList::const_iterator end = logicInList.end();

	for (LogicInList::const_iterator it = logicInList.begin(); it != end; ++it) {
		LogicIn *next = *it;
		last->setNextLogic(next);
		last->setLastState(state);
		last = next;
	}

	last->setNextLogic(0);
	last->setLastState(state);

	// Mark it as changed, if it isn't already changed...
	LogicOut *changed = m_pChangedLogicStart->nextChanged(m_currentChain);

	while (changed) {
		if (changed == logicOut) return;

		changed = changed->nextChanged(m_currentChain);
	}

	addChangedLogic(logicOut);
	logicOut->setCanAddChanged(false);

	if (!m_logicChainStarts.contains(logicOut))
		m_logicChainStarts << logicOut;
}

void Simulator::attachGpsimProcessor(GpsimProcessor *cpu) {
	m_gpsimProcessors->push_back(cpu);
}

void Simulator::detachGpsimProcessor(GpsimProcessor *cpu) {
	m_gpsimProcessors->remove(cpu);
}

void Simulator::attachComponentCallback(Component *component, VoidCallbackPtr function) {
	m_componentCallbacks->push_back(ComponentCallback(component, function));
}

void Simulator::attachComponent(Component *component) {
	if (!component || !component->doesStepNonLogic())
		return;

	m_components->push_back(component);
}

void Simulator::detachComponent(Component *component) {
	m_components->remove(component);
	detachComponentCallbacks(*component);
}

static Component *compx;

bool pred1(ComponentCallback &x) {
	return x.component() == compx;
}

void Simulator::detachComponentCallbacks(Component &component) {
	compx = &component;
	m_componentCallbacks->remove_if(pred1);
}

void Simulator::attachCircuit(Circuit *circuit) {
	if (!circuit) return;

	m_ordinaryCircuits->push_back(circuit);

//	if ( circuit->canAddChanged() ) {
	addChangedCircuit(circuit);
	circuit->setCanAddChanged(false);
//	}
}

void Simulator::removeLogicInReferences(LogicIn *logicIn) {
	if (!logicIn) return;

	QValueList<LogicOut*>::iterator end = m_logicChainStarts.end();

	for (QValueList<LogicOut*>::iterator it = m_logicChainStarts.begin(); it != end; ++it) {
		LogicIn *logicCallback = *it;

		while (logicCallback) {
			if (logicCallback->nextLogic() == logicIn)
				logicCallback->setNextLogic(logicCallback->nextLogic()->nextLogic());

			logicCallback = logicCallback->nextLogic();
		}
	}
}

void Simulator::removeLogicOutReferences(LogicOut *logic) {
	m_logicChainStarts.remove(logic);

	// Any changes to the code below will probably also apply to Simulator::detachCircuit
	if (m_pChangedLogicLast == logic) {
		LogicOut *previous_1 = 0;
		LogicOut *previous_2 = 0;

		for (LogicOut *logic = m_pChangedLogicStart; logic;) {
			if (previous_1)
				previous_2 = previous_1;

			previous_1 = logic;
			logic = logic->nextChanged(m_currentChain);
		}

		m_pChangedLogicLast = previous_2;
	}

	for (unsigned chain = 0; chain < 2; ++chain) {
		for (LogicOut *prevChanged = m_pChangedLogicStart; prevChanged; prevChanged = prevChanged->nextChanged(chain)) {
			LogicOut *nextChanged = prevChanged->nextChanged(chain);

			if (nextChanged == logic)
				prevChanged->setNextChanged(nextChanged->nextChanged(chain), chain);
		}
	}
}

void Simulator::detachCircuit(Circuit *circuit) {
	if (!circuit) return;

	m_ordinaryCircuits->remove(circuit);

	// Any changes to the code below will probably also apply to Simulator::removeLogicOutReferences

	if (m_pChangedCircuitLast == circuit) {
		Circuit *previous_1 = 0;
		Circuit *previous_2 = 0;

		for (Circuit * circuit = m_pChangedCircuitStart; circuit;) {
			if (previous_1)
				previous_2 = previous_1;

			previous_1 = circuit;
			circuit = circuit->nextChanged(m_currentChain);
		}

		m_pChangedCircuitLast = previous_2;
	}

	for (unsigned chain = 0; chain < 2; ++chain) {
		for (Circuit *prevChanged = m_pChangedCircuitStart; prevChanged; prevChanged = prevChanged->nextChanged(chain)) {
			Circuit *nextChanged = prevChanged->nextChanged(chain);

			if (nextChanged == circuit)
				prevChanged->setNextChanged(nextChanged->nextChanged(chain), chain);
		}
	}
}

//END class Simulator

#include "simulator.moc"

