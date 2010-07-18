/*
    KTechLab simulator plugin
    Copyright (C) 2010 Zoltan Padrah <zoltan.padrah@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef SIMULATORPLUGIN_H
#define SIMULATORPLUGIN_H

#include <kdevplatform/interfaces/iplugin.h>

#include <qvariant.h>

namespace KTechLab {

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
};

}
#endif // SIMULATORPLUGIN_H
