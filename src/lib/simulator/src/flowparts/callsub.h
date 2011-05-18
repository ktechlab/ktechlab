/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CALLSUB_H
#define CALLSUB_H

#include "flowpart.h"

/**
@short FlowPart that calls a subroutine
@author David Saxton
*/
class CallSub : public FlowPart
{
public:
	CallSub( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~CallSub();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	virtual void generateMicrobe( FlowCode *code );

private:
	void dataChanged();
};

#endif
