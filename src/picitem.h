/***************************************************************************
 *   Copyright (C) 2003,2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PICITEM_H
#define PICITEM_H

#include "cnitem.h"

#include <QObject>

class MicroSettings;
class FlowCodeDocument;
class PinSettings;

/**
@short Allows visual setting of pin type/state
@author David Saxton
*/
class PinItem : /*public QObject, */ public KtlQCanvasRectangle
{
    Q_OBJECT
public:
    PinItem(FlowCodeDocument *_view, QPoint position, bool _onLeft, PinSettings *_pinSettings);

    QRect boundingRect() const override;
    void switchState();

    QString id();

    /**
     * Called from ICNDocument when the pin item was dragged
     */
    void dragged(int dx);

    void moveBy(double dx, double dy) override;

public slots:
    void updateDrawing();

private:
    void initItem();
    void drawShape(QPainter &p) override;
    void calcTextRect();

    FlowCodeDocument *view; // Pointer to canvas view that the component item is currently on
    bool onLeft;
    PinSettings *m_pinSettings;
    QRect m_textRect;
    QFont m_font;
};
typedef QList<PinItem *> PinItemList;

/**
Allows visual editing of initial PIC settings
@author David Saxton
*/
class PicItem : public CNItem
{
    Q_OBJECT
public:
    PicItem(ICNDocument *icnDocument, bool newItem, const char *id, MicroSettings *_microSettings);
    ~PicItem() override;

    void drawShape(QPainter &p) override;

    void buttonStateChanged(const QString &id, bool state) override;
    bool isMovable() const override
    {
        return false;
    }

    static QString typeString()
    {
        return "microitem";
    }
    void updateZ(int baseZ) override;

    bool mousePressEvent(const EventInfo &info) override;
    bool mouseReleaseEvent(const EventInfo &info) override;
    bool mouseMoveEvent(const EventInfo &info) override;

protected slots:
    void slotMicroSettingsDlgAccepted();

protected:
    void updateVisibility();

    MicroSettings *microSettings;
    PinItemList m_pinItemList;
    ICNDocument *p_icnDocument;
    bool m_bExpanded;
    int m_innerHeight;

private:
    QPoint m_pos;
    int m_dx;
    bool m_pressed;
    bool m_dragged;
};

#endif
