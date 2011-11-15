/***************************************************************************
*   Copyright (C) 2011 Zoltan Padrah <zoltan_padrah@users.sourceforge.net>*
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ktlguiplugin.h"

#include <KGenericFactory>
#include <KAboutData>

#include <stdio.h>


using namespace KTechLab;

K_PLUGIN_FACTORY(KTLGuiPluginFactory, registerPlugin<KTLGuiPlugin>(); )

K_EXPORT_PLUGIN(KTLGuiPluginFactory(
    KAboutData("ktlgui","ktlgui",
        ki18n("KTechLab GUI"),
        "0.1",
        ki18n("This plugin provides GUI to KTechLab"),
        KAboutData::License_LGPL)))

KTLGuiPlugin::KTLGuiPlugin(QObject* parent, const QVariantList& /* args */)
  : IPlugin( KTLGuiPluginFactory::componentData(), parent )
{
    printf("gui plugin created\n");
}

KTLGuiPlugin::~KTLGuiPlugin()
{
    printf("gui plugin destroyed\n");
}


#include "ktlguiplugin.moc"

