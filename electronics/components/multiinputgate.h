/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MULTIINPUTGATE_H
#define MULTIINPUTGATE_H

#include "component.h"
#include "logic.h"

const int maxGateInput = 256;

/**
@author David Saxton
*/
class MultiInputGate : public CallbackClass, public Component
{
public:
	MultiInputGate( ICNDocument *icnDocument, bool newItem, const char *id, int baseWidth = -1 );
	~MultiInputGate();
	
protected:
	virtual void inStateChanged( bool newState ) = 0;
	void dataChanged();
	void updateInputs( int newNum );
	
	int m_numInputs;
	int m_baseWidth;
	
	LogicIn *inLogic[maxGateInput];
	ECNode *inNode[maxGateInput];
	
	LogicOut * m_pOut;
	
	virtual void updateAttachedPositioning();
	
private:
	bool b_doneInit;
};


/**
@short Boolean XNOR
@author David Saxton
*/
class ECXnor : public MultiInputGate
{
public:
	ECXnor( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
	~ECXnor();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
private:
	void inStateChanged( bool newState );
	virtual void drawShape( QPainter &p );
};


/**
@short Boolean XOR
@author David Saxton
*/
class ECXor : public MultiInputGate
{
public:
	ECXor( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
	~ECXor();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
protected:
	void inStateChanged( bool newState );
	virtual void drawShape( QPainter &p );
};


/**
@short Boolean OR
@author David Saxton
*/
class ECOr : public MultiInputGate
{
public:
	ECOr( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
	~ECOr();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
protected:
	void inStateChanged( bool newState );
	virtual void drawShape( QPainter &p );
};

/**
@short Boolean NOR
@author David Saxton
*/
class ECNor : public MultiInputGate
{
public:
	ECNor( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
	~ECNor();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
private:
	void inStateChanged( bool newState );
	virtual void drawShape( QPainter &p );
};

/**
@short Boolean NAND
@author David Saxton
*/
class ECNand : public MultiInputGate
{
public:
	ECNand( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
	~ECNand();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
private:
	void inStateChanged( bool newState );
	virtual void drawShape( QPainter &p );
};

/**
@short Boolean AND
@author David Saxton
*/
class ECAnd : public MultiInputGate
{
public:
	ECAnd( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
	~ECAnd();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
private:
	void inStateChanged( bool newState );
	virtual void drawShape( QPainter &p );
};



#endif
