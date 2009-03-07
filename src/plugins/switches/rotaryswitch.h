/***************************************************************************
 *   Copyright (C) 2005 by John Myers <electronerd@electronerdia.net>      *
 *   Copyright (C) 2009 by Julian Bäume <julian@svg4all.de>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ROTARYSWITCH_H
#define ROTARYSWITCH_H

#include "interfaces/component/icomponent.h"

struct SwitchPosition
{
//    ECNode* node;
//    Switch* posSwitch;
    bool isMomentary;
    int pinAngle;
};

/**
 * A rotary switch
 * \author John Myers 
 */
class RotarySwitch : public KTechLab::IComponent
{
public:
    RotarySwitch( );
    virtual ~RotarySwitch();

    static KTechLab::ComponentMetaData metaData();

    virtual void dataChanged();

private:
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

//    QValueVector<SwitchPosition> m_positions;
//    ECNode* m_inNode;

protected:
};
#endif //ROTARYSWITCH_H

