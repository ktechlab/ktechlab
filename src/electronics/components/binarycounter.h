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

#include "dipcomponent.h"
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

class BinaryCounter : public CallbackClass, public DIPComponent {

public:
	BinaryCounter(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~BinaryCounter();

	static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();

protected:
	void inStateChanged(bool state);   // Input
	void rStateChanged(bool state);   // Reset
//	void enStateChanged(bool state);   // Enable
//	void udStateChanged(bool state);   // Up/Down
	void outputValue();
	void dataChanged();
	void initPins(unsigned numBits);

	LogicIn *enLogic, *inLogic, *rLogic, *udLogic;
	LogicOut *m_pLogicOut[26];

	unsigned m_numBits;
	bool b_triggerHigh;
//	bool b_en; // Enable
//	bool b_ud; // Up/Down
	bool b_oldIn;
//	bool b_reset;
	long m_value;
	bool m_bDoneLogicIn;
};

#endif
