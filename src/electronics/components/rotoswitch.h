/***************************************************************************
 *   Copyright (C) 2005 by John Myers                                      *
 *   electronerd@electronerdia.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ROTOSWITCH_H
#define ROTOSWITCH_H

#include "component.h"
#include <QVector>

struct SwitchPosition
{
    ECNode* node;
    Switch* posSwitch;
    bool isMomentary;
    int pinAngle;
};

/**
 * A rotary switch
 * \author John Myers
 */
class ECRotoSwitch : public Component
{
public:
    ECRotoSwitch( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
    ~ECRotoSwitch() override;
    
    static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
    static LibraryItem *libraryItem();
    
    void buttonStateChanged( const QString &id, bool state ) override;
    void dataChanged() override;
    
private:
    void drawShape( QPainter &p ) override;

    int m_numPositions;
    int m_curPosition;

    ///Half the total width of the component including pins
    static const int _pinOuterRadius = 64;
    ///The width of the pins
    static const int _pinWidth = 8;
    ///the radius of the circle centered at the origin and tangent to the inner edge of the rows of pins
    static const int _pinInnerRadius = _pinOuterRadius - _pinWidth;
    ///gap between pins and contact circles
    static const int _wireGap = 7;
    ///The radius of the largest circle tangent to the pin circles
    static const int _contactOuterRadius  = _pinInnerRadius - _wireGap;
    ///The radius of the circles used to show positions
    static const int _contactRadius = 2;
    ///The radius of the ring of positions
    static const int _contactRingRadius = _contactOuterRadius - _contactRadius;

    QVector<SwitchPosition> m_positions;
    ECNode* m_inNode;

protected:
    void setUpSwitches();
protected:
    void setActivePosition(int newPosition);
};
#endif //ROTOSWITCH_H
