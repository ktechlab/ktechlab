/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef INTERRUPT_H
#define INTERRUPT_H

#include "flowcontainer.h"

/**
@short FlowPart that defines the start of a interrupt
@author David Saxton
*/
class Interrupt : public FlowContainer
{
public:
	Interrupt( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~Interrupt();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	virtual void generateMicrobe( FlowCode *code );
	
protected:
	void dataChanged();
};

#endif
