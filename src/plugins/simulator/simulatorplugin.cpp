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
#include "simulatorfactory.h"

#include "interfaces/simulator/isimulationmanager.h"

#include <kpluginfactory.h>
#include <kdebug.h>
#include <KAboutData>

using namespace KTechLab;

K_PLUGIN_FACTORY(SimulatorPluginFactory,
                 registerPlugin<SimulatorPlugin>();
                )

K_EXPORT_PLUGIN(SimulatorPluginFactory( //"ktlsimulatorplugin", "ktlsimulatorplugin"))
    KAboutData("ktlsimulatorplugin","ktlsimulatorplugin",
        ki18n("KTechLab Simulator Plugin"),
        "0.1",
        ki18n("This plugin provides simulators to KTechLab"),
        KAboutData::License_LGPL)))



SimulatorPlugin::SimulatorPlugin(QObject* parent, const QVariantList& args):
              IPlugin(SimulatorPluginFactory::componentData(), parent)
{
    Q_UNUSED(args);
    kDebug() << "SimulatorPlugin created\n";
    printf("simulator plugin created\n");
    // create factories
    m_simFact = new SimulatorFactory();
    // register factories
    ISimulationManager::self()->registerSimulatorFactory(m_simFact);
}

void SimulatorPlugin::unload()
{
    kDebug() << "unload\n";
    // unregister the factories
    ISimulationManager::self()->unregisterSimulatorFactory(m_simFact);
    printf("simulator plugin unloaded\n");
}

SimulatorPlugin::~SimulatorPlugin()
{
    kDebug() << "destruct\n";
}


#include "simulatorplugin.moc"
