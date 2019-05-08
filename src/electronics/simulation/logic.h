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

#include "element.h"

#include <qpointer.h>
#include <qlist.h>

class Component;
class Pin;
class Simulator;

typedef QList<QPointer<Pin> > PinList;

class LogicConfig
{
	public:
		LogicConfig();

		float risingTrigger;	///< Trigger on rising edge
		float fallingTrigger;	///< Trigger on falling edge
		float output;			///< Output voltage
		float highImpedance;	///< Output impedance when high
		float lowImpedance;		///< Output impedance when low
};


class CallbackClass {};
typedef void(CallbackClass::*CallbackPtr)( bool isHigh );

/**
Use this class for Logic Inputs - this will have infinite impedance.
Use isHigh() will return whether the voltage level at the pin
is high than the predetermined voltage threshold, and setHigh() will make the
output high/low, also according to the predetermined logic type / voltages.

@short Boolean Logic input
*/
class LogicIn : public Element
{
	public:
		LogicIn( LogicConfig config );
		virtual ~LogicIn();

		virtual Type type() const override { return Element_LogicIn; }
		virtual void setElementSet( ElementSet *c ) override;

		/**
		 * Set logic values from the LogicConfig.
		 */
		virtual void setLogic( LogicConfig config );
		/**
		 * Check if the input state has changed, to see if we need to callback.
		 */
		void check();
		/**
		 * Returns whether the pin is 'high', as defined for the logic type
		 * Note: this is defined as the voltage on the pin, as opposed to what the
		 * state was set to (the two are not necessarily the same).
		 */
		bool isHigh() const { return m_bLastState; }
		/**
		 * When the logic state on this LogicIn changes, the function passed in this
		 * function will be called. At most one Callback can be added per LogicIn.
		 */
		void setCallback( CallbackClass * object, CallbackPtr func );
		/**
		 * Reads the LogicConfig values in from KTLConfig, and returns them in a
		 * nice object form.
		 */
		static LogicConfig getConfig();
		/**
		 * If this belongs to a logic chain, then this will be called from the chain.
		 */
		void setLastState( bool state ) { m_bLastState = state; }
		/**
		 * Returns a pointer to the next LogicIn in the chain.
		 */
		LogicIn * nextLogic() const { return m_pNextLogic; }
		/**
		 * Sets the next LogicIn in the chain.
		 */
		void setNextLogic( LogicIn * next ) { m_pNextLogic = next; }
		/**
		 * Calls the callback function, if there is one.
		 */
		void callCallback()
		{
			if (m_pCallbackFunction)
				(m_pCallbackObject->*m_pCallbackFunction)(m_bLastState);
		}

	protected:
		virtual void updateCurrents() override;
		virtual void add_initial_dc() override;

		// TODO: fix this crap NO FUNCTION POINTERS
		CallbackPtr m_pCallbackFunction;
		CallbackClass * m_pCallbackObject;
		bool m_bLastState;
		LogicIn * m_pNextLogic;
		LogicConfig m_config;
};


/**
@short Logic output/input
*/
class LogicOut : public LogicIn
{
	public:
		LogicOut( LogicConfig config, bool _high );
		virtual ~LogicOut();

		virtual void setLogic( LogicConfig config ) override;
		virtual void setElementSet( ElementSet *c ) override;
		virtual Type type() const override { return Element_LogicOut; }

		/**
		 * Call this function to override the logic-high output impedance as set by
		 * the user. Once set, the impedance will not be changed by the user
		 * updating the config; only by subsequent calls to this function.
		 */
		void setOutputHighConductance( double g );
		/**
		 * Call this function to override the logic-low output impedance as set by
		 * the user. Once set, the impedance will not be changed by the user
		 * updating the config; only by subsequent calls to this function.
		 */
		void setOutputLowConductance( double g );
		/**
		 * Call this function to override the logic out voltage as set by the
		 * user. Once set, the impedance will not be changed by the user
		 * updating the config; only by subsequent calls to this function.
		 */
		void setOutputHighVoltage( double v );
		/**
		 * Returns the voltage that this will output when high.
		 */
		double outputHighVoltage() const { return m_vHigh; }
		/**
		 * Sets the pin to be high/low
		 */
		void setHigh( bool high );
		/**
		 * @returns the state that this is outputting (regardless of voltage level on logic)
		 */
		bool outputState() const { return b_state; }
		/**
		 * Set whether or not this LogicOut is the head of a LogicChain (controls
		 * itself and a bunch of LogicIns).
		 */
		void setUseLogicChain( bool use );
		/**
		 * When a LogicOut configured as the start of a LogicChain changes start, it
		 * appends a pointer to itself to the list of change LogicOut, starting from
		 * the Simulator. This functions enables appending the next changed LogicOut
		 * to this one.
		 */
		void setNextChanged( LogicOut * logicOut, unsigned char chain ) { m_pNextChanged[chain] = logicOut; }
		/**
		 * To avoid a pointer to this LogicOut being added twice in one
		 * iteration due to the state changing twice, this LogicOut sets an
		 * added flag to true after adding it to the list of changed. The flag
		 * must be reset to false with this function (done by Simulator).
		 */
		void setCanAddChanged( bool canAdd ) { m_bCanAddChanged = canAdd; }
		/**
		 * Returns the next LogicOut that has changed, when configured as the start
		 * of a LogicChain.
		 * @see setNextChanged
		 */
		LogicOut * nextChanged( unsigned char chain ) const { return m_pNextChanged[chain]; }
		PinList pinList;
		PinList::iterator pinListBegin;
		PinList::iterator pinListEnd;

	protected:
		void configChanged();
		virtual void updateCurrents() override;
		virtual void add_initial_dc() override;

		// Pre-initalized levels from config
		double m_gHigh;
		double m_gLow;
		double m_vHigh;

		// Whether to use the user-defined logic values
		bool m_bOutputHighConductanceConst;
		bool m_bOutputLowConductanceConst;
		bool m_bOutputHighVoltageConst;

		double m_g_out;
		double m_v_out;
		double m_old_g_out;
		double m_old_v_out;
		bool b_state;
		bool m_bCanAddChanged;
		LogicOut * m_pNextChanged[2];
		Simulator * m_pSimulator;
		bool m_bUseLogicChain;
};

#endif

