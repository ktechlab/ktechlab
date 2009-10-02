/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECBCDTO7SEGMENT_H
#define ECBCDTO7SEGMENT_H

#include "dipcomponent.h"
#include "logic.h"

/**
@short Converts a BCD input to 7-Segment Output
@author David Saxton
*/
class ECBCDTo7Segment : public CallbackClass, public DIPComponent
{
public:
	ECBCDTo7Segment( ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECBCDTo7Segment();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
private:
	void inStateChanged( bool newState );
	LogicIn *ALogic, *BLogic, *CLogic, *DLogic;
	LogicIn *ltLogic, *rbLogic, *enLogic;
	LogicOut *outLogic[7];
};

#endif
