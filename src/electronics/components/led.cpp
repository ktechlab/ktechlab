/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "diode.h"
#include "led.h"
#include "simulator.h"


LED::LED(Circuit& ownerCircuit) : ECDiode(ownerCircuit) {
	avg_brightness = 255;
	last_brightness = 255;
	lastUpdatePeriod = 0.;
}

LED::~LED() {
}

void LED::stepNonLogic() {
	avg_brightness += brightnessFromCurrent(m_diode.current());
	lastUpdatePeriod++;
}

uint LED::brightnessFromCurrent(double i) {
	if (i > 0.018) return 0;

	if (i < 0.002) return 255;

	return (uint)(255 * (1 - ((i - 0.002) / 0.016)));
}

uint LED::currentBrighness()
{
    if (lastUpdatePeriod != 0.)
        last_brightness = (uint)(avg_brightness / lastUpdatePeriod);

    uint ret = last_brightness;

    avg_brightness   = 0.;
    lastUpdatePeriod = 0.;

    return ret;
}
