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
#include "idocumentplugin.h"

#if KDE_ENABLED
#include <shell/core.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/iplugin.h>
#include <KComponentData>
#include <KSharedConfig>
#include <KDebug>
#else
#include <QDebug>
#endif

using namespace KTechLab;

IComponentItemFactory::IComponentItemFactory()
    :   m_componentDataList( QList<ComponentMetaData>() )
{
}

QList<ComponentMetaData> IComponentItemFactory::allMetaData()
{
    return m_componentDataList;
}

void IComponentItemFactory::addSupportedComponent( const ComponentMetaData & data )
{
    m_componentDataList.append( data );
}

#if KDE_ENABLED
void IComponentItemFactory::loadComponentsFromFile ( const QString& file )
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig( file, KConfig::CascadeConfig );

    foreach( const QString name, config->groupList() ){
        kDebug() << "adding component: " << name;
        addSupportedComponent( IComponent::metaData( name, *config ) );
    }
}
#else
void IComponentItemFactory::loadComponentsFromFile ( const QString& /* file */ )
{
    // TODO implement
    qCritical() << "TODO IComponentItemFactory::loadComponentsFromFile";
}
#endif

IComponentPlugin::IComponentPlugin( KComponentData data, QObject *parent )
    :   KDevelop::IPlugin( data, parent )
{

}

#if KDE_ENABLED
IDocumentPlugin* IComponentPlugin::documentPlugin() const
{
    QStringList constraints;
    constraints << QString("'%1' in [X-KDevelop-SupportedMimeTypes]").arg("application/x-circuit");
    QList<KDevelop::IPlugin*> plugins = KDevelop::Core::self()->pluginController()->allPluginsForExtension( "org.kdevelop.IDocument", constraints );
    if (plugins.isEmpty()) {
        kWarning() << "No plugin found to load KTechLab Documents";
        return 0;
    }
    IDocumentPlugin *plugin = qobject_cast<IDocumentPlugin*>( plugins.first() );
    return plugin;
}
#else
IDocumentPlugin* IComponentPlugin::documentPlugin() const
{
    // TODO implement
    qCritical() << "TODO IComponentPlugin::documentPlugin";
    return NULL;
}
#endif

// vim: sw=4 sts=4 et tw=100
