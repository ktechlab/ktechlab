/* This file is part of the KDE project
Copyright 2004, 2007 Alexander Dymo <adymo@kdevelop.org>
Copyright 2006 Matt Rogers <mattr@kde.org
Copyright 2007 Andreas Pakulat <apaku@gmx.de>

Based on code from Kopete
Copyright (c) 2002-2003 Martijn Klingens <klingens@kde.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public License
along with this library; see the file COPYING.LIB.  If not, write to
the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.
*/
#include "plugincontroller.h"

#include <QtCore/QFile>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QAction>
#include <QtDesigner/QExtensionManager>

#include <kcmdlineargs.h>
#include <klibloader.h>
#include <kservice.h>
#include <kservicetypetrader.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>
#include <kxmlguiwindow.h>
#include <assert.h>
#include <kdebug.h>
#include <kdialog.h>
#include <kstandarddirs.h>
#include <kaction.h>
#include <kxmlguifactory.h>
#include <kmenu.h>

#include <interfaces/contextmenuextension.h>
#include <interfaces/iplugin.h>
#include <interfaces/isession.h>

#include "mainwindow.h"
#include "core.h"
#include "shellextension.h"

namespace KDevelop
{

static const QString pluginControllerGrp("Plugins");

bool isGlobalPlugin( const KPluginInfo& info )
{
    return info.property( "X-KDevelop-Category" ).toString() == "Global";
}

bool hasMandatoryProperties( const KPluginInfo& info )
{
    QVariant mode = info.property( "X-KDevelop-Mode" );
    QVariant version = info.property( "X-KDevelop-Version" );

    return mode.isValid() && mode.canConvert( QVariant::String )
           && version.isValid() && version.canConvert( QVariant::String );
}


class PluginControllerPrivate
{
public:
    QList<KPluginInfo> plugins;

    //map plugin infos to currently loaded plugins
    typedef QMap<KPluginInfo, IPlugin*> InfoToPluginMap;
    InfoToPluginMap loadedPlugins;

    // The plugin manager's mode. The mode is StartingUp until loadAllPlugins()
    // has finished loading the plugins, after which it is set to Running.
    // ShuttingDown and DoneShutdown are used during shutdown by the
    // async unloading of plugins.
    enum CleanupMode
    {
        Running     /**< the plugin manager is running */,
        CleaningUp  /**< the plugin manager is cleaning up for shutdown */,
        CleanupDone /**< the plugin manager has finished cleaning up */
    };
    CleanupMode cleanupMode;

    bool canUnload( const KPluginInfo& plugin )
    {
        QStringList interfaces=plugin.property( "X-KDevelop-Interfaces" ).toStringList();
        foreach( const KPluginInfo& info, loadedPlugins.keys() )
        {
            if( info.pluginName() != plugin.pluginName() ) 
            {
                QStringList dependencies = info.property( "X-KDevelop-IRequired" ).toStringList();
                dependencies += info.property( "X-KDevelop-IOptional" ).toStringList();
                foreach( const QString& dep, dependencies )
                {
                    if( interfaces.contains( dep ) && !canUnload( info ) )
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    KPluginInfo infoForId( const QString& id ) const
    {
        foreach( const KPluginInfo& info, plugins )
        {
            if( info.pluginName() == id )
            {
                return info;
            }
        }
        return KPluginInfo();
    }

    Core *core;
    QExtensionManager* m_manager;
};

PluginController::PluginController(Core *core)
    : IPluginController(), d(new PluginControllerPrivate)
{
    setObjectName("PluginController");
    d->core = core;
    kDebug() << "Fetching plugin info which matches:" << QString( "[X-KDevelop-Version] == %1" ).arg(KDEVELOP_PLUGIN_VERSION);
    d->plugins = KPluginInfo::fromServices( KServiceTypeTrader::self()->query( QLatin1String( "KDevelop/Plugin" ),
        QString( "[X-KDevelop-Version] == %1" ).arg(KDEVELOP_PLUGIN_VERSION) ) );
    foreach( KPluginInfo p, d->plugins )
    {
        if( p.pluginName().contains("nongui") )
                kDebug() << "FOUND test plugin" << p.pluginName();
    }
    d->cleanupMode = PluginControllerPrivate::Running;
    d->m_manager = new QExtensionManager();
    // Register the KDevelop::IPlugin* metatype so we can properly unload it
    qRegisterMetaType<KDevelop::IPlugin*>( "KDevelop::IPlugin*" );
}

PluginController::~PluginController()
{
    if ( d->cleanupMode != PluginControllerPrivate::CleanupDone )
        kWarning(9501) << "Destructing plugin controller without going through the shutdown process! Backtrace is: "
                       << endl << kBacktrace() << endl;

    delete d->m_manager;
    delete d;
}

KPluginInfo PluginController::pluginInfo( const IPlugin* plugin ) const
{
    for ( PluginControllerPrivate::InfoToPluginMap::ConstIterator it = d->loadedPlugins.begin();
          it != d->loadedPlugins.end(); ++it )
    {
        if ( it.value() == plugin )
            return it.key();
    }
    return KPluginInfo();
}

void PluginController::cleanup()
{
    if(d->cleanupMode != PluginControllerPrivate::Running)
    {
        kDebug() << "called when not running. state =" << d->cleanupMode;
        return;
    }

    d->cleanupMode = PluginControllerPrivate::CleaningUp;

    // Ask all plugins to unload
    while ( !d->loadedPlugins.isEmpty() )
    {
        //Let the plugin do some stuff before unloading
        unloadPlugin(d->loadedPlugins.begin().value(), Now);
    }

    d->cleanupMode = PluginControllerPrivate::CleanupDone;
}

IPlugin* PluginController::loadPlugin( const QString& pluginName )
{
    return loadPluginInternal( pluginName );
}

void PluginController::initialize()
{
    QMap<QString, bool> pluginMap;
    // Get the default from the ShellExtension
    foreach( const QString& s, ShellExtension::getInstance()->defaultPlugins() )
    {
        pluginMap.insert( s, true );
    }

    KConfigGroup grp = Core::self()->activeSession()->config()->group( pluginControllerGrp );
    QMap<QString, QString> entries = grp.entryMap();

    QMap<QString, QString>::Iterator it;
    for ( it = entries.begin(); it != entries.end(); ++it )
    {
        QString key = it.key();
        if ( key.endsWith( QLatin1String( "Enabled" ) ) ) 
        {
            QString pluginid = key.left( key.length() - 7 );
            bool defValue;
            QMap<QString, bool>::const_iterator entry = pluginMap.constFind( pluginid );
            if( entry != pluginMap.constEnd() )
            {
                defValue = entry.value();
            } else {
                defValue = false;
            }
            pluginMap.insert( key.left(key.length() - 7), grp.readEntry(key,defValue) );
        }
    }

    foreach( const KPluginInfo& pi, d->plugins )
    {
        if( isGlobalPlugin( pi ) )
        {
            QMap<QString, bool>::const_iterator it = pluginMap.constFind( pi.pluginName() );
            if( pluginMap.isEmpty() || ( it != pluginMap.constEnd() && it.value() ) )
            { 
                loadPluginInternal( pi.pluginName() );
                if( pluginMap.isEmpty() || it == pluginMap.constEnd() )
                {
                    grp.writeEntry( pi.pluginName()+"Enabled", true );
                }
            }
        }
    }
    // Synchronize so we're writing out to the file.
    grp.sync();
}

QList<IPlugin *> PluginController::loadedPlugins() const
{
    return d->loadedPlugins.values();
}

bool PluginController::unloadPlugin( const QString & pluginId )
{
    IPlugin *thePlugin = plugin( pluginId );
    bool canUnload = d->canUnload( d->infoForId( pluginId ) );
    if( thePlugin && canUnload )
    {
        unloadPlugin(thePlugin, Later);
        return true;
    }
    return (canUnload && thePlugin);
}

void PluginController::unloadPlugin(IPlugin* plugin, PluginDeletion deletion)
{
    plugin->unload();

    //Remove the plugin from our list of plugins so we create a new
    //instance when we're asked for it again.
    //This is important to do right here, not later when the plugin really
    //vanishes. For example project re-opening might try to reload the plugin
    //and then would get the "old" pointer which will be deleted in the next
    //event loop run and thus causing crashes.
    for ( PluginControllerPrivate::InfoToPluginMap::Iterator it = d->loadedPlugins.begin();
            it != d->loadedPlugins.end(); ++it )
    {
        if ( it.value() == plugin )
        {
            d->loadedPlugins.erase( it );
            break;
        }
    }

    if (deletion == Later)
        plugin->deleteLater();
    else
        delete plugin;
}

KPluginInfo PluginController::infoForPluginId( const QString &pluginId ) const
{
    QList<KPluginInfo>::ConstIterator it;
    for ( it = d->plugins.begin(); it != d->plugins.end(); ++it )
    {
        if ( it->pluginName() == pluginId )
            return *it;
    }

    return KPluginInfo();
}

IPlugin *PluginController::loadPluginInternal( const QString &pluginId )
{
    KPluginInfo info = infoForPluginId( pluginId );
    if ( !info.isValid() )
    {
        kWarning(9501) << "Unable to find a plugin named '" << pluginId << "'!" ;
        return 0L;
    }

    // Do not load KDevKrossManager directly, its indirectly loaded when loading
    // any plugins written in a supported scripting language
    // The kross-manager plugin needs the name+interfaces of the script-plugin
    // as argument
    // At a later point in time, we should try to move the plugin's code directly
    // into shell
    if( info.pluginName() == "KDevKrossManager" )
    {
        kDebug() << "tried to load KDevKrossManager, ignoring";
        return 0;
    }

    if ( d->loadedPlugins.contains( info ) )
        return d->loadedPlugins[ info ];

    if( !hasMandatoryProperties( info ) ) {
        kWarning() << "Unable to load plugin named " << pluginId << "! Doesn't have all mandatory properties set";
        return 0;
    }

    if( info.property("X-KDevelop-Mode") == "GUI"
        && Core::self()->setupFlags() == Core::NoUi )
    {
        kWarning() << "Unable to load plugin named" << pluginId << ". Running in No-Ui mode, but the plugin says it needs a GUI";
        return 0;
    }

    kDebug() << "Attempting to load '" << pluginId << "'";
    emit loadingPlugin( info.pluginName() );
    QString str_error;
    IPlugin *plugin = 0;
    QStringList missingInterfaces;
    kDebug() << "Checking... " << info.name();
    if ( checkForDependencies( info, missingInterfaces ) )
    {
        QVariant prop = info.property( "X-KDevelop-PluginType" );
        kDebug() << "Checked... starting to load:" << info.name() << "type:" << prop;
        if(prop.toString()=="Kross")
        {
            kDebug() << "it is a kross plugin!!";
            QStringList interfaces=info.property( "X-KDevelop-Interfaces" ).toStringList();
            plugin = KServiceTypeTrader::createInstanceFromQuery<IPlugin>( QLatin1String( "KDevelop/Plugin" ),
                            QString::fromLatin1( "[X-KDE-PluginInfo-Name]=='KDevKrossManager'" ),
                            d->core, QVariantList() << interfaces << info.pluginName(), &str_error );
            kDebug() << "kross plugin:" << plugin;
        }
        else
        {
            plugin = KServiceTypeTrader::createInstanceFromQuery<IPlugin>( QLatin1String( "KDevelop/Plugin" ),
                    QString::fromLatin1( "[X-KDE-PluginInfo-Name]=='%1'" ).arg( pluginId ), d->core, QVariantList(), &str_error );
        }
        loadDependencies( info );
        loadOptionalDependencies( info );
    }

    if ( plugin )
    {
        d->loadedPlugins.insert( info, plugin );
        plugin->registerExtensions();
        info.setPluginEnabled( true );

        kDebug() << "Successfully loaded plugin '" << pluginId << "'";
        emit pluginLoaded( plugin );
    }
    else
    {
        if( str_error.isEmpty() && !missingInterfaces.isEmpty() )
        {
            kWarning() << "Can't load plugin '" << pluginId
                    << "' some of its required dependecies could not be fulfilled:" << endl
                    << missingInterfaces.join(",") << endl;
        }
        else
        {
            kWarning() << "Loading plugin '" << pluginId
                << "' failed, KPluginLoader reported error: '" << endl <<
                str_error << "'";
        }
    }

    return plugin;
}


IPlugin* PluginController::plugin( const QString& pluginId )
{
    KPluginInfo info = infoForPluginId( pluginId );
    if ( !info.isValid() )
        return 0L;

    if ( d->loadedPlugins.contains( info ) )
        return d->loadedPlugins[ info ];
    else
        return 0L;
}

///@todo plugin load operation should be O(n)
bool PluginController::checkForDependencies( const KPluginInfo& info, QStringList& missing ) const
{
    QVariant prop = info.property( "X-KDevelop-IRequired" );
    bool result = true;
    if( prop.canConvert<QStringList>() )
    {
        QStringList deps = prop.toStringList();
        foreach( const QString &iface, deps )
        {
            KPluginInfo::List l = queryPlugins( QString("'%1' in [X-KDevelop-Interfaces]").arg(iface) );
            if( l.isEmpty() )
            {
                result = false;
                missing << iface;
            }
        }
    }
    return result;
}

void PluginController::loadOptionalDependencies( const KPluginInfo& info )
{
    QVariant prop = info.property( "X-KDevelop-IOptional" );
    if( prop.canConvert<QStringList>() )
    {
        QStringList deps = prop.toStringList();
        foreach( const QString &iface, deps )
        {
            KPluginInfo info = queryPlugins( QString("'%1' in [X-KDevelop-Interfaces]").arg(iface) ).first();
            if( !loadPluginInternal( info.pluginName() ) )
	    {
		    kDebug() << "Couldn't load optional dependecy:" << iface << info.pluginName();
	    }
        }
    }
}

void PluginController::loadDependencies( const KPluginInfo& info )
{
    QVariant prop = info.property( "X-KDevelop-IRequired" );
    if( prop.canConvert<QStringList>() )
    {
        QStringList deps = prop.toStringList();
        foreach( const QString &iface, deps )
        {
            KPluginInfo info = queryPlugins( QString("'%1' in [X-KDevelop-Interfaces]").arg(iface) ).first();
            loadPluginInternal( info.pluginName() );
        }
    }
}

IPlugin* PluginController::pluginForExtension( const QString& extension, const QString& pluginname)
{
    kDebug() << "Loading Plugin ("<< pluginname << ") for Extension:" << extension;
    QStringList constraints;
    if (!pluginname.isEmpty())
        constraints << QString("[X-KDE-PluginInfo-Name]=='%1'").arg( pluginname );

    return pluginForExtension(extension, constraints);
}

IPlugin *PluginController::pluginForExtension(const QString &extension, const QStringList &constraints)
{
    kDebug() << "Finding Plugin for Extension:" << extension << "|" << constraints;
    KPluginInfo::List infos = queryExtensionPlugins(extension, constraints);

    if( infos.isEmpty() )
        return 0;
    if( d->loadedPlugins.contains( infos.first() ) )
        return d->loadedPlugins[ infos.first() ];
    else
        return loadPluginInternal( infos.first().pluginName() );
}

QList<IPlugin*> PluginController::allPluginsForExtension(const QString &extension, const QStringList &constraints)
{
    kDebug() << "Finding all Plugins for Extension:" << extension << "|" << constraints;
    KPluginInfo::List infos = queryExtensionPlugins(extension, constraints);
    QList<IPlugin*> plugins;
    foreach (const KPluginInfo &info, infos)
    {
        IPlugin* plugin;
        if( d->loadedPlugins.contains( info ) )
            plugin = d->loadedPlugins[ info ];
        else
            plugin = loadPluginInternal( info.pluginName() );

        if (plugin)
            plugins << plugin;
        else
            kWarning(9501) << "Null plugin retrieved! Loading error?";
    }
    return plugins;
}

QExtensionManager* PluginController::extensionManager()
{
    return d->m_manager;
}

QStringList PluginController::allPluginNames()
{
    QStringList names;
    Q_FOREACH( const KPluginInfo& info , d->plugins )
    {
        names << info.pluginName();
    }
    return names;
}

QList<ContextMenuExtension> PluginController::queryPluginsForContextMenuExtensions( KDevelop::Context* context ) const
{
    QList<ContextMenuExtension> exts;
    Q_FOREACH( const KPluginInfo& info, d->loadedPlugins.keys() )
    {
        IPlugin* plug = d->loadedPlugins[info];
        exts << plug->contextMenuExtension( context );
    }
    return exts;
}

QStringList PluginController::projectPlugins()
{
    QStringList names;
    Q_FOREACH( const KPluginInfo& info , d->plugins )
    {
        if( info.property("X-KDevelop-Category").toString() == "Project" )
            names << info.pluginName();
    }
    return names;
}

void PluginController::loadProjectPlugins()
{
    Q_FOREACH( const QString& name, projectPlugins() )
    {
        loadPluginInternal( name );
    }
}

void PluginController::unloadProjectPlugins()
{
    Q_FOREACH( const QString& name, projectPlugins() )
    {
        unloadPlugin( name );
    }
}

QList<KPluginInfo> PluginController::allPluginInfos() const
{
    return d->plugins;
}

void PluginController::updateLoadedPlugins()
{
    QStringList defaultPlugins = ShellExtension::getInstance()->defaultPlugins();
    KConfigGroup grp = Core::self()->activeSession()->config()->group( pluginControllerGrp );
    foreach( const KPluginInfo& info, d->plugins )
    {
        if( isGlobalPlugin( info ) )
        {
            bool enabled = grp.readEntry( info.pluginName()+"Enabled", ( defaultPlugins.isEmpty() || defaultPlugins.contains( info.pluginName() ) ) );
            if( d->loadedPlugins.contains( info ) && !enabled ) 
            {
                kDebug() << "unloading" << info.pluginName();
                if( !unloadPlugin( info.pluginName() ) ) 
                {
                    grp.writeEntry( info.pluginName()+"Enabled", false );
                }
            } else if( !d->loadedPlugins.contains( info ) && enabled ) 
            {
                loadPluginInternal( info.pluginName() );
            }
        }
    }
}

void PluginController::resetToDefaults()
{
    KSharedConfig::Ptr cfg = Core::self()->activeSession()->config();
    cfg->deleteGroup( pluginControllerGrp );
    cfg->sync();
    KConfigGroup grp = cfg->group( pluginControllerGrp );
    QStringList plugins = ShellExtension::getInstance()->defaultPlugins();
    if( plugins.isEmpty() ) 
    {
        foreach( const KPluginInfo& info, d->plugins )
        {
            plugins << info.pluginName();
        }
    }
    foreach( const QString& s, plugins )
    {
        grp.writeEntry( s+"Enabled", true );
    }
    grp.sync();
}

}
#include "plugincontroller.moc"

