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
	~ECClockInput() override;
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
    /** callback for logic steps, for each logic update step;
     only active when the period of the clock is less or equal than LOGIC_UPDATE_PER_STEP */
	void stepCallback();
    /** callback for linear steps, for each linear update step;
     only active when the period of the clock is greater than LOGIC_UPDATE_PER_STEP */
	void stepLogic();
    /** callback at linear steps; always active */
	void stepNonLogic() override;
	bool doesStepNonLogic() const override { return true; }
	
protected:
	void drawShape( QPainter &p ) override;
	void dataChanged() override;
	
	uint m_time;
    /** unit: simulator logic update tick == 1s / LOGIC_UPDATE_RATE */
	uint m_high_time;
    /** unit: simulator logic update tick == 1s / LOGIC_UPDATE_RATE */
	uint m_low_time;
    /** unit: simulator logic update tick == 1s / LOGIC_UPDATE_RATE */
	uint m_period;
    /** unit: simulator logic update tick == 1s / LOGIC_UPDATE_RATE */
	long long m_lastSetTime;
	LogicOut * m_pOut;
	bool m_bSetStepCallbacks;
	bool m_bLastStepCallbackOut;
	Simulator * m_pSimulator;
	std::list<ComponentCallback> * m_pComponentCallback[ 100 /* == LOGIC_UPDATE_PER_STEP */ ];
};

#endif
