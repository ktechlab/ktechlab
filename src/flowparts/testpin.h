/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef TESTPIN_H
#define TESTPIN_H

#include "flowpart.h"

/**
@short FlowPart that tests a pin to see if it's high
@author David Saxton
*/
class TestPin : public FlowPart
{
public:
	TestPin( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~TestPin();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	virtual void generateMicrobe( FlowCode *code );

protected:
	void dataChanged();
};

#endif
