/***************************************************************************
*   Copyright (C) 2010 Julian Bäume <julian@svg4all.de>                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "icomponent.h"
#include <KConfig>
#include <KConfigGroup>
#include <KIconLoader>

KTechLab::ComponentMetaData KTechLab::IComponent::metaData ( const QString& name, const KConfig& metaData )
{
    KConfigGroup item = metaData.group(name);
    KIconLoader *iconLoader = KIconLoader::global();
    iconLoader->addAppDir( "ktechlab" );
    ComponentMetaData data = {
        item.readEntry("name").toUtf8(),
        item.readEntry("title"),
        item.readEntry("category"),
        KIcon( iconLoader->iconPath( item.readEntry("icon"), KIconLoader::User ) ),
        item.readEntry("type").toUtf8()
    };
    return data;
}
