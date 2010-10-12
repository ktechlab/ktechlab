
#ifndef KTLCIRCUITPLUGIN_H
#define KTLCIRCUITPLUGIN_H

#include "interfaces/idocumentplugin.h"

#include <QVariantList>

namespace KTechLab
{
class IComponentFactory;

class KTLComponentViewFactory;
class KTLCircuitDocumentFactory;
class ComponentModel;

class KTLCircuitPlugin : public KTechLab::IDocumentPlugin
{
    Q_OBJECT
public:
    KTLCircuitPlugin( QObject *parent, const QVariantList& args );
    virtual ~KTLCircuitPlugin();
    virtual void unload();

    /**
     * @return the component model representing all components
     */
    ComponentModel * componentModel();
    /**
     * Register the component- @param{factory} to the plugin so it knows about all provided components
     * and can provide the user with entries in the browser to drag it into their circuit.
     */
    void registerComponentFactory( KTechLab::IComponentFactory *factory );

    /**
     * Deregister the component-factory from the plugin.
     */
    void deregisterComponentFactory( KTechLab::IComponentFactory *factory );

    /**
     * Create KTechLab::ComponentItem instances.
     * \sa KTechLab::IDocumentPlugin
     */
    virtual ComponentItem* createComponentItem(const QVariantMap& data, Theme* theme = 0);

private:
    void init();
    KTLComponentViewFactory *m_componentViewFactory;
    KTLCircuitDocumentFactory *m_documentFactory;

    ComponentModel *m_componentModel;
};

} // namespace KTechLab

#endif

