/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECCURRENTSIGNAL_H
#define ECCURRENTSIGNAL_H

#include "component.h"
#include "currentsignal.h"

class Circuit;
class CurrentSignal;

/**
@short Provides a current signal (sinusoidal, square, etc)
@author David Saxton
*/
class ECCurrentSignal : public Component
{
public:
	ECCurrentSignal(Circuit &ownerCircuit);
	~ECCurrentSignal();
	
protected:
    virtual void propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue);
	
	CurrentSignal m_currentSignal;
};

#endif
