/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DPTEXT_H
#define DPTEXT_H

#include "drawpart.h"

/**
@short Represents editable text on the canvas
@author David Saxton
*/
class DPText : public DrawPart
{
public:
    DPText(ItemDocument *itemDocument, bool newItem, const char *id = nullptr);
    ~DPText() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();
    static LibraryItem *libraryItemOld();

    void setSelected(bool yes) override;

    QSize minimumSize() const override;

protected:
    void postResize() override;

private:
    void drawShape(QPainter &p) override;
    void dataChanged() override;
    QString m_text;
    bool b_displayBackground;
    QColor m_backgroundColor;
    QColor m_frameColor;
    RectangularOverlay *m_rectangularOverlay;
};

#endif
