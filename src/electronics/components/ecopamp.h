/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECOPAMP_H
#define ECOPAMP_H

#include "component.h"

/**
@short Operational Amplifier
@author David Saxton
*/
class ECOpAmp : public Component
{
	public:
		ECOpAmp( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		~ECOpAmp() override;
		
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();

	protected:
		void drawShape( QPainter & p ) override;
};

#endif
