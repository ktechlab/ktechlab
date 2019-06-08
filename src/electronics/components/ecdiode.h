/***************************************************************************
 *   Copyright (C) 2003,2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECDIODE_H
#define ECDIODE_H

#include "component.h"

/**
@short Simple diode
@author David Saxton
*/
class ECDiode : public Component
{
public:
	ECDiode( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~ECDiode() override;
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

protected:
	void drawShape( QPainter & p ) override;
	void dataChanged() override;
	Diode *m_diode;
};

#endif
