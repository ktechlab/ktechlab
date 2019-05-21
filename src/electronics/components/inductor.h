/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef INDUCTOR_H
#define INDUCTOR_H

#include <component.h>

/**
@author David Saxton
*/
class Inductor : public Component
{
	public:
		Inductor( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		~Inductor() override;
	
		static Item * construct( ItemDocument * itemDocument, bool newItem, const char * id );
		static LibraryItem * libraryItem();
	
	private:
		void dataChanged() override;
		void drawShape( QPainter & p ) override;

		Inductance * m_pInductance;
};

#endif
