/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DPLINE_H
#define DPLINE_H

#include "drawpart.h"

class LineOverlay;

/**
@author David Saxton
*/
class DPLine : public DrawPart
{
	public:
		DPLine( ItemDocument *itemDocument, bool newItem, const char *id = 0 );
		~DPLine();

		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();

		virtual void setSelected( bool yes );

	protected:
		virtual void postResize();
		virtual void dataChanged();
		virtual void drawShape( QPainter &p );
		
		LineOverlay * m_pLineOverlay;
};

/**
@author David Saxton
*/
class DPArrow : public DPLine
{
	public:
		DPArrow( ItemDocument *itemDocument, bool newItem, const char *id = 0 );
		~DPArrow();

		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();

	protected:
		virtual void drawShape( QPainter &p );
};

#endif
