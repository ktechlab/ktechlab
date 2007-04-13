/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef REPEAT_H
#define REPEAT_H

#include "flowcontainer.h"

/**
@author David Saxton
*/
class Repeat : public FlowContainer
{
public:
	Repeat( ICNDocument *icnDocument, bool newItem, const char *id );
	~Repeat();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	virtual void generateMicrobe( FlowCode * );
	
protected:
	void dataChanged();
};

#endif
