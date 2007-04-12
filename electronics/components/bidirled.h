/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef BIDIRLED_H
#define BIDIRLED_H

#include <component.h>

/**
@author David Saxton
 NOTE: I'm not sure what the "BiDir" part means, but this class should probably be
 associated with a database containing the parameters such as current, voltage, resistance, spectrum, etc for common LED types. 
*/


class BiDirLED : public Component
{
	public:
		BiDirLED( ICNDocument * icnDocument, bool newItem, const char *id = 0 );
		~BiDirLED();
	
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
	
		virtual void dataChanged();
		virtual void stepNonLogic();
		virtual bool doesStepNonLogic() const { return true; }
	
	private:
		virtual void drawShape( QPainter &p );

// The standard precision for most graphics libraries 32 bit. 
		float r[2];
		float g[2];
		float b[2];
	
		double avg_brightness[2];
		uint last_brightness[2];
		double lastUpdatePeriod;
		Diode *m_pDiode[2];
};

#endif
