/* This file is part of the KDE project

Copyright 2009 Julian Bäume <julian@svg4all.de>

Based on code from kdevplatform
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

#include <QFileInfo>
#include <QExtensionManager>
#include <KDebug>
#include <KStandardDirs>

#include <interfaces/contextmenuextension.h>
#include <interfaces/iplugin.h>
#include <interfaces/isession.h>
#include <interfaces/idebugcontroller.h>
#include <shell/core.h>
#include <shell/shellextension.h>

namespace KTechLab
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
    typedef QMap<KPluginInfo, KDevelop::IPlugin*> InfoToPluginMap;
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
        if(interfaces.contains("ILanguageSupport")) {
            // Language supports can not be unloaded currently, mainly due to LanguageController and associated languages/mimetypes
            // See: https://bugs.kde.org/show_bug.cgi?id=194337
            return false; 
        }

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

    KDevelop::Core *core;
    QExtensionManager* m_manager;
};

PluginController::PluginController(KDevelop::Core *core)
    : KDevelop::PluginController(core), d(new PluginControllerPrivate)
{
    setObjectName("PluginController");
    d->core = core;
    kDebug() << "Fetching plugin info which matches:" << QString( "[X-KDevelop-Version] == %1" ).arg(KDEVELOP_PLUGIN_VERSION);
    d->plugins = KPluginInfo::fromServices( KServiceTypeTrader::self()->query( QLatin1String( "KTechLab/Plugin" ),
        QString( "[X-KDevelop-Version] == %1" ).arg(KDEVELOP_PLUGIN_VERSION) ) );
    d->cleanupMode = PluginControllerPrivate::Running;
    d->m_manager = new QExtensionManager();
    // Register the KDevelop::IPlugin* metatype so we can properly unload it
    qRegisterMetaType<KDevelop::IPlugin*>( "KDevelop::IPlugin*" );
}

PluginController::~PluginController()
{
    if ( d->cleanupMode != PluginControllerPrivate::CleanupDone ) {
        kWarning(9501) << "Destructing plugin controller without going through the shutdown process! Backtrace is: "
                       << endl << kBacktrace() << endl;
    }

    delete d->m_manager;
    delete d;
}


KDevelop::IPlugin* PluginController::plugin( const QString& pluginId )
{
    KPluginInfo info = infoForPluginId( pluginId );
    if ( !info.isValid() )
        return 0;

    if ( d->loadedPlugins.contains( info ) )
        return d->loadedPlugins[ info ];
    else
        return 0;
}


KDevelop::IPlugin* PluginController::loadPlugin( const QString& pluginName )
{
    return loadPluginInternal( pluginName );
}

bool PluginController::unloadPlugin( const QString & pluginId )
{
    KDevelop::IPlugin *thePlugin = plugin( pluginId );
    bool canUnload = d->canUnload( d->infoForId( pluginId ) );
    if( thePlugin && canUnload )
    {
        unloadPlugin(thePlugin, Later);
        return true;
    }
    return (canUnload && thePlugin);
}

void PluginController::unloadPlugin(KDevelop::IPlugin* plugin, PluginDeletion deletion)
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

QList<KDevelop::IPlugin *> PluginController::loadedPlugins() const
{
    return d->loadedPlugins.values();
}

QStringList PluginController::allPluginNames() const
{
    QStringList names;
    Q_FOREACH( const KPluginInfo& info , d->plugins )
    {
        names << info.pluginName();
    }
    return names;
}

KPluginInfo PluginController::infoForPluginId( const QString &pluginId ) const
{
    foreach (const KPluginInfo &info, d->plugins) {
        if (info.pluginName() == pluginId ) {
            return info;
        }
    }
    return KPluginInfo();
}

KDevelop::IPlugin *PluginController::loadPluginInternal( const QString &pluginId )
{
    KPluginInfo info = infoForPluginId( pluginId );
    if ( !info.isValid() )
    {
        kWarning(9501) << "Unable to find a plugin named '" << pluginId << "'!" ;
        return 0L;
    }

    if ( d->loadedPlugins.contains( info ) )
        return d->loadedPlugins[ info ];

    if( !isEnabled( info ) )
    {
        // Do not load disabled plugins
        kWarning() << "Not loading plugin named" << pluginId << "because its been disabled!";
        return 0;
    }

    if( !hasMandatoryProperties( info ) ) {
        kWarning() << "Unable to load plugin named " << pluginId << "! Doesn't have all mandatory properties set";
        return 0;
    }

    if( info.property("X-KDevelop-Mode") == "GUI"
        && KDevelop::Core::self()->setupFlags() == KDevelop::Core::NoUi )
    {
        kDebug() << "Unable to load plugin named" << pluginId << ". Running in No-Ui mode, but the plugin says it needs a GUI";
        return 0;
    }

    bool isKrossPlugin = false;
    QString krossScriptFile;
    if( info.property("X-KDevelop-PluginType").toString() == "Kross" ) 
    {
        isKrossPlugin = true;
        krossScriptFile = KStandardDirs::locate( "appdata", info.service()->library(), KComponentData("kdevkrossplugins"));
        if( krossScriptFile.isEmpty() || !QFileInfo( krossScriptFile ).exists() || !QFileInfo( krossScriptFile ).isReadable() )
        {
            kWarning() << "Unable to load kross plugin" << pluginId << ". Script file" << krossScriptFile << "not found or not readable";
            return 0;
        }
    }

    kDebug() << "Attempting to load '" << pluginId << "'";
    emit loadingPlugin( info.pluginName() );
    QString str_error;
    KDevelop::IPlugin *plugin = 0;
    QStringList missingInterfaces;
    kDebug() << "Checking... " << info.name();
    if ( checkForDependencies( info, missingInterfaces ) )
    {
        QVariant prop = info.property( "X-KDevelop-PluginType" );
        kDebug() << "Checked... starting to load:" << info.name() << "type:" << prop;

        QString failedPlugin;
        if( !loadDependencies( info, failedPlugin ) ) 
        {
            kWarning() << "Could not load a required dependency:" << failedPlugin;
            return 0;
        }
        loadOptionalDependencies( info );

        if( isKrossPlugin )
        {
            // Kross is special, we create always the same "plugin" which hooks up
            // the script and makes the connection between C++ and script side
            kDebug() << "it is a kross plugin!!";
            // Workaround for KAboutData constructor needing a KLocalizedString and 
            // KLocalized string storing the char* for later usage
            QString tmp = info.name();
            int len = tmp.toUtf8().size();
            char* name = new char[len+1];
            memcpy( name, tmp.toUtf8().data(), len );
            name[len] = '\0';
            tmp = info.comment();
            len = tmp.toUtf8().size();
            char* comment = new char[len+1];
            memcpy( comment, tmp.toUtf8().data(), len );
            comment[len] = '\0';
            // Create the kross plugin instance from the desktop file data.
/*
            plugin = new KrossPlugin( krossScriptFile, KAboutData( info.pluginName().toUtf8(), info.pluginName().toUtf8(),
                              ki18n( name ), info.version().toUtf8(), ki18n( comment ), KAboutLicense::byKeyword( info.license() ).key() ), d->core );
*/
            kWarning() << "KTechLab is not able to load Kross-Plugins, yet";
        }
        else
        {
            plugin = KServiceTypeTrader::createInstanceFromQuery<KDevelop::IPlugin>( QLatin1String( "KTechLab/Plugin" ),
                    QString::fromLatin1( "[X-KDE-PluginInfo-Name]=='%1'" ).arg( pluginId ), d->core, QVariantList(), &str_error );
        }
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
                    << "' some of its required dependencies could not be fulfilled:" << endl
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

bool PluginController::loadDependencies( const KPluginInfo& info, QString& failedPlugin )
{
    QVariant prop = info.property( "X-KDevelop-IRequired" );
    QStringList loadedPlugins;
    if( prop.canConvert<QStringList>() )
    {
        QStringList deps = prop.toStringList();
        foreach( const QString &iface, deps )
        {
            KPluginInfo info = queryPlugins( QString("'%1' in [X-KDevelop-Interfaces]").arg(iface) ).first();
            if( !loadPluginInternal( info.pluginName() ) )
            {
                foreach( const QString& name, loadedPlugins )
                {
                    unloadPlugin( name );
                }
                failedPlugin = info.pluginName();
                return false;
            }
            loadedPlugins << info.pluginName();
        }
    }
    return true;
}

bool PluginController::isEnabled( const KPluginInfo& info )
{
    KConfigGroup grp = KDevelop::Core::self()->activeSession()->config()->group( pluginControllerGrp );
    bool isEnabled = grp.readEntry( info.pluginName()+"Enabled", KDevelop::ShellExtension::getInstance()->defaultPlugins().isEmpty() || KDevelop::ShellExtension::getInstance()->defaultPlugins().contains( info.pluginName() ) );
    //kDebug() << "read config:" << isEnabled << "is global plugin:" << isGlobalPlugin( info ) << "default:" << ShellExtension::getInstance()->defaultPlugins().isEmpty()  << ShellExtension::getInstance()->defaultPlugins().contains( info.pluginName() );
    return !isGlobalPlugin( info ) || isEnabled;
}

KDevelop::IPlugin* PluginController::pluginForExtension( const QString& extension, const QString& pluginname)
{
    //kDebug() << "Loading Plugin ("<< pluginname << ") for Extension:" << extension;
    QStringList constraints;
    if (!pluginname.isEmpty())
        constraints << QString("[X-KDE-PluginInfo-Name]=='%1'").arg( pluginname );

    return pluginForExtension(extension, constraints);
}

KDevelop::IPlugin *PluginController::pluginForExtension(const QString &extension, const QStringList &constraints)
{
    //kDebug() << "Finding Plugin for Extension:" << extension << "|" << constraints;
    KPluginInfo::List infos = queryExtensionPlugins(extension, constraints);

    if( infos.isEmpty() )
        return 0;
    if( d->loadedPlugins.contains( infos.first() ) )
        return d->loadedPlugins[ infos.first() ];
    else
        return loadPluginInternal( infos.first().pluginName() );
}

}
#include "plugincontroller.moc"

