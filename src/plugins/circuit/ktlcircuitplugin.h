
#ifndef KTLCIRCUITPLUGIN_H
#define KTLCIRCUITPLUGIN_H

#include "interfaces/idocumentplugin.h"

#include <QVariantList>

namespace KTechLab
{
class IComponentFactory;
} // namespace KTechLab

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
     * implementation for KTechLab::DocumentPlugin
     */
    virtual Plasma::DataContainer * createDataContainer( KDevelop::IDocument *document, const QString &component = QString() );

    /**
     * @return the component model representing all components
     */
    ComponentModel * componentModel();
    /**
     * Register the component- @param{factory} to the plugin so it knows about all provided components
     * and can provide the user with entries in the browser to drag it into their circuit.
     */
    void registerComponentFactory( KTechLab::IComponentFactory *factory );

private:
    void init();
    KTLComponentViewFactory *m_componentViewFactory;
    KTLCircuitDocumentFactory *m_documentFactory;

    ComponentModel *m_componentModel;
};

#endif

