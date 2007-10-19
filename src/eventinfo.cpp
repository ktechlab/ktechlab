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


EventInfo::EventInfo()
{
	reset();
}


EventInfo::EventInfo( ItemView *itemView, QEvent *e)
{
	Q_UNUSED(itemView);
	Q_UNUSED(e);
	reset();
}

EventInfo::EventInfo( ItemView *itemView, QMouseEvent *e)
{
	pos = e->pos()/itemView->zoomLevel();
	globalPos = e->globalPos();
	isRightClick = e->button() == Qt::RightButton;
	ctrlPressed = e->state() & QMouseEvent::ControlButton;
	shiftPressed = e->state() & QMouseEvent::ShiftButton;
	altPressed = e->state() & QMouseEvent::AltButton;
	if( ItemDocument * id = dynamic_cast<ItemDocument*>(itemView->document()))
			qcanvasItemClickedOn = id->itemAtTop(pos);
	itemRtti = qcanvasItemClickedOn ? qcanvasItemClickedOn->rtti() : ItemDocument::RTTI::None;
	scrollDelta = 0;
	scrollOrientation = Qt::Vertical;
}


EventInfo::EventInfo( ItemView *itemView, QWheelEvent *e)
{
	pos = e->pos()/itemView->zoomLevel();
	globalPos = e->globalPos();
	isRightClick = false;
	ctrlPressed = e->state() & QMouseEvent::ControlButton;
	shiftPressed = e->state() & QMouseEvent::ShiftButton;
	altPressed = e->state() & QMouseEvent::AltButton;
	if( ItemDocument * id = dynamic_cast<ItemDocument*>(itemView->document()))
		qcanvasItemClickedOn = id->itemAtTop(pos);
	itemRtti = qcanvasItemClickedOn ? qcanvasItemClickedOn->rtti() : ItemDocument::RTTI::None;
	scrollDelta = e->delta();
	scrollOrientation = e->orientation();
	
// 	kdDebug() << "scrollOrientation="<<scrollOrientation<<endl;
}


void EventInfo::reset()
{
	isRightClick = false;
	ctrlPressed = false;
	shiftPressed = false;
	altPressed = false;
	qcanvasItemClickedOn = 0;
	itemRtti = ItemDocument::RTTI::None;
	scrollDelta = 0;
	scrollOrientation = Qt::Vertical;
}


QMouseEvent *EventInfo::mousePressEvent( int dx, int dy) const
{
	return new QMouseEvent( QEvent::MouseButtonPress,
							pos + QPoint( dx, dy),
							(isRightClick ? Qt::RightButton : Qt::LeftButton), 
							(isRightClick ? Qt::RightButton : Qt::LeftButton) |
									(ctrlPressed ? Qt::ControlButton : 0) |
									(shiftPressed ? Qt::ShiftButton : 0) |
									(altPressed ? Qt::AltButton : 0));
}


QMouseEvent *EventInfo::mouseReleaseEvent( int dx, int dy) const
{
	return new QMouseEvent( QEvent::MouseButtonRelease,
							pos + QPoint( dx, dy),
							(isRightClick ? Qt::RightButton : Qt::LeftButton), 
							(isRightClick ? Qt::RightButton : Qt::LeftButton) |
									(ctrlPressed ? Qt::ControlButton : 0) |
									(shiftPressed ? Qt::ShiftButton : 0) |
									(altPressed ? Qt::AltButton : 0));
}


QMouseEvent *EventInfo::mouseDoubleClickEvent( int dx, int dy) const
{
	return new QMouseEvent( QEvent::MouseButtonDblClick,
							pos + QPoint( dx, dy),
							(isRightClick ? Qt::RightButton : Qt::LeftButton), 
							(isRightClick ? Qt::RightButton : Qt::LeftButton) |
									(ctrlPressed ? Qt::ControlButton : 0) |
									(shiftPressed ? Qt::ShiftButton : 0) |
									(altPressed ? Qt::AltButton : 0));
}


QMouseEvent *EventInfo::mouseMoveEvent( int dx, int dy) const
{
	return new QMouseEvent( QEvent::MouseMove,
							pos + QPoint( dx, dy),
							Qt::NoButton, 
							(ctrlPressed ? Qt::ControlButton : 0) |
									(shiftPressed ? Qt::ShiftButton : 0) |
									(altPressed ? Qt::AltButton : 0));
}


QWheelEvent *EventInfo::wheelEvent( int dx, int dy) const
{
	return new QWheelEvent( pos + QPoint( dx, dy),
							scrollDelta,
							(ctrlPressed ? Qt::ControlButton : 0) |
									(shiftPressed ? Qt::ShiftButton : 0) |
									(altPressed ? Qt::AltButton : 0),
							scrollOrientation);
}

