/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktllogiccomponentsplugin.h"

#include "interfaces/component/icomponent.h"
#include "interfaces/component/icomponentplugin.h"
#include "interfaces/simulator/isimulationmanager.h"
#include "interfaces/idocumentplugin.h"

#include <circuit/simulator/genericelementfactory.h>
#include <shell/core.h>
#include <KGenericFactory>
#include <KAboutData>
#include <KStandardDirs>
#include <circuit/genericcomponentitemfactory.h>
#include <interfaces/iplugincontroller.h>

using namespace KTechLab;

K_PLUGIN_FACTORY(KTLLogicComponentsPluginFactory, registerPlugin<KTLLogicComponentsPlugin>(); )
K_EXPORT_PLUGIN(KTLLogicComponentsPluginFactory(
    KAboutData("ktllogic","ktllogic", ki18n("KTechLab Logic Components"),
               "0.1", ki18n("Provide a set of logic components"),
               KAboutData::License_LGPL))
)


class KTechLab::KTLLogicFactory: public GenericComponentItemFactory, public GenericElementFactory
{
public:
    KTLLogicFactory()
    {
        QString file;
        file = KGlobal::dirs()->findResource("data","ktechlab/components/ktllogic_components.rc");
        kDebug() << "Found component meta-data file: " << file;
        loadComponentsFromFile( file );
    }

protected:
    virtual IElement * createOrRegister(bool /* create */ , const QByteArray& /* type */,
                                        QVariantMap /* parentInModel */ = QVariantMap())
    {
        return 0;
    }

};


KTLLogicComponentsPlugin::KTLLogicComponentsPlugin(QObject* parent, const QVariantList& /* args */)
    :   IComponentPlugin( KTLLogicComponentsPluginFactory::componentData(), parent ),
        m_componentFactory( new KTLLogicFactory() )
{

    init();
}

IDocumentPlugin* KTLLogicComponentsPlugin::documentPlugin() const
{
    QStringList constraints;
    constraints << QString("'%1' in [X-KDevelop-SupportedMimeTypes]").arg("application/x-circuit");
    QList<KDevelop::IPlugin*> plugins = KDevelop::Core::self()->pluginController()->allPluginsForExtension( "org.kdevelop.IDocument", constraints );
    if (plugins.isEmpty()) {
        kWarning() << "No plugin found to load KTechLab Documents";
        return 0;
    }
    // note: IDocumentPlugin is not a QObject, so qobject_cast won't work here
    // IDocumentPlugin *plugin = qobject_cast<IDocumentPlugin*>( plugins.first() );
    IDocumentPlugin *plugin = dynamic_cast<IDocumentPlugin*>( plugins.first() );
    return plugin;
}

void KTLLogicComponentsPlugin::init()
{
    IDocumentPlugin *plugin = documentPlugin();
    if (!plugin) {
      return;
    }
    plugin->registerComponentFactory( m_componentFactory );
    ISimulationManager::self()->registerElementFactory(m_componentFactory);
}

KTLLogicComponentsPlugin::~KTLLogicComponentsPlugin()
{
    delete m_componentFactory;
}

void KTLLogicComponentsPlugin::unload()
{
    IDocumentPlugin *plugin = documentPlugin();
    if (plugin) {
      plugin->deregisterComponentFactory( m_componentFactory );
    }

    ISimulationManager::self()->unregisterElementFactory(m_componentFactory);
}

#include "ktllogiccomponentsplugin.moc"

// vim: sw=4 sts=4 et tw=100
