/***************************************************************************
 *   Copyright (C) 2006 by William Hillerby - william.hillerby@ntlworld.com*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VOLTAGEREGULATOR_H
#define VOLTAGEREGULATOR_H

#include <component.h>

/**
@author William Hillerby
 */
 
 /** @todo FOA Discrete, Liner, Non-Linear and arrange items in itemlib.h and libItem() */

/*
		
A voltage regulator is an integrated circuit whose function is to keep a voltage at a specific level. Some regulators are
adjustable, others are not. Voltage regulators come in both positive and negative voltages. They have three pins, the input, 
output and reference. The input is usually on the left side of the square and is where the unregulated input voltage is applied.
The reference is usually on the bottom of the square and is where the reference voltage is applied. For fixed regulators, this
reference is usually ground. For variable regulators, this reference is usually a small variable voltage just above ground. The
last connection is the output, and is usually located on the right side of the square. This is where the regulated output voltage
is taken from. On most schematics, the connections are numbered and labeled. 

*/
		
const double maxVoltageOut = 50.0;

class VoltageRegulator : public Component
{
	public:
		VoltageRegulator( ICNDocument* icnDocument, bool newItem, const QString& id = 0L );
		~VoltageRegulator() override;
		
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
				
	private:
		void dataChanged() override;
		void drawShape( QPainter &p ) override;
		
		// Input.
		// Output.
		// Reference = ground for fixed regulators.
			
};

#endif
