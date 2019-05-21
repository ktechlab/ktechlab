/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECGROUND_H
#define ECGROUND_H

#include "component.h"

/**
@short Fixed voltage source
@author David Saxton
*/
class ECGround : public Component
{
public:
	ECGround( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~ECGround() override;
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
private:
	void drawShape( QPainter &p ) override;
};

#endif
