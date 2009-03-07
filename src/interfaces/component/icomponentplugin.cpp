/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "icomponentplugin.h"
#include "icomponent.h"

#include <KComponentData>

using namespace KTechLab;

IComponentFactory::IComponentFactory()
    :   m_componentDataList( QList<KTechLab::ComponentMetaData>() )
{
}

QList<KTechLab::ComponentMetaData> IComponentFactory::allMetaData()
{
    return m_componentDataList;
}

void IComponentFactory::addSupportedComponent( const KTechLab::ComponentMetaData & data )
{
    m_componentDataList.append( data );
}

IComponentPlugin::IComponentPlugin( KComponentData data, QObject *parent )
    :   KDevelop::IPlugin( data, parent )
{

}

// vim: sw=4 sts=4 et tw=100
