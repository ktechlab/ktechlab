/***************************************************************************
 *    IKtlPlugin interface                                                 *
 *       A common interface for all plugins in KTechLab                    *
 *    Copyright (c) 2014 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "iktlplugin.h"

namespace KTechLab
{

#if KDE_ENABLED

IKTLPlugin::IKTLPlugin( KComponentData data, QObject *parent) :
    KDevelop::IPlugin(data, parent)
{
}

#else // KDE_ENABLE

IKTLPlugin::IKTLPlugin( KComponentData /* data */, QObject *parent) :
    QObject(parent)
{
}

#endif // KDE_ENABLE

IKTLPlugin::~IKTLPlugin()
{
}

}

