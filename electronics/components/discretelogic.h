/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DISCRETELOGIC_H
#define DISCRETELOGIC_H

#include "component.h"
#include "logic.h"

class Simulator;

/*
NOTE: There is probably a better way to do this. I would suggest creating a single
generic discrete logic class with a database containing the shape, rules for input/output pins, (eg how many there can be, whether the number is variable.), and the boolean expression the gate implements. It may also be useful to allow the user to invert selected inputs to the gate. (denoted by a small circle)
*/

/**
@short Boolean NOT
@author David Saxton
*/
class Inverter : public CallbackClass, public Component
{
	public:
		Inverter( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
		~Inverter();
	
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
	
	protected:
		void inStateChanged( bool newState );
		virtual void drawShape( QPainter &p );
		
		LogicIn * m_pIn;
		LogicOut * m_pOut;
};

/**
@short Buffer
@author David Saxton
*/
class Buffer : public CallbackClass, public Component
{
public:
	Buffer( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
	~Buffer();

	static Item* construct(ItemDocument *itemDocument, bool newItem, const char *id);
	static LibraryItem *libraryItem();

private:
	void inStateChanged( bool newState );
	virtual void drawShape( QPainter &p );
	
	LogicIn * m_pIn;
	LogicOut * m_pOut;
};

/**
@short Boolean logic input
@author David Saxton
*/
class ECLogicInput : public Component
{
public:
	ECLogicInput( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
	~ECLogicInput();

	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	virtual void buttonStateChanged( const QString &id, bool state );

private:
	virtual void dataChanged();
	virtual void drawShape( QPainter &p );
	LogicOut * m_pOut;
	bool b_state;
};

/**
@short Boolean logic output
@author David Saxton
*/
class ECLogicOutput : public CallbackClass, public Component
{
	public:
		ECLogicOutput( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
		~ECLogicOutput();

		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();

	protected:
		void inStateChanged( bool newState );
		virtual void drawShape( QPainter &p );

		unsigned long long m_lastDrawTime;
		unsigned long long m_lastSwitchTime;
		unsigned long long m_highTime;
		bool m_bLastState;

		double m_lastDrawState;
		LogicIn * m_pIn;
		Simulator * m_pSimulator;
};

#endif
