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

#include "component.h"

class ComponentCallback;
class Simulator;

template <typename T>
class LinkedList;

/**
@short Boolean clock input
@author David Saxton
*/
class ECClockInput : public Component
{
public:
	ECClockInput( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~ECClockInput();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	void stepCallback();
	void stepLogic();
	virtual void stepNonLogic();
	virtual bool doesStepNonLogic() const { return true; }
	
protected:
	virtual void drawShape( QPainter &p );
	void dataChanged();
	
	uint m_time;
	uint m_high_time;
	uint m_low_time;
	uint m_period;
	long long m_lastSetTime;
	LogicOut * m_pOut;
	bool m_bSetStepCallbacks;
	bool m_bLastStepCallbackOut;
	Simulator * m_pSimulator;
	std::list<ComponentCallback> * m_pComponentCallback[1000];
};

#endif
