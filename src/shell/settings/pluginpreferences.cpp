/* KDevelop Project Settings
 *
 * Copyright 2008 Andreas Pakulat <apaku@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */
#include "pluginpreferences.h"

#include <QtGui/QVBoxLayout>

#include <kgenericfactory.h>
#include <kaboutdata.h>
#include <kpluginselector.h>
#include <kplugininfo.h>
#include <ksettings/dispatcher.h>

#include <interfaces/isession.h>

#include "../core.h"
#include "../plugincontroller.h"


namespace KDevelop
{

K_PLUGIN_FACTORY(PluginPreferencesFactory, registerPlugin<PluginPreferences>();)
K_EXPORT_PLUGIN(PluginPreferencesFactory( KAboutData("kcm_kdev_pluginsettings", "kdevplatform", ki18n("Plugin Selection"), "0.1")))


PluginPreferences::PluginPreferences( QWidget *parent, const QVariantList &args )
 : KCModule( PluginPreferencesFactory::componentData(), parent, args )
{
    QVBoxLayout* lay = new QVBoxLayout(this );
    selector = new KPluginSelector( this );
    lay->addWidget( selector );
    QList<KPluginInfo> plugins;
    foreach( KPluginInfo info, Core::self()->pluginControllerInternal()->allPluginInfos() )
    {
        if( info.property("X-KDevelop-Category") == "Global" )
        {
            plugins << info;
        }
    }
    selector->addPlugins( plugins, KPluginSelector::ReadConfigFile, QString(), QString(), Core::self()->activeSession()->config() );
    connect( selector, SIGNAL( changed(bool) ), this, SLOT( changed() ) );
    connect( selector, SIGNAL( configComitted(const QByteArray&) ), this, SLOT( reparseConfig(const QByteArray&) ) );
    selector->load();
}

void PluginPreferences::reparseConfig( const QByteArray& conf )
{
    KSettings::Dispatcher::reparseConfiguration( conf );
}

void PluginPreferences::defaults()
{
    Core::self()->pluginControllerInternal()->resetToDefaults();
    selector->load();
    KCModule::defaults();
}

void PluginPreferences::save()
{
    selector->save();
    KCModule::save();
    Core::self()->pluginControllerInternal()->updateLoadedPlugins();
}

void PluginPreferences::load()
{
    selector->load();
    KCModule::load();
}

}

#include "pluginpreferences.moc"

