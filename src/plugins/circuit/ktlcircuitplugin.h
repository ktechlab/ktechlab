
#ifndef KTLCIRCUITPLUGIN_H
#define KTLCIRCUITPLUGIN_H

#include <interfaces/iplugin.h>
#include <QVariantList>

class KTLCircuitViewFactory;
class KTLCircuitDocumentFactory;

class KTLCircuitPlugin : public KDevelop::IPlugin
{
    Q_OBJECT
public:
    KTLCircuitPlugin( QObject *parent, const QVariantList& args );
    virtual ~KTLCircuitPlugin();
    virtual void unload();

private:
    void init();
    KTLCircuitViewFactory *m_viewFactory;
    KTLCircuitDocumentFactory *m_documentFactory;
};

#endif

