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


#ifndef SIMULATORPLUGIN_H
#define SIMULATORPLUGIN_H

#include <kdevplatform/interfaces/iplugin.h>

#include <qvariant.h>

namespace KTechLab {

    class SimulatorFactory;

/**
 * Plugin for the electronic simulator
 * The plugin only registers the simulator and element (model) factories
*/
class SimulatorPlugin : public KDevelop::IPlugin
{
        Q_OBJECT
    public:
        // creation
        SimulatorPlugin(QObject *parent, const QVariantList& args );
        // destructor
        virtual ~SimulatorPlugin();
        // called at unloading of the plugin
        virtual void unload();
    private:
        SimulatorFactory *m_simFact;
};

}
#endif // SIMULATORPLUGIN_H
