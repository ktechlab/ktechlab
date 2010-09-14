/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECFIXEDVOLTAGE_H
#define ECFIXEDVOLTAGE_H

#include "simplecomponent.h"

#include "voltagepoint.h"

/**
@short Fixed voltage source
@author David Saxton
*/
class ECFixedVoltage : public SimpleComponent
{
public:
	ECFixedVoltage(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECFixedVoltage();
	
	static Item* construct(ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();
	
private:
	virtual void drawShape(QPainter &p);
	void dataChanged();
	VoltagePoint m_voltagePoint;
};

#endif
