/***************************************************************************
 *    KTechLab simulator plugin                                            *
 *       A plugin that registers factories for the simulator and elements  *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "simulatorplugin.h"

#include <kpluginfactory.h>
#include <kdebug.h>

using namespace KTechLab;

K_PLUGIN_FACTORY(SimulatorPluginFactory,
                 registerPlugin<SimulatorPlugin>();
                )
                
K_EXPORT_PLUGIN(SimulatorPluginFactory("ktlsimulator", "ktlsimulator"))


SimulatorPlugin::SimulatorPlugin(QObject* parent, const QVariantList& args):
              IPlugin(SimulatorPluginFactory::componentData(), parent)
{
    kDebug() << "SimulatorPlugin created\n";
    // create factories
    // register factories
}

void SimulatorPlugin::unload()
{
    kDebug() << "unload\n";
    // unregister the factories
}

SimulatorPlugin::~SimulatorPlugin()
{
    kDebug() << "destruct\n";
}


#include "simulatorplugin.moc"
