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
#include "../idocumentplugin.h"

#include <shell/core.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/iplugin.h>
#include <KComponentData>
#include <KSharedConfig>
#include <KDebug>

using namespace KTechLab;

IComponentFactory::IComponentFactory()
    :   m_componentDataList( QList<ComponentMetaData>() )
{
}

QList<ComponentMetaData> IComponentFactory::allMetaData()
{
    return m_componentDataList;
}

void IComponentFactory::addSupportedComponent( const ComponentMetaData & data )
{
    m_componentDataList.append( data );
}

void IComponentFactory::loadComponentsFromFile ( const QString& file )
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig( file, KConfig::CascadeConfig );

    foreach( const QString name, config->groupList() ){
        kDebug() << "adding component: " << name;
        addSupportedComponent( IComponent::metaData( name, *config ) );
    }
}

IComponentPlugin::IComponentPlugin( KComponentData data, QObject *parent )
    :   KDevelop::IPlugin( data, parent )
{

}

IDocumentPlugin* IComponentPlugin::documentPlugin() const
{
    KDevelop::IPlugin* plugin = KDevelop::Core::self()->pluginController()->pluginForExtension( "org.kdevelop.IDocument", "ktlcircuit" );
    if (!plugin) {
        kError() << "No plugin found to load KTechLab Documents";
        return 0;
    }
    IDocumentPlugin *p = plugin->extension<IDocumentPlugin>();
    return p;
}

// vim: sw=4 sts=4 et tw=100
