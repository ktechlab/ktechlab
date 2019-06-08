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

#include "component.h"

/**
@short Push-to-Break switch component
@author David Saxton
*/
class ECPTBSwitch : public Component
{
public:
	ECPTBSwitch( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~ECPTBSwitch() override;
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	void buttonStateChanged( const QString &id, bool state ) override;
	void dataChanged() override;
	
private:
	void drawShape( QPainter &p ) override;
	Switch *m_switch;
	bool pressed;
};


/**
@short Push-to-make switch
@author David Saxton
*/
class ECPTMSwitch : public Component
{
public:
	ECPTMSwitch( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~ECPTMSwitch() override;
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	void buttonStateChanged( const QString &id, bool state ) override;
	void dataChanged() override;
	
private:
	void drawShape( QPainter &p ) override;

	Switch *m_switch;
	bool pressed;
};


#endif
