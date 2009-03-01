
#ifndef KTLCIRCUITPLUGIN_H
#define KTLCIRCUITPLUGIN_H

#include <interfaces/iplugin.h>
#include <QVariantList>

class KTLComponentViewFactory;
class KTLCircuitDocumentFactory;
class ComponentModel;

class KTLCircuitPlugin : public KDevelop::IPlugin
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
     * Register the @param{component} into the the model so users can see it in the component
     * browser and drag it into their circuit.
     */
    void registerComponent( const QString &component );

private:
    void init();
    KTLComponentViewFactory *m_componentViewFactory;
    KTLCircuitDocumentFactory *m_documentFactory;

    ComponentModel *m_componentModel;
};

#endif

