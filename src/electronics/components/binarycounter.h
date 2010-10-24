/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef EC4BITCOUNTER_H
#define EC4BITCOUNTER_H

#include "component.h"
#include "logic.h"

/**
Simple logic counter. 4 Inputs, 4 Outputs.

Outputs (A-D) represent the stored value (0-15).
The inputs are:
@li en - Enable incrementing of value
@li in - Input (trigger high)
@li r - Reset stored value to 0
@li ud - Up/Down increment

@short 4 Bit Binary Counter
@author David Saxton

TODO: refactor and make a subclass of clockedFlipFLop (or clocked Logic, as proposed.
*/
class BinaryCounter : public CallbackClass, public Component {

public:
	BinaryCounter();
	~BinaryCounter();

    /**
     * \return the number of bits of the counter
     */
    int bitNumber() const;

    /**
     * Set the number of bits that this counter has
     */
    void setBitNumber(int numBits);

    /**
     * \return if the triggering of this counter is done on the rising edge
     *      of the clock
     */
    bool triggherIsOnRisingEdge() const ;

    /**
     * Set if the triggering should happen on rising or falling edge
     */
    void setTriggerOnRisingEdge(bool isOnRisingEdge);

protected:
	void inStateChanged(bool state);   // Input
	void rStateChanged(bool state);   // Reset
	void outputValue();
	// void dataChanged();
	void initPins(unsigned numBits);

	LogicIn enLogic, inLogic, rLogic, udLogic;
	LogicOut *m_pLogicOut[26];

	unsigned m_numBits;
	bool m_bTriggerHigh;
	bool b_oldIn;  // <<< maybe we should inherit "clocked logic" or something... 
	unsigned m_value;
	bool m_bDoneLogicIn;
};

#endif
