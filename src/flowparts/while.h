/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef WHILE_H
#define WHILE_H

#include "flowcontainer.h"

/**
@author David Saxton
*/
class While : public FlowContainer
{
public:
	While( ICNDocument *icnDocument, bool newItem, const char *id );
	~While();

	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

	virtual void generateMicrobe( FlowCode *code ) override;

protected:
	void dataChanged() override;
};

#endif
