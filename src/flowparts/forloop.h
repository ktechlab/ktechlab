/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef FORLOOP_H
#define FORLOOP_H

#include "flowcontainer.h"

/**
@author David Saxton
*/
class ForLoop : public FlowContainer
{
public:
	ForLoop( ICNDocument *icnDocument, bool newItem, const char *id );
	~ForLoop() override;

	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

	void generateMicrobe( FlowCode * ) override;

protected:
	void dataChanged() override;
};

#endif
