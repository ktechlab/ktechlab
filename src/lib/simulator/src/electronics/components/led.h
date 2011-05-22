/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef LED_H
#define LED_H

#include "component.h"
#include "ecdiode.h"
#include "simulatorexport.h"

class Circuit;

/**
@short Simulates a LED
@author David Saxton
*/

class SIMULATOR_EXPORT LED : public ECDiode {

public:
	LED(Circuit &ownerCircuit);
	~LED();

	/**
	 * Returns the brightness for the given current, from 255 (off) -> 0 (on)
	 */
	static uint brightnessFromCurrent(double i);

    uint currentBrighness();

	virtual void stepNonLogic();
	virtual bool doesStepNonLogic() const {
		return true;
	}

private:

	uint avg_brightness;
	uint last_brightness;
	uint lastUpdatePeriod;
};

#endif
