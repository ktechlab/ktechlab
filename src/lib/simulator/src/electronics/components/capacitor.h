/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "component.h"
#include "simulatorexport.h"

class Capacitance;
class Circuit;
class ECNode;

/**
@short Capacitor
Simple capacitor
@author David Saxton
*/
class SIMULATOR_EXPORT Capacitor : public Component
{
public:
	Capacitor(Circuit &ownerCircuit);
	~Capacitor();

protected:
    virtual void propertyChanged(Property& theProperty,
                                 QVariant newValue, QVariant oldValue );

	Capacitance *m_capacitance;
};

#endif
