/**************************************************************************
*   Copyright (C) 2005 by John Myers <electronerd@electronerdia.net>      *
*   Copyright (C) 2009 by Julian Bäume <julian@svg4all.de>                *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "rotaryswitch.h"

#include <KLocale>

#include <KDebug>

//BEGIN class RotarySwitch

KTechLab::ComponentMetaData RotarySwitch::metaData()
{
    KTechLab::ComponentMetaData data = {
        "ec/roto_switch",
        i18n("Rotary"),
        i18n("Switches"),
        KIcon("rotary.png"),
        ""
        //LibraryItem::lit_component
    };
    return data;
}

RotarySwitch::RotarySwitch()
{

}

RotarySwitch::~RotarySwitch()
{
}

void RotarySwitch::dataChanged()
{
}

//END class RotarySwitch

