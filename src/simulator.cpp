/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "gpsimprocessor.h"
#include "simulator.h"
#include "switch.h"

#include "component.h"
#include "pin.h"

// #include <kstaticdeleter.h>
#include <qtimer.h>
#include <cassert>

//BEGIN class Simulator
Simulator *Simulator::m_pSelf = 0;
// static KStaticDeleter<Simulator> staticSimulatorDeleter;

// FIXME: our global simulator is not cleaned up on shutdown. 
Simulator *Simulator::self() {
	if (!m_pSelf)
	    m_pSelf = new Simulator();
//		staticSimulatorDeleter.setObject(m_pSelf, new Simulator());

	return m_pSelf;
}

void Simulator::destroy()
{
    if(m_pSelf){
        delete m_pSelf;
        m_pSelf = 0;
    }
}


Simulator::Simulator()
		:  m_bIsSimulating(true), m_stepNumber(0) {
	m_gpsimProcessors = new list<GpsimProcessor*>;
	m_components	   = new list<Component*>;
	m_ordinaryCircuits = new list<Circuit*>;

// use integer math for these, update period is double. 
	unsigned max = unsigned(LOGIC_UPDATE_RATE / LINEAR_UPDATE_RATE);

	for (unsigned i = 0; i < max; i++) {
		m_pStartStepCallback[i] = 0;
	}

	LogicConfig lc;

	m_stepTimer = new QTimer(this);
	connect(m_stepTimer, SIGNAL(timeout()), this, SLOT(step()));
	m_stepTimer->start(1);
}

Simulator::~Simulator() {
    disconnect(m_stepTimer, SIGNAL(timeout()), this, SLOT(step()));
    delete m_stepTimer;
	delete m_gpsimProcessors;
	delete m_components;
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
			for (list<Component*>::iterator component = m_components->begin(); component != components_end; ++component) {
				(*component)->stepNonLogic();
			}
		}
// TODO: refactor various classes so the above and below can be combined; hopefully.
		{
			list<Circuit*>::iterator circuits_end = m_ordinaryCircuits->end();
			for (list<Circuit*>::iterator circuit = m_ordinaryCircuits->begin(); circuit != circuits_end; ++circuit) {
				(*circuit)->doNonLogic();
			}
		}

		// Update the logic parts of our simulation
		const unsigned max = unsigned(LOGIC_UPDATE_RATE / LINEAR_UPDATE_RATE);
		for (m_llNumber = 0; m_llNumber < max; ++m_llNumber) {

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
                            unsigned int clockNumber; // current clock count in inner loop
                            unsigned int clockTotal; // max. clocks for the given gpsim processor
                            
                            // for each processor... 
				list<GpsimProcessor*>::iterator processors_end = m_gpsimProcessors->end();
				for (list<GpsimProcessor*>::iterator processor = m_gpsimProcessors->begin(); processor != processors_end; ++processor) {
                                    // .. get the number of steps it should execute ...
                                    clockTotal = (*processor)->stepsPerMicrosecond();
                                    for( clockNumber = 0; clockNumber < clockTotal; clockNumber++)
                                        // ... and run them
					(*processor)->executeNext();
				}
			}
#endif
			// Update the non-logic circuits
			{
			list<Circuit*>::iterator end = m_ordinaryCircuits->end();
			for(list<Circuit*>::iterator it = m_ordinaryCircuits->begin(); it != end; ++it) {
				Circuit *changed = *it;

				if(changed->isChanged()) {
					changed->clearChanged();
					changed->doLogic();
				}
			}
			}

			// Call the logic callbacks
			{
			std::set<LogicOut*>::iterator end = m_logicChainStarts.end();
// WARNING: it makes a huge performance difference whether the ++ is before or after the iterator!!!!! 
			for(std::set<LogicOut*>::iterator it = m_logicChainStarts.begin(); it != end ;++it) {
				LogicOut *changed = *it;

				if(changed->isChanged()) {
					double v = changed->isHigh() ? changed->outputHighVoltage() : 0.0;
					for (PinSet::iterator it = changed->logicPinList.begin(); it != changed->logicPinList.end(); ++it) {
						if (Pin *pin = *it)
							pin->setVoltage(v);
					}

					changed->clearChanged();
					changed->callCallbacks();
				}
			}
			}
		}
	}
}

void Simulator::slotSetSimulating(bool simulate) {
	if (m_bIsSimulating == simulate) return;

	m_bIsSimulating = simulate;
	emit simulatingStateChanged(simulate);
}

void Simulator::createLogicChain(LogicOut *logicOut) {
	assert(logicOut);

	logicOut->setElementSet(0);
	logicOut->setChain();
	logicOut->setChanged();

	m_logicChainStarts.insert(logicOut);
}

void Simulator::attachGpsimProcessor(GpsimProcessor *cpu) {
	m_gpsimProcessors->push_back(cpu);
}

void Simulator::detachGpsimProcessor(GpsimProcessor *cpu) {
	m_gpsimProcessors->remove(cpu);
}

void Simulator::attachComponent(Component *component) {
	if (!component || !component->doesStepNonLogic())
		return;

	m_components->push_back(component);
}

void Simulator::detachComponent(Component *component) {
	m_components->remove(component);
}

void Simulator::attachCircuit(Circuit *circuit) {
	if (!circuit) return;

	m_ordinaryCircuits->push_back(circuit);

	circuit->setChanged();
}

void Simulator::removeLogicInReferences(LogicIn *logicIn) {
	if (!logicIn) return;

	std::set<LogicOut*>::iterator end = m_logicChainStarts.end();
	for(std::set<LogicOut*>::iterator it = m_logicChainStarts.begin(); it != end; ++it) {
		(*it)->removeDependent(logicIn);
	}
}

void Simulator::removeLogicOutReferences(LogicOut *logic) {
	m_logicChainStarts.erase(logic);
}

void Simulator::detachCircuit(Circuit *circuit) {
	m_ordinaryCircuits->remove(circuit);
}
//END class Simulator

#include "simulator.moc"

