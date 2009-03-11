/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktlpassiveplugin.h"

#include "resistor.h"
#include "shell/core.h"
#include "interfaces/component/icomponent.h"
#include "interfaces/component/icomponentplugin.h"
#include "interfaces/idocumentplugin.h"

#include <interfaces/iplugincontroller.h>
#include <KGenericFactory>
#include <KAboutData>

K_PLUGIN_FACTORY(KTLPassivePluginFactory, registerPlugin<KTLPassivePlugin>(); )
K_EXPORT_PLUGIN(KTLPassivePluginFactory(KAboutData("ktlpassive","ktlpassive", ki18n("KTechLab Passive Components"), "0.1", ki18n("Provide a set of standard passive components"), KAboutData::License_LGPL)))


class KTLPassiveFactory: public KTechLab::IComponentFactory
{
public:
    KTLPassiveFactory()
    {
        addSupportedComponent( Resistor::metaData() );
    }

    virtual KTechLab::IComponent * create( const QString &name )
    {
        return 0;
    }

};

KTLPassivePlugin::KTLPassivePlugin( QObject *parent, const QVariantList& args )
    :   KTechLab::IComponentPlugin( KTLPassivePluginFactory::componentData(), parent ),
        m_componentFactory( new KTLPassiveFactory() )
{

    init();
}

void KTLPassivePlugin::init()
{
    QStringList constraints;
    constraints << QString("'%1' in [X-KDevelop-SupportedMimeTypes]").arg("application/x-circuit");
    QList<KDevelop::IPlugin*> plugins = KTechLab::Core::self()->pluginController()->allPluginsForExtension( "KTLDocument", constraints );
    if (plugins.isEmpty()) {
        return;
    }
    KTechLab::IDocumentPlugin *plugin = qobject_cast<KTechLab::IDocumentPlugin*>( plugins.first() );

    plugin->registerComponentFactory( m_componentFactory );
}

KTLPassivePlugin::~KTLPassivePlugin()
{
}

void KTLPassivePlugin::unload()
{
    //me be, we should unregister our components at the circuit-plugin
}

#include "ktlpassiveplugin.moc"

// vim: sw=4 sts=4 et tw=100
