/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECJFET_H
#define ECJFET_H

#include "component.h"

class JFET;

/**
@short Simulates a JFET
@author David Saxton
 */
class ECJFET : public Component
{
	public:
		ECJFET( int JFET_type, ICNDocument *icnDocument, bool newItem, const char * id = nullptr );
		~ECJFET() override;

		static Item * constructNJFET( ItemDocument * itemDocument, bool newItem, const char * id );
		static Item * constructPJFET( ItemDocument * itemDocument, bool newItem, const char * id );
		static LibraryItem * libraryItemNJFET();
		static LibraryItem * libraryItemPJFET();

	protected:
		void dataChanged() override;
		void drawShape( QPainter &p ) override;

		int m_JFET_type;
		JFET * m_pJFET;
};

#endif
