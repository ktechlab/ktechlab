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


//BEGIN class Simulator
Simulator * Simulator::m_pSelf = 0l;
static KStaticDeleter<Simulator> staticSimulatorDeleter;

Simulator * Simulator::self()
{
	if (!m_pSelf)
		staticSimulatorDeleter.setObject( m_pSelf, new Simulator() );
	return m_pSelf;
}


Simulator::Simulator()
{
	m_currentChain = 0;
	m_llNumber = 0;
	m_stepNumber = 0;
	m_bIsSimulating = true;
	m_gpsimProcessors = 0l;
	m_componentCallbacks = 0l;
	m_components = 0l;
	m_ordinaryCircuits = 0l;
	m_switches = 0l;
	
	unsigned max = unsigned(LOGIC_UPDATE_RATE/LINEAR_UPDATE_RATE);
	for ( unsigned i = 0; i < max; i++ )
	{
		m_pStartStepCallback[i] = 0l;
		m_pNextStepCallback[i] = 0l;
	}
	
	LogicConfig lc;
	m_pChangedLogicStart = new LogicOut( lc, false );
	m_pChangedLogicLast = m_pChangedLogicStart;
	
	m_pChangedCircuitStart = new Circuit;
	m_pChangedCircuitLast = m_pChangedCircuitStart;
	
	QTimer * stepTimer = new QTimer(this);
	connect( stepTimer, SIGNAL(timeout()), this, SLOT(step()) );
	stepTimer->start(1);
}


Simulator::~Simulator()
{
	delete m_pChangedLogicStart;
	delete m_pChangedCircuitStart;
	
	detachAll(m_gpsimProcessors);
	detachAll(m_components);
	detachAll(m_componentCallbacks);
	detachAll(m_ordinaryCircuits);
	detachAll(m_switches);
}


void Simulator::step()
{
	if (!m_bIsSimulating)
		return;
	
	// We are called a thousand times a second (the maximum allowed by QTimer),
	// so divide the LINEAR_UPDATE_RATE by 1e3 for the number of loops we need
	// to do.
	const unsigned maxSteps = unsigned(LINEAR_UPDATE_RATE/1e3);
	for ( unsigned i = 0; i < maxSteps; ++i )
	{
		m_llNumber = 0;
		m_stepNumber++;
	
		// Update the non-logic parts of the simulation
		LinkedList<Component> * component = m_components;
		while (component)
		{
			component->data()->stepNonLogic();
			component = component->m_pNext;
		}
		LinkedList<Circuit> * circuit = m_ordinaryCircuits;
		while (circuit)
		{
			circuit->data()->doNonLogic();
			circuit = circuit->m_pNext;
		}
		LinkedList<Switch> * sw = m_switches;
		while (sw)
		{
			sw->data()->bounce();
			sw = sw->m_pNext;
		}
	
		// Update the logic parts of our simulation
		const unsigned max = unsigned(LOGIC_UPDATE_RATE/LINEAR_UPDATE_RATE);
		for ( m_llNumber = 0; m_llNumber < max; ++m_llNumber )
		{
			// Update the logic components
			LinkedList<ComponentCallback> * callback = m_componentCallbacks;
			while (callback)
			{
				callback->data()->callback();
				callback = callback->m_pNext;
			}
		
			callback = m_pStartStepCallback[m_llNumber];
			while (callback)
			{
				LinkedList<ComponentCallback> * next = callback->m_pNext;
				callback->m_pNext = 0l;
				callback->data()->callback();
				callback = next;
			}
			m_pStartStepCallback[m_llNumber] = 0l;
		
#ifndef NO_GPSIM
			// Update the gpsim processors
			LinkedList<GpsimProcessor> * gpsimProcessor = m_gpsimProcessors;
			while (gpsimProcessor)
			{
				gpsimProcessor->data()->executeNext();
				gpsimProcessor = gpsimProcessor->m_pNext;
			}
#endif
		
		
			int prevChain = m_currentChain;
			m_currentChain = 1 - m_currentChain;
		
		
			// Update the non-logic circuits
			if ( Circuit * changed = m_pChangedCircuitStart->nextChanged(prevChain) )
			{
				for ( Circuit * circuit = changed; circuit; circuit = circuit->nextChanged(prevChain) )
					circuit->setCanAddChanged(true);
			
				m_pChangedCircuitStart->setNextChanged( 0l, prevChain );
				m_pChangedCircuitLast = m_pChangedCircuitStart;
			
				do
				{
					Circuit * next = changed->nextChanged(prevChain);
					changed->setNextChanged( 0l, prevChain );
					changed->doLogic();
					changed = next;
				}
				while (changed);
			}
		
			// Call the logic callbacks
			if (LogicOut * changed = m_pChangedLogicStart->nextChanged(prevChain))
			{
				for ( LogicOut * out = changed; out; out = out->nextChanged(prevChain) )
					out->setCanAddChanged(true);
			
				m_pChangedLogicStart->setNextChanged( 0l, prevChain );
				m_pChangedLogicLast = m_pChangedLogicStart;
				do
				{
					LogicOut * next = changed->nextChanged(prevChain);
					changed->setNextChanged( 0l, prevChain );
			
					double v = changed->isHigh() ? changed->outputHighVoltage() : 0.0;
				
					for ( PinList::iterator it = changed->pinListBegin; it != changed->pinListEnd; ++it )
					{
						if ( Pin * pin = *it )
							pin->setVoltage(v);
					}
			
					LogicIn * logicCallback = changed;
					while (logicCallback)
					{
						logicCallback->callCallback();
						logicCallback = logicCallback->nextLogic();
					}
		
					changed = next;
				}
				while (changed);
			}
		}
	}
}


void Simulator::slotSetSimulating( bool simulate )
{
	if ( m_bIsSimulating == simulate )
		return;
	
	m_bIsSimulating = simulate;
	emit simulatingStateChanged(simulate);
}


void Simulator::createLogicChain( LogicOut * logicOut, const LogicInList & logicInList, const PinList & pinList )
{
	if (!logicOut)
		return;
	
	bool state = logicOut->outputState();
	
	logicOut->setUseLogicChain(true);
	logicOut->pinList = pinList;
	logicOut->pinListBegin = logicOut->pinList.begin();
	logicOut->pinListEnd = logicOut->pinList.end();
	
	LogicIn * last = logicOut;
	const LogicInList::const_iterator end = logicInList.end();
	for ( LogicInList::const_iterator it = logicInList.begin(); it != end; ++it )
	{
		LogicIn * next = *it;
		last->setNextLogic(next);
		last->setLastState(state);
		last = next;
	}
	last->setNextLogic(0l);
	last->setLastState(state);
	
	// Mark it as changed, if it isn't already changed...
	LogicOut * changed = m_pChangedLogicStart->nextChanged(m_currentChain);
	while (changed)
	{
		if ( changed == logicOut )
			return;
		changed = changed->nextChanged(m_currentChain);
	}
	addChangedLogic(logicOut);
	logicOut->setCanAddChanged(false);
	
	if ( !m_logicChainStarts.contains( logicOut ) )
		m_logicChainStarts << logicOut;
}


template <typename T>
void Simulator::attach( LinkedList<T> ** start, T * data )
{
	if (!data)
		return;
	
	while ( *start && (*start)->m_pNext )
	{
		if ( (*start)->data() == data )
			return;
		start = & (*start)->m_pNext;
	}
	
	if (*start)
		(*start)->m_pNext = new LinkedList<T>(data);
	else
		*start = new LinkedList<T>(data);
}


template <typename T>
void Simulator::detach( LinkedList<T> ** start, T * data )
{
	if (!data)
		return;
	
	while (*start)
	{
		if ( (*start)->data() == data )
		{
			LinkedList<T> * toDelete = *start;
			*start = (*start)->m_pNext;
			delete toDelete;
			return;
		}
		
		start = & (*start)->m_pNext;
	}
}


template <typename T>
void Simulator::detachAll( LinkedList<T> * list )
{
	while (list)
	{
		LinkedList<T> * next = list->m_pNext;
		delete list;
		list = next;
	}
}


void Simulator::attachGpsimProcessor( GpsimProcessor * cpu )
{
	attach( & m_gpsimProcessors, cpu );
}


void Simulator::detachGpsimProcessor( GpsimProcessor * cpu )
{
	detach( & m_gpsimProcessors, cpu );
}


void Simulator::attachComponentCallback( Component * component, VoidCallbackPtr function )
{
	attach( & m_componentCallbacks, new ComponentCallback( component, function ) );
}


void Simulator::attachComponent( Component * component )
{
	if ( !component || !component->doesStepNonLogic() )
		return;
	
	attach( & m_components, component );
}


void Simulator::detachComponent( Component * component )
{
	detach( & m_components, component );
	detachComponentCallbacks(component);
}


void Simulator::attachSwitch( Switch * sw )
{
	attach( & m_switches, sw );
}


void Simulator::detachSwitch( Switch * sw )
{
	detach( & m_switches, sw );
}


void Simulator::detachComponentCallbacks( Component * component )
{
	LinkedList<ComponentCallback> * callback = m_componentCallbacks;
	while (callback)
	{
		LinkedList<ComponentCallback> * next = callback->m_pNext;
		ComponentCallback * data = callback->data();
		if ( data->component() == component )
		{
			detach( & m_componentCallbacks, data );
			delete data;
		}
		callback = next;
	}
}


void Simulator::attachCircuit( Circuit * circuit )
{
	if (!circuit)
		return;
	attach( & m_ordinaryCircuits, circuit );
	
	if ( circuit->canAddChanged() )
	{
		addChangedCircuit(circuit);
		circuit->setCanAddChanged(false);
	}
}


void Simulator::removeLogicInReferences( LogicIn * logicIn )
{
	if ( !logicIn )
		return;
	
	QValueList<LogicOut*>::iterator end = m_logicChainStarts.end();
	for ( QValueList<LogicOut*>::iterator it = m_logicChainStarts.begin(); it != end; ++it )
	{
		LogicIn * logicCallback = *it;
		while (logicCallback)
		{
			if ( logicCallback->nextLogic() == logicIn )
				logicCallback->setNextLogic( logicCallback->nextLogic()->nextLogic() );
			logicCallback = logicCallback->nextLogic();
		}
	}
}


void Simulator::removeLogicOutReferences( LogicOut * logic )
{
	m_logicChainStarts.remove( logic );
	
	// Any changes to the code below will probably also apply to Simulator::detachCircuit
	
	if ( m_pChangedLogicLast == logic )
	{
		LogicOut * previous_1 = 0l;
		LogicOut * previous_2 = 0l;
		for ( LogicOut * logic = m_pChangedLogicStart; logic; )
		{
			if (previous_1)
				previous_2 = previous_1;
			previous_1 = logic;
			logic = logic->nextChanged( m_currentChain );
		}
		
		m_pChangedLogicLast = previous_2;
	}
	
	for ( unsigned chain = 0; chain < 2; ++chain )
	{
		for ( LogicOut * prevChanged = m_pChangedLogicStart; prevChanged; prevChanged = prevChanged->nextChanged( chain ) )
		{
			LogicOut * nextChanged = prevChanged->nextChanged( chain );
			if ( nextChanged == logic )
				prevChanged->setNextChanged( nextChanged->nextChanged( chain ), chain );
		}
	}
}


void Simulator::detachCircuit( Circuit * circuit )
{
	if (!circuit)
		return;
	
	detach( & m_ordinaryCircuits, circuit );
	
	// Any changes to the code below will probably also apply to Simulator::removeLogicOutReferences
	
	if ( m_pChangedCircuitLast == circuit )
	{
		Circuit * previous_1 = 0l;
		Circuit * previous_2 = 0l;
		for ( Circuit * circuit = m_pChangedCircuitStart; circuit; )
		{
			if (previous_1)
				previous_2 = previous_1;
			previous_1 = circuit;
			circuit = circuit->nextChanged( m_currentChain );
		}
		
		m_pChangedCircuitLast = previous_2;
	}
	
	for ( unsigned chain = 0; chain < 2; ++chain )
	{
		for ( Circuit * prevChanged = m_pChangedCircuitStart; prevChanged; prevChanged = prevChanged->nextChanged( chain ) )
		{
			Circuit * nextChanged = prevChanged->nextChanged( chain );
			if ( nextChanged == circuit )
				prevChanged->setNextChanged( nextChanged->nextChanged( chain ), chain );
		}
	}
}
//END class Simulator

#include "simulator.moc"
