/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECCURRENTSIGNAL_H
#define ECCURRENTSIGNAL_H

#include "simplecomponent.h"
#include "currentsignal.h"

class CurrentSignal;

/**
@short Provides a current signal (sinusoidal, square, etc)
@author David Saxton
*/
class ECCurrentSignal : public SimpleComponent
{
public:
	ECCurrentSignal( ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECCurrentSignal();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
protected:
	virtual void drawShape( QPainter &p );
	void dataChanged();
	
	CurrentSignal m_currentSignal;
};

#endif
