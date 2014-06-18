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

#if KDE_ENABLED

#include <interfaces/iplugin.h>

#else // KDE_ENABLED

#include <QObject>
/** mock KDE's component data for allowing compilation with and without KDE */
class KComponentData
{
};

#endif // KDE_ENABLED

namespace KTechLab
{

class IKTLPlugin :

#if KDE_ENABLED
    public KDevelop::IPlugin
#else
    public QObject
#endif
{
    Q_OBJECT

public:
    IKTLPlugin( KComponentData data, QObject *parent = 0 );
    virtual ~IKTLPlugin();

};

}

#endif // IKTLPLUGIN_H_



