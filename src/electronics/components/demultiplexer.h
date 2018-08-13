/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DEMULTIPLEXER_H
#define DEMULTIPLEXER_H

#include "component.h"
#include "logic.h"

#include <q3ptrvector.h>

/**
@author David Saxton
*/
class Demultiplexer : public CallbackClass,  public Component
{
public:
	Demultiplexer( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~Demultiplexer();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
protected:
	void dataChanged();
	/**
	 * Add / remove pins according to the number of inputs the user has requested
	 */
	void initPins( unsigned addressSize );
	
	void inStateChanged( bool newState );
	
	Q3PtrVector<LogicIn> m_aLogic;      // TODO QT3
	Q3PtrVector<LogicOut> m_xLogic;     // TODO QT3
	LogicIn * m_input;
};

#endif
