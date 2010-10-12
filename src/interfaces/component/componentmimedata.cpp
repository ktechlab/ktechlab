/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "componentmimedata.h"
#include "icomponentplugin.h"

using namespace KTechLab;

ComponentMimeData::ComponentMimeData( const QByteArray &name, KTechLab::IComponentItemFactory *factory )
    :   QMimeData(),
        m_componentFactory( factory ),
        m_componentName( name )
{}

KTechLab::IComponent *ComponentMimeData::createComponent()
{
    return m_componentFactory->create( m_componentName );
}

QByteArray ComponentMimeData::name() const
{
    return m_componentName;
}

QByteArray ComponentMimeData::type() const
{
    return data("ktechlab/x-icomponent");
}


// vim: sw=4 sts=4 et tw=100
