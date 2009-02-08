/* This file is part of the KDE project
Copyright 2007 Andreas Pakulat <apaku@gmx.de>
Copyright 2004, 2007 Alexander Dymo <adymo@kdevelop.org>
Copyright 2006 Matt Rogers <mattr@kde.org

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
#ifndef KDEVPLUGINCONTROLLER_H
#define KDEVPLUGINCONTROLLER_H


#include <QtCore/QHash>

#include <KDE/KUrl>
#include <KDE/KService>
#include <KDE/KPluginInfo>
#include <KDE/KServiceTypeTrader>

#include <interfaces/iplugincontroller.h>

#include "shellexport.h"


namespace KDevelop
{
class Core;
class CorePrivate;
class IPlugin;
class PluginControllerPrivate;
/**
 * The KDevelop plugin controller.
 * The Plugin controller is responsible for querying, loading and unloading
 * available plugins.
 */
class KDEVPLATFORMSHELL_EXPORT PluginController: public IPluginController
{

    Q_OBJECT
friend class Core;
friend class CorePrivate;

public:

    PluginController(Core *core);

    virtual ~PluginController();

    /**
     * Get the plugin instance based on the ID. The ID should be whatever is
     * in X-KDE-PluginInfo-Name
     */
    IPlugin* plugin( const QString& );

    /**
     * Get the plugin info for a loaded plugin
     */
    KPluginInfo pluginInfo( const IPlugin* ) const;

    /**
     * Get a list of currently loaded plugins
     */
    QList<IPlugin*> loadedPlugins() const;

    /**
     * Returns a uniquely specified plugin. If it isn't already loaded, it will be.
     * @param pluginName the name of the plugin, as given in the X-KDE-PluginInfo-Name property
     * @returns a pointer to the plugin instance or 0
     */
    IPlugin * loadPlugin( const QString & pluginName );

    /**
     * @brief Unloads the plugin specified by @p plugin
     *
     * @param plugin The name of the plugin as specified by the
     * X-KDE-PluginInfo-Name key of the .desktop file for the plugin
     */
    bool unloadPlugin( const QString & plugin );

    enum PluginDeletion {
        Now,
        Later
    };

    /**
     * retrieve all plugin infos
     */
    QList<KPluginInfo> allPluginInfos() const;

    /**
     * loads not-yet-loaded plugins and unloads plugins
     * depending on the configuration in the session\
     */
    void updateLoadedPlugins();


    /**
     * Directly unload the given \a plugin, either deleting it now or \a deletion.
     *
     * \param plugin plugin to unload
     * \param deletion if true, delete the plugin later, if false, delete it now.
     */
    void unloadPlugin(IPlugin* plugin, PluginDeletion deletion);

    /**
     * Queries for the plugin which supports given extension interface.
     * All already loaded plugins will be queried and the first one to support the extension interface
     * will be returned. Any plugin can be an extension, only "ServiceTypes=..." entry is
     * required in .desktop file for that plugin.
     * @param extension The extension interface
     * @param pluginname The name of the plugin to load if multiple plugins for the extension exist, corresponds to the X-KDE-PluginInfo-Name
     * @return A KDevelop extension plugin for given service type or 0 if no plugin supports it
     */
    IPlugin *pluginForExtension(const QString &extension, const QString &pluginname = "");
    IPlugin *pluginForExtension(const QString &extension, const QStringList &constraints);

    QList<IPlugin*> allPluginsForExtension(const QString &extension, const QStringList &constraints = QStringList());

    QExtensionManager* extensionManager();

    QStringList allPluginNames();

    QList<ContextMenuExtension> queryPluginsForContextMenuExtensions( KDevelop::Context* context ) const;

    QStringList projectPlugins();

    void loadProjectPlugins();
    void unloadProjectPlugins();

    void resetToDefaults();


private:

    /**
     * @internal
     *
     * The internal method for loading plugins.
     * Called by @ref loadPlugin directly or through the queue for async plugin
     * loading.
     */
    IPlugin* loadPluginInternal( const QString &pluginId );

    /**
     * @internal
     *
     * Find the KPluginInfo structure by key. Reduces some code duplication.
     *
     * Returns a null pointer when no plugin info is found.
     */
    KPluginInfo infoForPluginId( const QString &pluginId ) const;

    bool checkForDependencies( const KPluginInfo& info, QStringList& missing ) const;

    void loadDependencies( const KPluginInfo& );
    void loadOptionalDependencies( const KPluginInfo& info );

    void cleanup();
    void initialize();

private:
    class PluginControllerPrivate* const d;
};

}
#endif


