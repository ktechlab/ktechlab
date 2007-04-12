/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECRESISTOR_H
#define ECRESISTOR_H

#include "component.h"

/**
@short Simple resistor
@author David Saxton
*/
class ECResistor : public Component
{
	public:
		ECResistor( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
		~ECResistor();
	
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
	
	protected:
		virtual void dataChanged();
		virtual void drawShape( QPainter & p );
	
		Resistance * m_resistance;
};

#endif
