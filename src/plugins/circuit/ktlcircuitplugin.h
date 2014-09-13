
#ifndef KTLCIRCUITPLUGIN_H
#define KTLCIRCUITPLUGIN_H

// #include "interfaces/idocumentplugin.h"
#include "ktlcircuitplugin_qt.h"

#include <interfaces/iplugin.h>

namespace Sublime {
    class MainWindow;
}
class KActionCollection;

#include <QVariantList>
#include <interfaces/iplugin.h>
#include <kde_file.h>

namespace KTechLab
{
class IComponentItemFactory;
class FakeComponentItemFactory;

class KTLComponentViewFactory;
class KTLComponentEditorFactory;
class KTLCircuitDocumentFactory;
class ComponentModel;

class KTLCircuitPlugin :
    public KDevelop::IPlugin,
    public KTechLab::KTLCircuitPluginQt
{
    Q_OBJECT
    // Q_INTERFACES(KDevelop::IPlugin)

public:
    KTLCircuitPlugin( QObject *parent, const QVariantList& args );
    virtual ~KTLCircuitPlugin();
    virtual void unload();

	/**
	 * Override the method for creating actions for the main window
	 */
	virtual void createActionsForMainWindow(Sublime::MainWindow* window,
                                            QString& xmlFile,
										 KActionCollection& actions);

protected:
    void init();
    KTLComponentViewFactory *m_componentViewFactory;
    KTLCircuitDocumentFactory *m_documentFactory;
    KTLComponentEditorFactory* m_componentEditorFactory;


protected slots:
	/**
	 * Create a new circuit file and open it. This slot is activated by the
	 * file_new_circuit action.
	 */
	void newCircuitFile();

	/**
	 * Print the status of the simulator manager singleton
	 */
	void printSimulationManagerStatus();
};

} // namespace KTechLab

#endif

