/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <cassert>

#include "ecbcdto7segment.h"

#include "logic.h"

// Values for a,b,c,d,e,f,g of common-anode 7 segment display
static bool numbers[16][7] = { 
	{ 1, 1, 1, 1, 1, 1, 0 }, // 0
	{ 0, 1, 1, 0, 0, 0, 0 }, // 1
	{ 1, 1, 0, 1, 1, 0, 1 }, // 2
	{ 1, 1, 1, 1, 0, 0, 1 }, // 3
	{ 0, 1, 1, 0, 0, 1, 1 }, // 4
	{ 1, 0, 1, 1, 0, 1, 1 }, // 5
	{ 1, 0, 1, 1, 1, 1, 1 }, // 6
	{ 1, 1, 1, 0, 0, 0, 0 }, // 7
	{ 1, 1, 1, 1, 1, 1, 1 }, // 8
	{ 1, 1, 1, 0, 0, 1, 1 }, // 9
	{ 1, 1, 1, 0, 1, 1, 1 }, // A
	{ 0, 0, 1, 1, 1, 1, 1 }, // b
	{ 1, 0, 0, 1, 1, 1, 0 }, // C
	{ 0, 1, 1, 1, 1, 0, 1 }, // d
	{ 1, 0, 0, 1, 1, 1, 1 }, // E
	{ 1, 0, 0, 0, 1, 1, 1 }}; // F

ECBCDTo7Segment::ECBCDTo7Segment()
		: Component() {

	ALogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));
	BLogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));
	CLogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));
	DLogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));
	ltLogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));
	rbLogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));
	enLogic.setCallback(this, (CallbackPtr)(&ECBCDTo7Segment::inStateChanged));

	for (uint i = 0; i < 7; ++i) {
		outLogic[i] = new LogicOut(LogicConfig(), false);
	}

	inStateChanged(false);
}

ECBCDTo7Segment::~ECBCDTo7Segment() {
    // TODO delete outLogic members
}

void ECBCDTo7Segment::inStateChanged(bool) {

	unsigned char n = ALogic.isHigh() | 
			 (BLogic.isHigh() << 1) |
			 (CLogic.isHigh() << 2) |
			 (DLogic.isHigh() << 3);

	if(!ltLogic.isHigh()) { 
		if(!rbLogic.isHigh()) {
			if(enLogic.isHigh()) { // Enable (store)
				for (int i = 0; i < 7; i++) {
					outLogic[i]->setHigh(numbers[n][i]);
				}
			}
		} else { // Ripple Blank
			for (int i = 0; i < 7; i++) 
				outLogic[i]->setHigh(false);
		}
	} else { // Lamp test
		for (int i = 0; i < 7; i++)
			outLogic[i]->setHigh(true);
	}
}
