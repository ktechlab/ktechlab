/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "eventinfo.h"
#include "itemdocument.h"
#include "itemview.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>
#include <QWheelEvent>


EventInfo::EventInfo()
{
	reset();
}


EventInfo::EventInfo( ItemView *itemView, QEvent *e )
{
	Q_UNUSED(itemView);
	Q_UNUSED(e);
	reset();
}

void EventInfo::extractPos( ItemView * itemView, const QPoint & contentsMouseClick )
{
	pos = itemView->mousePosToCanvasPos( contentsMouseClick );
}

EventInfo::EventInfo( ItemView *itemView, QMouseEvent *e )
{
	reset();
	
	extractPos( itemView, e->pos() );
	globalPos = e->globalPos();
	isRightClick = e->button() == Qt::RightButton;
	isMiddleClick = e->button() == Qt::MidButton;
	ctrlPressed = e->state() & Qt::ControlModifier;
	shiftPressed = e->state() & Qt::ShiftModifier;
	altPressed = e->state() & Qt::AltModifier;
	if ( ItemDocument * id = dynamic_cast<ItemDocument*>(itemView->document()) )
			qcanvasItemClickedOn = id->itemAtTop(pos);
	scrollDelta = 0;
	scrollOrientation = Qt::Vertical;
}


EventInfo::EventInfo( ItemView *itemView, QWheelEvent *e )
{
	reset();
	
	extractPos( itemView, e->pos() );
	globalPos = e->globalPos();
	ctrlPressed = e->state() & Qt::ControlModifier;
	shiftPressed = e->state() & Qt::ShiftModifier;
	altPressed = e->state() & Qt::AltModifier;
	if ( ItemDocument * id = dynamic_cast<ItemDocument*>(itemView->document()) )
		qcanvasItemClickedOn = id->itemAtTop(pos);
	scrollDelta = e->delta();
	scrollOrientation = e->orientation();
}


void EventInfo::reset()
{
	isRightClick = false;
	isMiddleClick = false;
	ctrlPressed = false;
	shiftPressed = false;
	altPressed = false;
	qcanvasItemClickedOn = 0l;
	scrollDelta = 0;
	scrollOrientation = Qt::Vertical;
}


QMouseEvent *EventInfo::mousePressEvent( int dx, int dy ) const
{
	return new QMouseEvent( QEvent::MouseButtonPress,
							pos + QPoint( dx, dy ),
							(isRightClick ? Qt::RightButton : Qt::LeftButton), 
							(isRightClick ? Qt::RightButton : Qt::LeftButton) |
									(ctrlPressed ? Qt::ControlModifier : 0 ) |
									(shiftPressed ? Qt::ShiftModifier : 0 ) |
									(altPressed ? Qt::AltModifier : 0 ) );
}


QMouseEvent *EventInfo::mouseReleaseEvent( int dx, int dy ) const
{
	return new QMouseEvent( QEvent::MouseButtonRelease,
							pos + QPoint( dx, dy ),
							(isRightClick ? Qt::RightButton : Qt::LeftButton), 
							(isRightClick ? Qt::RightButton : Qt::LeftButton) |
									(ctrlPressed ? Qt::ControlModifier : 0 ) |
									(shiftPressed ? Qt::ShiftModifier : 0 ) |
									(altPressed ? Qt::AltModifier : 0 ) );
}


QMouseEvent *EventInfo::mouseDoubleClickEvent( int dx, int dy ) const
{
	return new QMouseEvent( QEvent::MouseButtonDblClick,
							pos + QPoint( dx, dy ),
							(isRightClick ? Qt::RightButton : Qt::LeftButton), 
							(isRightClick ? Qt::RightButton : Qt::LeftButton) |
									(ctrlPressed ? Qt::ControlModifier : 0 ) |
									(shiftPressed ? Qt::ShiftModifier : 0 ) |
									(altPressed ? Qt::AltModifier : 0 ) );
}


QMouseEvent *EventInfo::mouseMoveEvent( int dx, int dy ) const
{
	return new QMouseEvent( QEvent::MouseMove,
							pos + QPoint( dx, dy ),
							Qt::NoButton, 
							(ctrlPressed ? Qt::ControlModifier : 0 ) |
									(shiftPressed ? Qt::ShiftModifier : 0 ) |
									(altPressed ? Qt::AltModifier : 0 ) );
}


QWheelEvent *EventInfo::wheelEvent( int dx, int dy ) const
{
	return new QWheelEvent( pos + QPoint( dx, dy ),
							scrollDelta,
							(ctrlPressed ? Qt::ControlModifier : 0 ) |
									(shiftPressed ? Qt::ShiftModifier : 0 ) |
									(altPressed ? Qt::AltModifier : 0 ),
							scrollOrientation );
}

