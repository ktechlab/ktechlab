/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PUSHSWITCH_H
#define PUSHSWITCH_H

#include "simplecomponent.h"

class Switch;

/**
@short Push-to-Break switch component
@author David Saxton
*/
class ECPTBSwitch : public SimpleComponent
{
public:
	ECPTBSwitch( ICNDocument *icnDocument, bool newItem, const char *id = 0);
	~ECPTBSwitch();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	virtual void buttonStateChanged( const QString &id, bool state );
	virtual void dataChanged();
	
private:
	virtual void drawShape( QPainter &p );
	Switch *m_switch;
	bool pressed;
};


/**
@short Push-to-make switch
@author David Saxton
*/
class ECPTMSwitch : public SimpleComponent
{
public:
	ECPTMSwitch( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~ECPTMSwitch();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	virtual void buttonStateChanged( const QString &id, bool state );
	virtual void dataChanged();
	
private:
	virtual void drawShape( QPainter &p );
	Switch *m_switch;
	bool pressed;
};


#endif
