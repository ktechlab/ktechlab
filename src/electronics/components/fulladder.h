/***************************************************************************
 *   Copyright (C) 2004 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECFullAdder_H
#define ECFullAdder_H

#include "dipcomponent.h"
#include "logic.h"

/**
@author David Saxton
*/
class FullAdder : public CallbackClass, public DIPComponent
{
public:
	FullAdder( ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~FullAdder();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
protected:
	void inStateChanged( bool newState );
	
	LogicIn ALogic, BLogic, inLogic;
	LogicOut outLogic, SLogic;
};

#endif
