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
#include "simulator.h"
#include "ktlconfig.h"

//BEGIN class LogicConfig
LogicConfig::LogicConfig() {

	risingTrigger = KtlConfig::self()->logicRisingTrigger();
	fallingTrigger = KtlConfig::self()->logicFallingTrigger();
	output = KtlConfig::self()->logicOutputHigh();
	highImpedance = KtlConfig::self()->logicOutputHighImpedance();
	lowImpedance = KtlConfig::self()->logicOutputLowImpedance();
}
//END class LogicConfig

//BEGIN class LogicIn
LogicIn::LogicIn(LogicConfig config)
		: Element::Element(), m_pCallbackFunction(0),
	m_config(config), m_bState(false) {
	m_numCNodes = 1;

	check();
}

LogicIn::~LogicIn() {
	Simulator::self()->removeLogicInReferences(this);
	m_pCallbackObject = 0;
	m_pCallbackFunction = 0;
}

void LogicIn::setCallback(CallbackClass *object, CallbackPtr func) {
	m_pCallbackFunction = func;
	m_pCallbackObject = object;
}

void LogicIn::check() {
	if (!b_status)
		return;

	bool newState;

	if (m_bState) {
		// Was high, will still be high unless voltage is less than falling trigger
		newState = p_cnode[0]->voltage() > m_config.fallingTrigger;
	} else {
		// Was low, will still be low unless voltage is more than rising trigger
		newState = p_cnode[0]->voltage() > m_config.risingTrigger;
	}

	if (m_pCallbackFunction && (newState != m_bState)) {
		m_bState = newState;
		(m_pCallbackObject->*m_pCallbackFunction)(newState);
	}

	m_bState = newState;
}

void LogicIn::setLogic(LogicConfig config) {
	m_config = config;
	check();
}

void LogicIn::setElementSet(ElementSet *c) {
	Element::setElementSet(c);
}

void LogicIn::callCallback() {
	if (m_pCallbackFunction)
		(m_pCallbackObject->*m_pCallbackFunction)(m_bState);
}
//END class LogicIn

//BEGIN class LogicOut
LogicOut::LogicOut(LogicConfig config, bool _high)
		: LogicIn(config) {

	m_numCBranches = 1;
//	m_old_r_out = 
	m_r_out = 0.0;
	m_old_x = m_v_out = 0.0;
	setHigh(_high);

	configChanged();
	isSetChanged = true;
}

LogicOut::~LogicOut() {
	Simulator *theSimulator = Simulator::self();
	theSimulator->removeLogicOutReferences(this);
}

void LogicOut::setElementSet(ElementSet *c) {

	if (c) isSetChanged = false;

//	m_old_r_out = 
	m_old_x = 0.0;
	LogicIn::setElementSet(c);
}

void LogicOut::setOutputHighConductance(double g) {
	double impedance = 1.0 / g;

	if (impedance == m_config.highImpedance) return;

	m_config.highImpedance = impedance;
	configChanged();
}

void LogicOut::setOutputLowConductance(double g) {
	double impedance = 1.0 / g;

	if (impedance == m_config.lowImpedance) return;

	m_config.lowImpedance = impedance;
	configChanged();
}

void LogicOut::setOutputHighVoltage(double v) {

	if (v == m_config.output) return;

	m_config.output = v;
	configChanged();
}

void LogicOut::setLogic(LogicConfig config) {
	m_config = config;

	configChanged();
}

void LogicOut::configChanged() {

	if (p_eSet)
		p_eSet->setCacheInvalidated();
	else return;

	switch (out_state) {
	case HIGH :
		m_r_out = m_config.highImpedance;
		m_v_out = m_config.output;
		setState(true);
		break;
	case LOW : 
		m_r_out	= m_config.lowImpedance;
		m_v_out	= 0.0;
		setState(false);
		break;
	case Z :
		m_r_out = 1e6;
		m_v_out = 2.5;
		break;
	};

	add_initial_dc();
}

void LogicOut::add_initial_dc() {
	if (!b_status)
		return;

//	double delta_resistance = m_r_out - m_old_r_out;

// FIXME FIXME BROKEN BROKEN FIXME FIXME!!!
//	double delta_x = 1 + (1 / (1 + m_r_out)) - m_old_x;

// ignores the series resistence of the source but otherwise correct. 
	double delta_x = 1 - m_old_x;
// ###

	A_b(0, 0) -= delta_x;
	A_c(0, 0) += delta_x;
//	A_d(0, 0) += delta_resistance;

	b_v(0) = m_v_out; // we own this variable so we simply write the new value.

//	m_old_r_out = m_r_out;
	m_old_x = 1;
}

void LogicOut::updateCurrents() {
	if (!p_eSet) {
        m_cnodeCurrent[0] = 0;
		return;
	}

	if (!b_status) return;

    m_cnodeCurrent[0] = (m_v_out - p_cnode[0]->voltage()) * m_r_out;
}

void LogicOut::setHigh(bool high) {
	if (high == (out_state == HIGH))
		return;

	out_state = high ? HIGH : LOW;

	if (!p_eSet) {
		setState(high);
		setChain();
		isSetChanged = true;
		return;
	}

	// NOTE Make sure that the next two lines are the same as those in setLogic
	m_r_out = high ? m_config.highImpedance : m_config.lowImpedance;
	m_v_out = high ? m_config.output : 0.0;

	add_initial_dc();

	p_eSet->circuit()->setChanged();
}

/*!
    \fn LogicOut::void setDependents(const LogicInList &logicInList)
 */
void LogicOut::setDependents(LogicInList &logicInList)
{
	dependents.erase(dependents.begin(), dependents.end());	

	std::list<LogicIn*>::iterator end = logicInList.end();
	for(std::list<LogicIn*>::iterator it = logicInList.begin(); it != end; ++it) {
		dependents.push_back(*it);
	}
}

/*!
    \fn LogicOut::callCallbacks()
 */
void LogicOut::callCallbacks()
{
	std::list<LogicIn*>::iterator end = dependents.end();
	for(std::list<LogicIn*>::iterator it = dependents.begin(); it != end; ++it) {
		(*it)->callCallback();
	}
}

/*!
    \fn LogicOut::setChain()
// ensure all dependants have the same state. 
 */
void LogicOut::setChain()
{
	std::list<LogicIn*>::iterator end = dependents.end();
// WARNING: it makes a huge performance difference if the ++ is before or after the iterator!! 
	for(std::list<LogicIn*>::iterator it = dependents.begin(); it != end; ++it) {
		(*it)->setState(out_state == HIGH);
	}
}

/*!
    \fn LogicOut::setZ()
Set output to high impedance mode. 
 */
void LogicOut::setZ()
{
	if(out_state == Z) return;

	out_state = Z;

	m_r_out = 1e6;
	m_v_out = 2.5;

	add_initial_dc();
}
//END class LogicOut
