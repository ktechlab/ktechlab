/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECFIXEDVOLTAGE_H
#define ECFIXEDVOLTAGE_H

#include "component.h"

class VoltagePoint;

/**
@short Fixed voltage source
@author David Saxton
*/
class ECFixedVoltage : public Component
{
public:
	ECFixedVoltage();
	~ECFixedVoltage();
	
	
protected:
    virtual void propertyChanged(Property& theProperty, QVariant newValue,
                                 QVariant oldValue);

	VoltagePoint *m_voltagePoint;
    ElementMap *m_map;
};

#endif
