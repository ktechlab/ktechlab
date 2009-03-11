/**************************************************************************
*   Copyright (C) 2009 by Julian Bäume <julian@svg4all.de>                *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "resistor.h"

#include <KLocale>
#include <KIconLoader>

#include <KDebug>

//BEGIN class Resistor

KTechLab::ComponentMetaData Resistor::metaData()
{
    KIconLoader *iconLoader = KIconLoader::global();
    iconLoader->addAppDir( "ktechlab" );
    KTechLab::ComponentMetaData data = {
        "ec/resistor",
        i18n("Resistor"),
        i18n("Passive"),
        KIcon( iconLoader->iconPath( "resistor", KIconLoader::User ) ),
        ""
        //LibraryItem::lit_component
    };
    return data;
}

Resistor::Resistor()
{

}

Resistor::~Resistor()
{
}

void Resistor::dataChanged()
{
}

//END class Resistor

