/***************************************************************************
 *   Copyright (C) 2003,2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECDIODE_H
#define ECDIODE_H

#include "component.h"
#include "simulatorexport.h"

class Circuit;
class Diode;

/**
@short Simple diode
@author David Saxton
*/
class SIMULATOR_EXPORT ECDiode : public Component
{
public:
	ECDiode(Circuit &ownerCircuit);
	~ECDiode();

protected:
    virtual void propertyChanged(Property& theProperty, QVariant newValue,
                                 QVariant oldValue);

	Diode *m_diode;
};

#endif
