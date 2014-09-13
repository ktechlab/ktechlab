/***************************************************************************
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktlcircuitplugin_qt.h"

#include <QDebug>

namespace KTechLab {

KTLCircuitPluginQt::KTLCircuitPluginQt( const ::KComponentData & componentData,
                                        QObject* parent, const QVariantList& /* args */ )
    : KTechLab::IDocumentPlugin( (const ::KComponentData & ) componentData, parent )
    , m_componentModel( new ComponentModel() )
{
    m_fakeComponentItemFactory = new FakeComponentItemFactory;
    registerComponentFactory(m_fakeComponentItemFactory);
    // init();
}

KTLCircuitPluginQt::~KTLCircuitPluginQt()
{
    qDebug() << "KTLCircuitPluginQt::~KTLCircuitPluginQt";
    delete m_fakeComponentItemFactory;;
    delete m_componentModel;
}

void KTLCircuitPluginQt::unload()
{
    qWarning() << "KTLCircuitPluginQt::unload: not implemented";
}


ComponentModel * KTLCircuitPluginQt::componentModel()
{
    return m_componentModel;
}

void KTLCircuitPluginQt::registerComponentFactory( IComponentItemFactory *factory )
{
    QList<ComponentMetaData> metaData = factory->allMetaData();
    qDebug() << "registering" << metaData.size() << "components";
    foreach (ComponentMetaData data, metaData) {
        m_componentModel->insertComponentData( data, factory );
    }
}

void KTLCircuitPluginQt::deregisterComponentFactory(IComponentItemFactory* factory)
{
    QList<ComponentMetaData> metaData = factory->allMetaData();
    qDebug() << "deregistering" << metaData.size() << "components";
    foreach (ComponentMetaData data, metaData) {
        m_componentModel->removeComponentData( data, factory );
    }
}

IComponentItemFactory* KTLCircuitPluginQt::componentItemFactory(const QString& name,
                                                              Theme* /* theme */ )
{
    IComponentItemFactory* factory = m_componentModel->factoryForComponent(name);
    if (!factory) {
        qWarning() << "factory for data not found";
        return m_componentModel->factoryForComponent("ec/unknown");
    }
    return factory;
}

}


