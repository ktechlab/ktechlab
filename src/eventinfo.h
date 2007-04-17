/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef EVENTINFO_H
#define EVENTINFO_H

#include <qpoint.h>

class ItemView;

class QCanvasItem;
class QEvent;
class QMouseEvent;
class QWheelEvent;

/**
Contains information from for a mouse event that occured on a canvas. Like a
QMouseEvent / QEvent / QWheelEvent, but abstracted to the canvas coordinate
system, as well as holding lots of useful information.
@author David Saxton
*/
class EventInfo
{
public:
	EventInfo();
	EventInfo( ItemView *itemView, QMouseEvent *e );
	EventInfo( ItemView *itemView, QWheelEvent *e );
	EventInfo( ItemView *itemView, QEvent *e );
	
	QMouseEvent *mousePressEvent( int dx, int dy ) const;
	QMouseEvent *mouseReleaseEvent( int dx, int dy ) const;
	QMouseEvent *mouseDoubleClickEvent( int dx, int dy ) const;
	QMouseEvent *mouseMoveEvent( int dx, int dy ) const;
	QWheelEvent *wheelEvent( int dx, int dy ) const;
	
	QPoint pos;
	QPoint globalPos;
	QCanvasItem * qcanvasItemClickedOn;
	short scrollDelta;
	Qt::Orientation scrollOrientation;
	bool isRightClick:1;
	bool isMiddleClick:1;
	bool ctrlPressed:1;
	bool shiftPressed:1;
	bool altPressed:1;
	
protected:
	void extractPos( ItemView * itemView, const QPoint & contentsMouseClick );
	void reset();
};

#endif
