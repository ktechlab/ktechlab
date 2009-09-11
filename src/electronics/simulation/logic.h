/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef LOGIC_H
#define LOGIC_H

#include <set>

#include "element.h"

class Pin;
class Simulator;

typedef std::set<Pin *> PinSet;

class LogicConfig {

public:
	LogicConfig();

	double risingTrigger;	///< Trigger on rising edge
	double fallingTrigger;	///< Trigger on falling edge
	double output;		///< Output voltage
	double highImpedance;	///< Output impedance when high
	double lowImpedance;	///< Output impedance when low
};

class CallbackClass {};

typedef void(CallbackClass::*CallbackPtr)(bool isHigh);

/**
Use this class for Logic Inputs - this will have infinite impedance.
Use isHigh() will return whether the voltage level at the pin
is high than the predetermined voltage threshold, and setHigh() will make the
output high/low, also according to the predetermined logic type / voltages.

@short Boolean Logic input
*/

class LogicIn : public Element {

public:
	LogicIn(LogicConfig config);
	virtual ~LogicIn();

	virtual Type type() const {
		return Element_LogicIn;
	}

	virtual void setElementSet(ElementSet *c);

	/**
	 * Set logic values from the LogicConfig.
	 */
	virtual void setLogic(LogicConfig config);
	/**
	 * Check if the input state has changed, to see if we need to callback.
	 */
	void check();
	/**
	 * Returns whether the pin is 'high', as defined for the logic type
	 * Note: this is defined as the voltage on the pin, as opposed to what the
	 * state was set to (the two are not necessarily the same).
	 */
	inline bool isHigh() const {
		return m_bState;
	}

	/**
	 * When the logic state on this LogicIn changes, the function passed in this
	 * function will be called. At most one Callback can be added per LogicIn.
	 */
	void setCallback(CallbackClass * object, CallbackPtr func);
	/**
	 * Reads the LogicConfig values in from KTLConfig, and returns them in a
	 * nice object form.
	 */
	static LogicConfig getConfig();
	/**
	 * If this belongs to a logic chain, then this will be called from the chain.
	 */
	void setLastState(bool state) {
		m_bState = state;
	}

// ### crappy linked list! =(
/**
 * Returns a pointer to the next LogicIn in the chain. */
LogicIn *nextLogic() const { return m_pNextLogic; }
void setNextLogic(LogicIn *next) { m_pNextLogic = next; }
// ### 

	/**
	 * Calls the callback function, if there is one.
	 */
	void callCallback() {
		if (m_pCallbackFunction)
			(m_pCallbackObject->*m_pCallbackFunction)(m_bState);
	}

protected:

// evil linked list stuff: 
void setChain(bool high);
// ### 

	virtual void updateCurrents();
	virtual void add_initial_dc();

	// TODO: fix this crap NO FUNCTION POINTERS
	CallbackPtr m_pCallbackFunction;
	CallbackClass *m_pCallbackObject;
	bool m_bState;
	LogicConfig m_config;

private: 
/// FIXME: crappy linked list implementation. 
LogicIn *m_pNextLogic;
/// ###
};

/**
@short Logic output/input
*/
class LogicOut : public LogicIn {

public:
	LogicOut(LogicConfig config, bool _high);
	virtual ~LogicOut();

	virtual void setLogic(LogicConfig config);
	virtual void setElementSet(ElementSet *c);
	virtual Type type() const {
		return Element_LogicOut;
	}

	/**
	 * Call this function to override the logic-high output impedance as set by
	 * the user. Once set, the impedance will not be changed by the user
	 * updating the config; only by subsequent calls to this function.
	 */
	void setOutputHighConductance(double g);
	/**
	 * Call this function to override the logic-low output impedance as set by
	 * the user. Once set, the impedance will not be changed by the user
	 * updating the config; only by subsequent calls to this function.
	 */
	void setOutputLowConductance(double g);
	/**
	 * Call this function to override the logic out voltage as set by the
	 * user. Once set, the impedance will not be changed by the user
	 * updating the config; only by subsequent calls to this function.
	 */
	void setOutputHighVoltage(double v);
	/**
	 * Returns the voltage that this will output when high.
	 */
	double outputHighVoltage() const {
		return m_config.output;
	}

	/**
	 * Sets the pin to be high/low
	 */
	void setHigh(bool high);

/// SHODDY LINKED LIST STUFF!!! 
	/**
	* We have two modes, a fast mode based on internal logic chains and a slow mode based on the 
	* analog simulator. Calling this function puts us in fast mode, use only if there are no analog considerations. 
	 */
	void setUseLogicChain();

void setNextChanged(LogicOut *logicOut, unsigned char chain) {
	m_pNextChanged[chain] = logicOut;
}

LogicOut *nextChanged(unsigned char chain) const {
	return m_pNextChanged[chain];
}

void setCanAddChanged(bool canAdd) {
// avoid being added twice. 
	m_bCanAddChanged = canAdd;
}
// **** 

// FIXME RED ALERT: THESE ARE ONLY ACCESSED BY SIMULATOR!!!
	PinSet pinList;

protected:

	void configChanged();
	virtual void updateCurrents();
	virtual void add_initial_dc();

	double m_r_out, m_v_out;
	double m_old_r_out, m_old_x;

// ###  We also moonlight as a shoddy linked list implementation; woo hoo!! 
	bool m_bCanAddChanged;
	LogicOut *m_pNextChanged[2];
	bool m_bUseLogicChain;
// ###
//private: 
//	bool m_bOutState;  // state which is insensitive to circuit conditions. 
};

#endif

