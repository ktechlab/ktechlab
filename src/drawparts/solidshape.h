/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SOLIDSHAPE_H
#define SOLIDSHAPE_H

#include <drawpart.h>

class RectangularOverlay;

/**
@short Represents a drawable rectangle on the canvas
@author David Saxton
*/
class DPRectangle : public DrawPart
{
	public:
		DPRectangle( ItemDocument *itemDocument, bool newItem, const char *id = 0L );
		~DPRectangle();

		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();

		virtual void setSelected( bool yes );
	
		virtual QSize minimumSize() const;

	protected:
		virtual void drawShape( QPainter &p );
		void dataChanged();
		virtual void postResize();
		
		/** Returns the rectangle to draw in, taking into account the line
		  * width */
		QRect drawRect() const;
	
	private:
		RectangularOverlay *m_pRectangularOverlay;
};

/**
@short Represents a drawable rectangle on the canvas
@author David Saxton
*/
class DPEllipse : public DPRectangle
{
	public:
		DPEllipse( ItemDocument *itemDocument, bool newItem, const char *id = 0L );
		~DPEllipse();

		static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
		static LibraryItem *libraryItem();

	protected:
		virtual void postResize();
	
	private:
		virtual void drawShape( QPainter &p );
};

#endif
