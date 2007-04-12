/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RAM_H
#define RAM_H

#include "component.h"
#include "logic.h"

#include <qbitarray.h>
#include <qptrvector.h>

/**
@author David Saxton
*/
class RAM : public CallbackClass, public Component
{
	public:
		RAM( ICNDocument *icnDocument, bool newItem, const char *id = 0 );
		~RAM();
	
		virtual bool canFlip() const { return true; }
		static Item * construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem * libraryItem();
	
	protected:
		void initPins();
		virtual void dataChanged();
		void inStateChanged( bool newState );
	
		QBitArray m_data;
		LogicIn * m_pCS; // Chip select
		LogicIn * m_pOE; // Output enable
		LogicIn * m_pWE; // Write enable
		
		int m_wordSize;
		int m_addressSize;
		
		QPtrVector<LogicIn> m_address;
		QPtrVector<LogicIn> m_dataIn;
		QPtrVector<LogicOut> m_dataOut;
};

#endif
