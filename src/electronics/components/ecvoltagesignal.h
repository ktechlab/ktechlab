/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECVOLTAGESIGNAL_H
#define ECVOLTAGESIGNAL_H

#include "simplecomponent.h"

class VoltageSignal;

/**
@short Provides an alternating voltage source
@author David Saxton
*/
class ECVoltageSignal : public SimpleComponent
{
public:
	ECVoltageSignal(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECVoltageSignal();
	
	static Item* construct(ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();
	
protected:
	virtual void drawShape(QPainter &p);
	void dataChanged();
	
	VoltageSignal *m_voltageSignal;
};

#endif
