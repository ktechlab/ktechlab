/***************************************************************************
 *   Copyright (C) 2005,2006 by David Saxton                               *
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
		DPLine( ItemDocument *itemDocument, bool newItem, const char *id = nullptr );
		~DPLine() override;

		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();

		void setSelected( bool yes ) override;

	protected:
		void postResize() override;
		void dataChanged() override;
		void drawShape( QPainter &p ) override;

		LineOverlay * m_pLineOverlay;
};

/**
@author David Saxton
*/
class DPArrow : public DPLine
{
	public:
		DPArrow( ItemDocument *itemDocument, bool newItem, const char *id = nullptr );
		~DPArrow() override;

		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();

	protected:
		void dataChanged() override;
		void drawShape( QPainter &p ) override;
		double m_headAngle;
};

#endif
