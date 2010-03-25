/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef END_H
#define END_H

#include "flowpart.h"

/**
@short FlowPart that tells the program where to end
@author David Saxton
*/
class End : public FlowPart
{
public:
	End( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~End();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	virtual void generateMicrobe( FlowCode * );
};

#endif
