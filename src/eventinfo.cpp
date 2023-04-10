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

#include <QMouseEvent>

EventInfo::EventInfo()
{
    reset();
}

EventInfo::EventInfo(ItemView *itemView, QEvent *e)
{
    Q_UNUSED(itemView);
    Q_UNUSED(e);
    reset();
}

void EventInfo::extractPos(ItemView *itemView, const QPoint &contentsMouseClick)
{
    pos = itemView->mousePosToCanvasPos(contentsMouseClick);
}

EventInfo::EventInfo(ItemView *itemView, QMouseEvent *e)
{
    reset();

    extractPos(itemView, e->pos());
    globalPos = e->globalPos();
    isRightClick = e->button() == Qt::RightButton;
    isMiddleClick = e->button() == Qt::MidButton;
    ctrlPressed = e->modifiers() & Qt::ControlModifier; // QMouseEvent::ControlButton;
    shiftPressed = e->modifiers() & Qt::ShiftModifier;  // QMouseEvent::ShiftButton;
    altPressed = e->modifiers() & Qt::AltModifier;      // QMouseEvent::AltButton;
    if (ItemDocument *id = dynamic_cast<ItemDocument *>(itemView->document()))
        qcanvasItemClickedOn = id->itemAtTop(pos);
}

EventInfo::EventInfo(ItemView *itemView, QWheelEvent *e)
{
    reset();

    extractPos(itemView, e->position().toPoint());
    globalPos = e->globalPosition().toPoint();
    ctrlPressed = e->modifiers() & Qt::ControlModifier; // QMouseEvent::ControlButton;
    shiftPressed = e->modifiers() & Qt::ShiftModifier;  // QMouseEvent::ShiftButton;
    altPressed = e->modifiers() & Qt::AltModifier;      // QMouseEvent::AltButton;
    if (ItemDocument *id = dynamic_cast<ItemDocument *>(itemView->document()))
        qcanvasItemClickedOn = id->itemAtTop(pos);
    pixelDelta = e->pixelDelta();
    angleDelta = e->angleDelta();
    phase = e->phase();
    inverted = e->inverted();
}

void EventInfo::reset()
{
    isRightClick = false;
    isMiddleClick = false;
    ctrlPressed = false;
    shiftPressed = false;
    altPressed = false;
    qcanvasItemClickedOn = nullptr;
    pixelDelta.setX(0);
    pixelDelta.setY(0);
    angleDelta.setX(0);
    angleDelta.setY(0);
    phase = Qt::NoScrollPhase;
    inverted = false;
}

QMouseEvent *EventInfo::mousePressEvent(int dx, int dy) const
{
    return new QMouseEvent(QEvent::MouseButtonPress,
                           pos + QPoint(dx, dy),
                           (isRightClick ? Qt::RightButton : Qt::LeftButton),
                           (isRightClick ? Qt::RightButton : Qt::LeftButton),
                           (ctrlPressed ? Qt::ControlModifier : Qt::NoModifier) | (shiftPressed ? Qt::ShiftModifier : Qt::NoModifier) | (altPressed ? Qt::AltModifier : Qt::NoModifier));
}

QMouseEvent *EventInfo::mouseReleaseEvent(int dx, int dy) const
{
    return new QMouseEvent(QEvent::MouseButtonRelease,
                           pos + QPoint(dx, dy),
                           (isRightClick ? Qt::RightButton : Qt::LeftButton),
                           Qt::NoButton
                           /*(isRightClick ? Qt::RightButton : Qt::LeftButton)
                            * - 2017.09.18 - this is the state after the mouse has been released */
                           ,
                           (ctrlPressed ? Qt::ControlModifier : Qt::NoModifier) | (shiftPressed ? Qt::ShiftModifier : Qt::NoModifier) | (altPressed ? Qt::AltModifier : Qt::NoModifier));
}

QMouseEvent *EventInfo::mouseDoubleClickEvent(int dx, int dy) const
{
    return new QMouseEvent(QEvent::MouseButtonDblClick,
                           pos + QPoint(dx, dy),
                           (isRightClick ? Qt::RightButton : Qt::LeftButton),
                           (isRightClick ? Qt::RightButton : Qt::LeftButton),
                           (ctrlPressed ? Qt::ControlModifier : Qt::NoModifier) | (shiftPressed ? Qt::ShiftModifier : Qt::NoModifier) | (altPressed ? Qt::AltModifier : Qt::NoModifier));
}

QMouseEvent *EventInfo::mouseMoveEvent(int dx, int dy) const
{
    return new QMouseEvent(
        QEvent::MouseMove, pos + QPoint(dx, dy), Qt::NoButton, Qt::NoButton, (ctrlPressed ? Qt::ControlModifier : Qt::NoModifier) | (shiftPressed ? Qt::ShiftModifier : Qt::NoModifier) | (altPressed ? Qt::AltModifier : Qt::NoModifier));
}

QWheelEvent *EventInfo::wheelEvent(int dx, int dy) const
{
    return new QWheelEvent(
        pos + QPoint(dx, dy), globalPos,
        pixelDelta, angleDelta,
        Qt::NoButton,
        (ctrlPressed ? Qt::ControlModifier : Qt::NoModifier) | (shiftPressed ? Qt::ShiftModifier : Qt::NoModifier) | (altPressed ? Qt::AltModifier : Qt::NoModifier),
        phase, inverted);
}
