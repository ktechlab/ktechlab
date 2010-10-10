/***************************************************************************
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktlcircuitplugin.h"

#include "circuitdocument.h"
#include "interfaces/component/componentmodel.h"
#include "interfaces/component/icomponentplugin.h"
#include "interfaces/component/icomponent.h"
#include "shell/core.h"

#include <interfaces/iuicontroller.h>
#include <interfaces/idocumentcontroller.h>
#include <KGenericFactory>
#include <KAboutData>
#include <KDebug>
#include <QTreeView>
#include <QHeaderView>

using namespace KTechLab;

K_PLUGIN_FACTORY(KTLCircuitFactory, registerPlugin<KTLCircuitPlugin>(); )
K_EXPORT_PLUGIN(KTLCircuitFactory(KAboutData("ktlcircuit","ktlcircuit", ki18n("KTechLab Circuits"), "0.1", ki18n("Managing, viewing, manipulating circuit files"), KAboutData::License_LGPL)))

class KTechLab::KTLComponentViewFactory: public KDevelop::IToolViewFactory
{
public:
    KTLComponentViewFactory( KTLCircuitPlugin *plugin )
        : m_plugin(plugin)
    {};

    virtual QWidget * create( QWidget *parent )
    {
        QTreeView *componentView = new QTreeView( parent );
        componentView->setModel( m_plugin->componentModel() );
        componentView->header()->hide();
        componentView->setAcceptDrops( false );
        componentView->setDragEnabled( true );

        return componentView;
    };

    virtual QString id() const
    {
        return "org.ktechlab.ComponentView";
    };

    virtual Qt::DockWidgetArea defaultPosition()
    {
        return Qt::LeftDockWidgetArea;
    };
private:
    KTLCircuitPlugin * m_plugin;
};

class KTechLab::KTLCircuitDocumentFactory: public KDevelop::IDocumentFactory
{
public:
    KTLCircuitDocumentFactory( KTLCircuitPlugin *plugin )
        : m_plugin(plugin)
    {};

    virtual KDevelop::IDocument * create( const KUrl &url, KDevelop::ICore *core )
    {
        KDevelop::Core *ktlCore = dynamic_cast<KDevelop::Core*>(core);
        if ( ktlCore )
            return new CircuitDocument( url, ktlCore );

        return 0;
    }
private:
    KTLCircuitPlugin * m_plugin;
};

KTLCircuitPlugin::KTLCircuitPlugin( QObject *parent, const QVariantList& args )
    : KTechLab::IDocumentPlugin( KTLCircuitFactory::componentData(), parent ),
    m_componentModel( new ComponentModel() )
{

    init();
}

void KTLCircuitPlugin::init()
{
    m_componentViewFactory = new KTLComponentViewFactory(this);
    KDevelop::Core::self()->uiController()->addToolView( "Components", m_componentViewFactory );

    m_documentFactory = new KTLCircuitDocumentFactory(this);
    KDevelop::Core::self()->documentController()->registerDocumentForMimetype( "application/x-circuit", m_documentFactory );
}

KTLCircuitPlugin::~KTLCircuitPlugin()
{
    // it crashes, when we delete this. I guess,
    // it has been deleted before, somewhere else.
    //delete m_componentViewFactory;
    delete m_documentFactory;
    delete m_componentModel;
}

ComponentModel * KTLCircuitPlugin::componentModel()
{
    return m_componentModel;
}

void KTLCircuitPlugin::registerComponentFactory( IComponentFactory *factory )
{
    QList<ComponentMetaData> metaData = factory->allMetaData();
    kDebug() << "registering" << metaData.size() << "components";
    foreach (ComponentMetaData data, metaData) {
        m_componentModel->insertComponentData( data, factory );
    }
}

void KTLCircuitPlugin::deregisterComponentFactory(IComponentFactory* factory)
{
    QList<ComponentMetaData> metaData = factory->allMetaData();
    kDebug() << "deregistering" << metaData.size() << "components";
    foreach (ComponentMetaData data, metaData) {
        m_componentModel->removeComponentData( data, factory );
    }
}

void KTLCircuitPlugin::unload()
{
    KDevelop::Core::self()->uiController()->removeToolView(m_componentViewFactory);
}

#include "ktlcircuitplugin.moc"

// vim: sw=4 sts=4 et tw=100
