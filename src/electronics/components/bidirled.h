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
*/
class BiDirLED : public Component
{
	public:
		BiDirLED( ICNDocument * icnDocument, bool newItem, const char *id = nullptr );
		~BiDirLED() override;
	
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
	
		void dataChanged() override;
		void stepNonLogic() override;
		bool doesStepNonLogic() const override { return true; }
	
	private:
		void drawShape( QPainter &p ) override;
	
		double r[2];
		double g[2];
		double b[2];

		double avg_brightness[2];
		uint last_brightness[2];
		double lastUpdatePeriod;
		Diode *m_pDiode[2];
};

#endif
