#include "ktlcircuitplugin.h"

#include <KGenericFactory>
#include <KAboutData>

K_PLUGIN_FACTORY(KTLCircuitFactory, registerPlugin<KTLCircuitPlugin>(); )
K_EXPORT_PLUGIN(KTLCircuitFactory(KAboutData("ktlcircuit","ktlcircuit", ki18n("KTechLab Circuits"), "0.1", ki18n("Managing, viewing, manipulating circuit files"), KAboutData::License_LGPL)))

KTLCircuitPlugin::KTLCircuitPlugin( QObject *parent, const QVariantList& args )
    : KDevelop::IPlugin( KTLCircuitFactory::componentData(), parent )
{
}

KTLCircuitPlugin::~KTLCircuitPlugin()
{
}

void KTLCircuitPlugin::unload()
{
}

#include "ktlcircuitplugin.moc"

// vim: sw=4 sts=4 et tw=100
