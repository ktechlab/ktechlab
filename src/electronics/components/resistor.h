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

/**
@short Simple resistor
@author David Saxton
*/
class Resistor : public Component
{
	public:
		Resistor( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		~Resistor();

		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();

	protected:
		virtual void dataChanged() override;
		virtual void drawShape( QPainter & p ) override;

		Resistance * m_resistance;
};

#endif
