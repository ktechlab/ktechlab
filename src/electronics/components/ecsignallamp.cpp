/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecsignallamp.h"
#include "element.h"

// TODO: resistance and power rating should be user definable properties.
#define RESISTANCE 100
#define WATTAGE    0.5
// minimal power to create glow. (looks low...)
#define LIGHTUP   (WATTAGE / 20)


ECSignalLamp::ECSignalLamp()
	:   Component()
{
	the_filament.setResistance(RESISTANCE);

	advanceSinceUpdate = 0;
	avgPower = 0.;
}

ECSignalLamp::~ECSignalLamp() {}

void ECSignalLamp::stepNonLogic()
{
    // FIXME implement
	const double voltage = 0; // m_pPNode[0]->pin().voltage()-m_pNNode[0]->pin().voltage();
	avgPower = fabs(avgPower * advanceSinceUpdate +
			(voltage * voltage / RESISTANCE)) /
			++advanceSinceUpdate;
}
