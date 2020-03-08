/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ROTATE_H
#define ROTATE_H

#include "flowpart.h"

/**
@short FlowPart that rotates a variable
@author David Saxton
*/
class Unary : public FlowPart
{
public:
	Unary( ICNDocument *icnDocument, bool newItem, const char *id = nullptr );
	~Unary() override;

	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

	void generateMicrobe( FlowCode *code ) override;

protected:
	void dataChanged() override;
};

#endif
