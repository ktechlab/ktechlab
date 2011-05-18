/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef READPORT_H
#define READPORT_H

#include "flowpart.h"

/**
@short FlowPart that reads from a port
@author David Saxton
*/
class ReadPort : public FlowPart
{
public:
	ReadPort( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~ReadPort();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	virtual void generateMicrobe( FlowCode * );

protected:
	void dataChanged();
};

#endif
