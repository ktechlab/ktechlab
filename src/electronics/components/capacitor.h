/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "component.h"

class Capacitance;
class ECNode;

/**
@short Capacitor
Simple capacitor
@author David Saxton
*/
class Capacitor : public Component
{
public:
	Capacitor( ICNDocument *icnDocument, bool newItem, const char *id = nullptr );
	~Capacitor() override;
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

private:
	void dataChanged() override;
	void drawShape( QPainter &p ) override;

	Capacitance * m_capacitance;
};

#endif
