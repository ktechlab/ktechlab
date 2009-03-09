/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ICOMPONENTPLUGIN_H
#define ICOMPONENTPLUGIN_H

#include "../ktlinterfacesexport.h"
#include "icomponent.h"

#include <interfaces/iplugin.h>

namespace KTechLab
{
class IComponent;

class KTLINTERFACES_EXPORT IComponentFactory
{
public:
    IComponentFactory();
    virtual ~IComponentFactory() {};

    virtual IComponent * create( const QString &name )=0;

    QList<KTechLab::ComponentMetaData> allMetaData();
protected:
    void addSupportedComponent( const KTechLab::ComponentMetaData & data );

private:
    QList<KTechLab::ComponentMetaData> m_componentDataList;
};

class KTLINTERFACES_EXPORT IComponentPlugin: public KDevelop::IPlugin
{
public:
    IComponentPlugin( KComponentData data, QObject *parent = 0 );
};

} // namespace KTechLab

#endif

