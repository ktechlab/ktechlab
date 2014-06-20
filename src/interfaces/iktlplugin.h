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

#ifndef IKTLPLUGIN_H_
#define IKTLPLUGIN_H_

#include "ktlinterfacesexport.h"

#if KDE_ENABLED

#include <interfaces/iplugin.h>

#else // KDE_ENABLED

#include <QObject>
/** mock KDE's component data for allowing compilation with and without KDE */
class KTLINTERFACES_EXPORT KComponentData
{
};

#endif // KDE_ENABLED

namespace KTechLab
{

#if KDE_ENABLED
class KTLINTERFACES_EXPORT IKTLPlugin : public KDevelop::IPlugin
#else
class KTLINTERFACES_EXPORT IKTLPlugin : public QObject
#endif
{
    Q_OBJECT

public:
    IKTLPlugin(const KComponentData &data, QObject *parent = 0 );
    virtual ~IKTLPlugin();

};

}

#endif // IKTLPLUGIN_H_



