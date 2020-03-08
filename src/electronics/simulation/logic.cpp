/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <vector>
#include "circuit.h"
#include "elementset.h"
#include "logic.h"
#include "matrix.h"
#include "simulator.h"

#include <ktlconfig.h>


//BEGIN class LogicConfig
LogicConfig::LogicConfig()
{
	risingTrigger = 0.0;
	fallingTrigger = 0.0;
	output = 0.0;
	highImpedance = 0.0;
	lowImpedance = 0.0;
}
//END class LogicConfig



//BEGIN class LogicIn
LogicIn::LogicIn( LogicConfig config )
	: Element::Element()
{
	m_config = config;
	m_pCallbackFunction = nullptr;
	m_numCNodes = 1;
	m_bLastState = false;
	m_pNextLogic = nullptr;
	setLogic(getConfig());
}

LogicIn::~LogicIn()
{
    if (Simulator::isDestroyedSim()) {
        return;
    }
	Simulator::self()->removeLogicInReferences(this);
}


void LogicIn::setCallback( CallbackClass * object, CallbackPtr func )
{
	m_pCallbackFunction = func;
	m_pCallbackObject = object;
}


void LogicIn::check()
{
	if (!b_status)
		return;
	
	bool newState;
	if (m_bLastState)
	{
		// Was high, will still be high unless voltage is less than falling trigger
		newState = p_cnode[0]->v > m_config.fallingTrigger;
	}
	else
	{
		// Was low, will still be low unless voltage is more than rising trigger
		newState = p_cnode[0]->v > m_config.risingTrigger;
	}
	
	if ( m_pCallbackFunction && (newState != m_bLastState) )
	{
		m_bLastState = newState;
		(m_pCallbackObject->*m_pCallbackFunction)(newState);
	}
	m_bLastState = newState;
}


void LogicIn::setLogic( LogicConfig config )
{
	m_config = config;
	check();
}


void LogicIn::setElementSet( ElementSet *c )
{
	if (c)
		m_pNextLogic = nullptr;
	else
		m_cnodeI[0] = 0.;
	
	Element::setElementSet(c);
}


void LogicIn::add_initial_dc()
{
}


void LogicIn::updateCurrents()
{
}


LogicConfig LogicIn::getConfig()
{
	LogicConfig c;
	c.risingTrigger = KTLConfig::logicRisingTrigger();
	c.fallingTrigger = KTLConfig::logicFallingTrigger();
	c.output = KTLConfig::logicOutputHigh();
	c.highImpedance = KTLConfig::logicOutputHighImpedance();
	c.lowImpedance = KTLConfig::logicOutputLowImpedance();
	return c;
}
//END class LogicIn



//BEGIN class LogicOut
LogicOut::LogicOut( LogicConfig config, bool _high )
	: LogicIn(config)
{
	m_bCanAddChanged = true;
	m_bOutputHighConductanceConst = false;
	m_bOutputLowConductanceConst = false;
	m_bOutputHighVoltageConst = false;
	m_pNextChanged[0] = m_pNextChanged[1] = nullptr;
	m_pSimulator = nullptr;
	m_bUseLogicChain = false;
	b_state = false;
	m_numCNodes = 1;
	m_vHigh = m_gHigh = m_gLow = 0.0;
	m_old_g_out = m_g_out = 0.0;
	m_old_v_out = m_v_out = 0.0;
	setHigh(_high);
	
	// Although we already call this function in LogicIn's constructor, our
	// virtual function will not have got called, so we have to call it again.
	setLogic(getConfig());
}

LogicOut::~LogicOut()
{
    if (Simulator::isDestroyedSim()) {
        return;
    }
	if (!m_pSimulator)
		m_pSimulator = Simulator::self();
	
	// Note that although this function will get called in the destructor of
	// LogicIn, we must call it here as well as it needs to be called before
	// removeLogicOutReferences(this) is called.
	m_pSimulator->removeLogicInReferences(this);
	
	m_pSimulator->removeLogicOutReferences(this);
}


void LogicOut::setUseLogicChain( bool use )
{
	if (!m_pSimulator)
		m_pSimulator = Simulator::self();
	
	m_bUseLogicChain = use;
	if (use)
		setElementSet(nullptr);
}


void LogicOut::setElementSet( ElementSet *c )
{
	if (!m_pSimulator)
		m_pSimulator = Simulator::self();
	
	if (c)
	{
		m_bUseLogicChain = false;
		m_pNextChanged[0] = m_pNextChanged[1] = nullptr;
	}
	
	// NOTE Make sure that the next two lines are the same as those in setHigh and setLogic
	m_g_out = b_state ? m_gHigh : m_gLow;
	m_v_out = b_state ? m_vHigh : 0.0;
	
	LogicIn::setElementSet(c);
}


void LogicOut::setOutputHighConductance( double g )
{
	m_bOutputHighConductanceConst = true;
	if ( g == m_gHigh )
		return;
	m_gHigh = g;
	configChanged();
}


void LogicOut::setOutputLowConductance( double g )
{
	m_bOutputLowConductanceConst = true;
	if ( g == m_gLow )
		return;
	m_gLow = g;
	configChanged();
}


void LogicOut::setOutputHighVoltage( double v )
{
	m_bOutputHighVoltageConst = true;
	if ( v == m_vHigh )
		return;
	m_vHigh = v;
	configChanged();
}


void LogicOut::setLogic( LogicConfig config )
{
	m_config = config;
	
	if (!m_bOutputHighConductanceConst)
		m_gHigh = 1.0/config.highImpedance;
	
	if (!m_bOutputLowConductanceConst)
		m_gLow = (config.lowImpedance == 0.0) ? 0.0 : 1.0/config.lowImpedance;
	
	if (!m_bOutputHighVoltageConst)
		m_vHigh = config.output;
	
	configChanged();
}


void LogicOut::configChanged()
{
	if (m_bUseLogicChain)
		return;
	
	if (p_eSet)
		p_eSet->setCacheInvalidated();
	
	// Re-add the DC stuff using the new values
	
	m_old_g_out = m_g_out;
	m_old_v_out = m_v_out;
	
	// NOTE Make sure that the next two lines are the same as those in setElementSet and setHigh
	m_g_out = b_state ? m_gHigh : m_gLow;
	m_v_out = b_state ? m_vHigh : 0.0;
	
	add_initial_dc();
	
	m_old_g_out = 0.;
	m_old_v_out = 0.;
	
	check();
}

void LogicOut::add_initial_dc()
{
	if (!b_status)
		return;
	
	A_g( 0, 0 ) += m_g_out-m_old_g_out;
	b_i( 0 ) += m_g_out*m_v_out-m_old_g_out*m_old_v_out;
}

void LogicOut::updateCurrents()
{
	if (m_bUseLogicChain)
	{
		m_cnodeI[0] = 0.0;
		return;
	}
	if (!b_status)
		return;
	
	m_cnodeI[0] = (m_v_out - p_cnode[0]->v) * m_g_out;
}

void LogicOut::setHigh( bool high )
{
	if ( high == b_state )
		return;
	
	if (m_bUseLogicChain)
	{
		b_state = high;
		
		for ( LogicIn * logic = this; logic; logic = logic->nextLogic() )
			logic->setLastState(high);
		
		if (m_bCanAddChanged)
		{
			m_pSimulator->addChangedLogic(this);
			m_bCanAddChanged = false;
		}
	
		return;
	}
	
	m_old_g_out = m_g_out;
	m_old_v_out = m_v_out;
	
	// NOTE Make sure that the next two lines are the same as those in setElementSet and setLogic
	m_g_out = high ? m_gHigh : m_gLow;
	m_v_out = high ? m_vHigh : 0.0;
	
	add_initial_dc();
	
	m_old_g_out = 0.;
	m_old_v_out = 0.;
	
	b_state = high;
	
	if ( p_eSet && p_eSet->circuit()->canAddChanged() )
	{
		m_pSimulator->addChangedCircuit( p_eSet->circuit() );
		p_eSet->circuit()->setCanAddChanged(false);
	}
}
//END class LogicOut

