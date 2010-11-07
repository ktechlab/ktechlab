/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RESISTOR_H
#define RESISTOR_H

#include "component.h"
#include "resistance.h"

/**
@short Simple resistor
@author David Saxton
*/
class Resistor : public Component
{
	public:
		Resistor();
		~Resistor();


	protected:
        virtual void propertyChanged(Property& theProperty, QVariant newValue,
                                     QVariant oldValue);

		Resistance m_resistance;
};

#endif
