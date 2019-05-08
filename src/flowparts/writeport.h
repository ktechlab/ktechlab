/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef WRITEPORT_H
#define WRITEPORT_H

#include "flowpart.h"

/**
@short FlowPart that writes to a port
@author David Saxton
*/
class WritePort : public FlowPart
{
public:
	WritePort( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~WritePort();

	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

	virtual void generateMicrobe( FlowCode *code ) override;

private:
	void dataChanged() override;
};

#endif
