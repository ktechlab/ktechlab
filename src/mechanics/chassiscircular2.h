/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CHASSISCIRCULAR2_H
#define CHASSISCIRCULAR2_H

#include "mechanicsitem.h"

/**
@short Mechanics Framework, circular base, two wheels
@author David Saxton
*/
class ChassisCircular2 : public MechanicsItem
{
public:
    ChassisCircular2(MechanicsDocument *mechanicsDocument, bool newItem, const char *id = nullptr);
    ~ChassisCircular2() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

    virtual void advance(int phase);

protected:
    void itemResized() override;
    void drawShape(QPainter &p) override;

    double m_theta1; // Angle of rotation of wheel 1 (used for drawing)
    double m_theta2; // Angle of rotation of wheel 1 (used for drawing)

    QRect m_wheel1Pos; // Position of first wheel, with respect to top left of item
    QRect m_wheel2Pos; // Position of second wheel, with respect to top left of item
};

#endif
