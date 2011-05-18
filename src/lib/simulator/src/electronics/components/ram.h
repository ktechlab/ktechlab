/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RAM_H
#define RAM_H

#include "dipcomponent.h"
//#include "logic.h"

#include <qbitarray.h>
#include <vector>

class LogicIn;
class LogicOut;

/**
@author David Saxton
*/
class RAM : public CallbackClass, public DIPComponent {

public:
	RAM(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~RAM();

	static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();

protected:
	void initPins();
	virtual void dataChanged();
	void inStateChanged(bool newState);

	QBitArray m_data;
	LogicIn m_pCS; // Chip select
	LogicIn m_pOE; // Output enable
	LogicIn m_pWE; // Write enable

	int m_wordSize;
	int m_addressSize;

// Here's one of the places where the STL really bytes us. We want to store the objects directly in the container, but
// then we need to be able to store manipluate refferences directly to them elsewhere... C++ reorganizes our 
// storage at whim so we don't seem to have a choice here but to use pointer arrays and deal with the problems that causes. 
	std::vector<LogicIn *> m_address;
	std::vector<LogicIn *> m_dataIn;
	std::vector<LogicOut *> m_dataOut;
};
#endif

