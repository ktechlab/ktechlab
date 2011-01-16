/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECSIGNALLAMP_H
#define ECSIGNALLAMP_H

#include "component.h"
#include "resistance.h"

class Circuit;

/**
@short Signal Lamp - glows when current flows
@author David Saxton
*/
class ECSignalLamp : public Component
{
public:
	ECSignalLamp(Circuit &ownerCircuit);
	~ECSignalLamp();

	
	virtual void stepNonLogic();
	virtual bool doesStepNonLogic() const { return true; }
	
private:

	Resistance the_filament;
	double avgPower;
	uint advanceSinceUpdate;
};

#endif
