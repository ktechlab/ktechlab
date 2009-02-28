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
#include "shell/core.h"

#include <interfaces/idocumentcontroller.h>
#include <KGenericFactory>
#include <KAboutData>

#include <QPushButton>

K_PLUGIN_FACTORY(KTLCircuitFactory, registerPlugin<KTLCircuitPlugin>(); )
K_EXPORT_PLUGIN(KTLCircuitFactory(KAboutData("ktlcircuit","ktlcircuit", ki18n("KTechLab Circuits"), "0.1", ki18n("Managing, viewing, manipulating circuit files"), KAboutData::License_LGPL)))

class KTLCircuitDocumentFactory: public KDevelop::IDocumentFactory
{
public:
    KTLCircuitDocumentFactory( KTLCircuitPlugin *plugin )
        : m_plugin(plugin)
    {};

    virtual KDevelop::IDocument * create( const KUrl &url, KDevelop::ICore *core )
    {
        KTechLab::Core *ktlCore = dynamic_cast<KTechLab::Core*>(core);
        if ( ktlCore )
            return new CircuitDocument( url, ktlCore );

        return 0;
    }
private:
    KTLCircuitPlugin * m_plugin;
};

KTLCircuitPlugin::KTLCircuitPlugin( QObject *parent, const QVariantList& args )
    : KDevelop::IPlugin( KTLCircuitFactory::componentData(), parent )
{

    init();
}

void KTLCircuitPlugin::init()
{
    m_documentFactory = new KTLCircuitDocumentFactory(this);
    KTechLab::Core::self()->documentController()->registerDocumentForMimetype( "application/x-circuit", m_documentFactory );
}

KTLCircuitPlugin::~KTLCircuitPlugin()
{
}

void KTLCircuitPlugin::unload()
{
    delete m_documentFactory;
}

#include "ktlcircuitplugin.moc"

// vim: sw=4 sts=4 et tw=100
