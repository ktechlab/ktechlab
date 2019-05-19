/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SEVENSEG_H
#define SEVENSEG_H

#include "flowpart.h"

/**
@short Allows a configurable output to a seven segment display
@author David Saxton
*/
class SevenSeg : public FlowPart
{
public:
	SevenSeg( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~SevenSeg();

	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

	virtual void generateMicrobe( FlowCode *code ) override;

protected:
	void dataChanged() override;
};

#endif
