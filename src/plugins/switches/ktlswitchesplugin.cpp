/***************************************************************************
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktlswitchesplugin.h"

#include "shell/core.h"
#include "interfaces/component/icomponent.h"

#include <KGenericFactory>
#include <KAboutData>

K_PLUGIN_FACTORY(KTLSwitchesPluginFactory, registerPlugin<KTLSwitchesPlugin>(); )
K_EXPORT_PLUGIN(KTLSwitchesPluginFactory(KAboutData("ktlswitches","ktlswitches", ki18n("KTechLab Switch Components"), "0.1", ki18n("Provide a set of standard switch components"), KAboutData::License_LGPL)))


class KTLSwitchesFactory: KTechLab::IComponentFactory
{
public:
    virtual KTechLab::IComponent * create( const QString &name )
    {
        return 0;
    }
};

KTLSwitchesPlugin::KTLSwitchesPlugin( QObject *parent, const QVariantList& args )
    : KTechLab::IComponentPlugin( KTLSwitchesPluginFactory::componentData(), parent )
{

    init();
}

void KTLSwitchesPlugin::init()
{
    KTechLab::Core::self()->pluginController();
}

KTLSwitchesPlugin::~KTLSwitchesPlugin()
{
}

void KTLSwitchesPlugin::unload()
{
    //me be, we should unregister our components at the circuit-plugin
}

#include "ktlswitchesplugin.moc"

// vim: sw=4 sts=4 et tw=100
