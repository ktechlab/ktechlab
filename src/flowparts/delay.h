/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DELAY_H
#define DELAY_H

#include "flowpart.h"

/**
@short FlowPart that provides a delay
@author David Saxton
*/
class Delay : public FlowPart
{
public:
	Delay( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~Delay() override;

	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

	void generateMicrobe( FlowCode *code ) override;

protected:
	void dataChanged() override;
};

#endif
