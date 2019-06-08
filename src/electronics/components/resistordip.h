/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RESISTORDIP_H
#define RESISTORDIP_H

#include "component.h"

const int maxCount = 256;

/**
@author David Saxton
*/
class ResistorDIP : public Component
{
public:
	ResistorDIP( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~ResistorDIP() override;

	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

protected:
	void drawShape( QPainter &p ) override;
	void updateDIPNodePositions();
	void dataChanged() override;
	/**
	 * Add / remove pins according to the number of inputs the user has requested
	 */
	void initPins();

	int m_resistorCount;
	Resistance* m_resistance[maxCount];
};

#endif
