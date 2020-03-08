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

class ClockedFlipFlop : public CallbackClass, public Component
{
	public:
		ClockedFlipFlop( ICNDocument *icnDocument, bool newItem, const char * id );

	protected:
		enum EdgeTrigger { Rising, Falling };
		void dataChanged() override;
		virtual void initSymbolFromTrigger() = 0;
		EdgeTrigger m_edgeTrigger;
};


/**
@short Boolean D-Type Flip-Flop
@author David Saxton
*/
class ECDFlipFlop : public ClockedFlipFlop
{
public:
	ECDFlipFlop( ICNDocument *icnDocument, bool newItem, const char *id = nullptr );
	~ECDFlipFlop() override;

    static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

protected:
	void drawShape( QPainter & p ) override;
	void initSymbolFromTrigger() override;
	void inputChanged( bool newState );
	void inStateChanged( bool newState );
	void asyncChanged(bool newState );
	void clockChanged(bool newState );

	LogicIn *m_pD;
	LogicIn *m_pClock;
	LogicOut *m_pQ;
	LogicOut *m_pQBar;
	LogicIn *setp;
	LogicIn *rstp;
	bool m_bPrevClock;

	bool m_prevD;
	unsigned long long m_prevDChangeSimTime;
	Simulator * m_pSimulator;
};


/**
@short Boolean JK-Type Flip-Flop
@author Couriousous
*/
class ECJKFlipFlop : public ClockedFlipFlop
{
public:
	ECJKFlipFlop( ICNDocument *icnDocument, bool newItem, const char *id = nullptr );
	~ECJKFlipFlop() override;
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

private:
	void drawShape( QPainter & p ) override;
	void initSymbolFromTrigger() override;
	void inStateChanged( bool newState );
	void asyncChanged(bool newState );
	void clockChanged(bool newState );

	bool prev_state;
	bool m_bPrevClock;
	LogicIn *m_pJ;
	LogicIn *m_pClock;
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
class ECSRFlipFlop : public CallbackClass, public Component
{
public:
	ECSRFlipFlop( ICNDocument *icnDocument, bool newItem, const char *id = nullptr );
	~ECSRFlipFlop() override;

	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();

protected:
	void inStateChanged( bool newState );
	LogicIn * m_pS;
	LogicIn * m_pR;
	LogicOut * m_pQ;
	LogicOut * m_pQBar;
	bool old_q1;
	bool old_q2;
};

#endif
