/***************************************************************************
 *   Copyright (C) 2009 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktlbasicecplugin.h"
#include "elements/resistance.h"
#include "elements/capacitance.h"

#include "interfaces/component/icomponent.h"
#include "interfaces/component/icomponentplugin.h"
#include "interfaces/isimulationmanager.h"
#include "interfaces/idocumentplugin.h"
#include "interfaces/genericelementfactory.h"

#include <shell/core.h>
#include <KGenericFactory>
#include <KAboutData>
#include <KStandardDirs>

using namespace KTechLab;

K_PLUGIN_FACTORY(KTLBasicECPluginFactory, registerPlugin<KTLBasicECPlugin>(); )
K_EXPORT_PLUGIN(KTLBasicECPluginFactory(KAboutData("ktlbasic_ec","ktlbasic_ec", ki18n("KTechLab Basic Electronic Components"), "0.1", ki18n("Provide a set of basic electronic components"), KAboutData::License_LGPL)))


class KTechLab::KTLBasicECFactory: public IComponentFactory
{
public:
    KTLBasicECFactory()
    {
        QString file;
        file = KGlobal::dirs()->findResource("data","ktechlab/components/ktlbasic_ec.rc");
        kDebug() << "Found component meta-data file: " << file;
        loadComponentsFromFile( file );
    }

    virtual IComponent * create( const QString &name )
    {
        return 0;
    }

};

DECLARE_ELEMENT_FACTORY(
    BasicElementFactory,
    SUPPORT_ELEMENT(Resistance)
    SUPPORT_ELEMENT(Capacitance)
    );


KTLBasicECPlugin::KTLBasicECPlugin( QObject *parent, const QVariantList& args )
    :   IComponentPlugin( KTLBasicECPluginFactory::componentData(), parent ),
        m_componentFactory( new KTLBasicECFactory() )
{

    init();
}

void KTLBasicECPlugin::init()
{
    IDocumentPlugin *plugin = documentPlugin();
    if (!plugin) {
      return;
    }
    plugin->registerComponentFactory( m_componentFactory );
    m_basicElementFactory = new BasicElementFactory();
    ISimulationManager::self()->registerElementFactory(m_basicElementFactory);
}

KTLBasicECPlugin::~KTLBasicECPlugin()
{
    delete m_componentFactory;
    delete m_basicElementFactory;
}

void KTLBasicECPlugin::unload()
{
    //me be, we should unregister our components at the circuit-plugin
    ISimulationManager::self()->unregisterElementFactory(m_basicElementFactory);
}

#include "ktlbasicecplugin.moc"

// vim: sw=4 sts=4 et tw=100
