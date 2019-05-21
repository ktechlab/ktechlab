/***************************************************************************
 *   Copyright (C) 2003,2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECKEYPAD_H
#define ECKEYPAD_H

#include "component.h"

/**
@short 4x3 PTM Keypad
@author David Saxton
*/
class ECKeyPad : public Component
{
	public:
		ECKeyPad( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
		~ECKeyPad() override;
	
		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();
	
		void buttonStateChanged( const QString &id, bool state ) override;

	protected:
		void dataChanged() override;
		void initPins( unsigned numCols);
		QString buttonID( int row, int col ) const;
		int sideLength( unsigned numButtons ) const;
		
		Switch *m_switch[4][11];
		unsigned m_numCols;
};

#endif
