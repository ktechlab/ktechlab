/***************************************************************************
*   Copyright (C) 2010 Julian Bäume <julian@svg4all.de>                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "icomponent.h"

#if KDE_ENABLED
#include <KConfig>
#include <KConfigGroup>
#include <KIconLoader>
#endif

#include <QDebug>

KTechLab::ComponentMetaData KTechLab::IComponent::metaData ( const QString& name, const KConfig& metaData )
{
#if KDE_ENABLED
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
#else
    ComponentMetaData data;
    qCritical() << "KTechLab::IComponent::metaData: returning dummy data";
#endif
    return data;
}
