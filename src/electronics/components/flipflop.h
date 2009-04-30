/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef FLIPFLOP_H
#define FLIPFLOP_H

#include "component.h"
#include "logic.h"

class Simulator;

class ClockedLogic : public CallbackClass, public Component {
public:
	ClockedLogic(ICNDocument *icnDocument, bool newItem, const char *id);

protected:
	enum EdgeTrigger { Rising, Falling};
	virtual void dataChanged();
	virtual void initSymbolFromTrigger() = 0;
	EdgeTrigger m_edgeTrigger;

	LogicIn *m_pClock;

	bool m_bPrevClock;
};

/**
@short Boolean D-Type Flip-Flop
@author David Saxton
*/
class ECDFlipFlop : public ClockedLogic {

public:
	ECDFlipFlop(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECDFlipFlop();

	static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();

protected:
	virtual void drawShape(QPainter & p);
	virtual void initSymbolFromTrigger();
	void inputChanged(bool newState);
	void asyncChanged(bool newState);
	void clockChanged(bool newState);

	LogicIn  *m_pD;
	LogicOut *m_pQ;
	LogicOut *m_pQBar;
	LogicIn  *setp;
	LogicIn  *rstp;

	bool m_prevD;

	unsigned long long m_prevDChangeSimTime;
	Simulator *m_pSimulator;
};

/**
@short Boolean JK-Type Flip-Flop
@author Couriousous
*/
class ECJKFlipFlop : public ClockedLogic {

public:
	ECJKFlipFlop(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECJKFlipFlop();

	static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();

private:
	virtual void drawShape(QPainter &p);
	virtual void initSymbolFromTrigger();
	void asyncChanged(bool newState);
	void clockChanged(bool newState);

// TODO: think real hard about getting rid of this "hidden state" in favor of simply reading
// the user visible output pins.
	bool prev_state;

	LogicIn *m_pJ;
	LogicIn *m_pK;
	LogicIn *setp;
	LogicIn *rstp;
	LogicOut *m_pQ;
	LogicOut *m_pQBar;
};

/**
@short Boolean Set-Reset Flip-Flop
@author David Saxton
*/
class ECSRFlipFlop : public CallbackClass, public Component {
// TODO: let user choose between NOR and NAND type.

public:
	ECSRFlipFlop(ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECSRFlipFlop();

	static Item* construct(ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();

protected:
	void inStateChanged(bool newState);
	LogicIn *m_pS;
	LogicIn *m_pR;
	LogicOut *m_pQ;
	LogicOut *m_pQBar;
};

#endif
