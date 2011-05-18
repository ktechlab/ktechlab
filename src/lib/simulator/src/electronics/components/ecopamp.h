/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECOPAMP_H
#define ECOPAMP_H

#include "component.h"
#include "opamp.h"

class OpAmp;
class Circuit;

/**
@short Operational Amplifier
@author David Saxton
*/
class ECOpAmp : public Component
{
public:
	ECOpAmp(Circuit &ownerCircuit);
	~ECOpAmp();

private:
	OpAmp the_amp;
};

#endif
