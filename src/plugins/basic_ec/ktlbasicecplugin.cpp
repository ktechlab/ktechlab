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
#include "elements/voltagesource.h"

#include <interfaces/component/icomponent.h>
#include <interfaces/component/icomponentplugin.h>
#include <interfaces/simulator/isimulationmanager.h>
#include <interfaces/idocumentplugin.h>
#include <circuit/simulator/genericelementfactory.h>

#include <shell/core.h>
#include <KGenericFactory>
#include <KAboutData>
#include <KStandardDirs>

using namespace KTechLab;

K_PLUGIN_FACTORY(KTLBasicECPluginFactory, registerPlugin<KTLBasicECPlugin>(); )
K_EXPORT_PLUGIN(KTLBasicECPluginFactory(KAboutData("ktlbasic_ec","ktlbasic_ec", ki18n("KTechLab Basic Electronic Components"), "0.1", ki18n("Provide a set of basic electronic components"), KAboutData::License_LGPL)))


class KTechLab::KTLBasicECFactory: public IComponentFactory, public GenericElementFactory
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

protected:
    virtual IElement * createOrRegister(bool create, const QByteArray& type,
                                        QVariantMap parentInModel = QVariantMap())
    {
        if (create) {
            parentInModel.insert("pinList", pinListForComponent(type));
        }
        SUPPORT_ELEMENT(Resistance,"ec/resistor")
        SUPPORT_ELEMENT(Capacitance,"ec/capacitor")
        SUPPORT_ELEMENT(VoltageSource,"ec/voltagesource")
        return 0;
    }
private:
    QStringList pinListForComponent(const QByteArray& type)
    {
        QStringList result;
        result << QString::fromAscii("n1");
        result << QString::fromAscii("p1");
        return result;
    }
};

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
    ISimulationManager::self()->registerElementFactory(m_componentFactory);
}

KTLBasicECPlugin::~KTLBasicECPlugin()
{
    delete m_componentFactory;
}

void KTLBasicECPlugin::unload()
{
    IDocumentPlugin *plugin = documentPlugin();
    if (plugin) {
      plugin->deregisterComponentFactory( m_componentFactory );
    }

    ISimulationManager::self()->unregisterElementFactory(m_componentFactory);
}

#include "ktlbasicecplugin.moc"

// vim: sw=4 sts=4 et tw=100
