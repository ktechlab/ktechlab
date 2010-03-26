/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECCLOCKINPUT_H
#define ECCLOCKINPUT_H

#include <list>

using namespace std;

#include "simplecomponent.h"
#include "logic.h"

class ComponentCallback;
class Simulator;

/**
@short Boolean clock input
@author David Saxton
*/
class ECClockInput : public SimpleComponent
{
public:
	ECClockInput(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECClockInput();
	
	static Item *construct( ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();
	
	void stepCallback();
	void stepLogic();

/// set the next callback based on the time remaining in the curren phase. 
	virtual void stepNonLogic();
	virtual bool doesStepNonLogic() const { return true; }
	
protected:
	virtual void drawShape(QPainter &p);
	void dataChanged();

	uint m_high_time;
	uint m_low_time;

	long long m_lastSetTime;
	LogicOut m_pOut;
	Simulator *m_pSimulator;
	list<ComponentCallback> *m_pComponentCallback[1000];
};

#endif
