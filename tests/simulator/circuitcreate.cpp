/*
    Tests for circuit simulation. Tries to create a circuit. Can be used as 
    reference for further tests.
    
    
*/
#include <QtCore/QObject>

#include <QtTest>

#include <tests/autotestshell.h>
#include <tests/testcore.h>

// #include <iostream>

#include <qtest_kde.h>

#include <kdebug.h>

#include <interfaces/iplugincontroller.h>

#include "plugins/circuit/circuitmodel.h"

#include "plugins/circuit/ktlcircuitplugin.h"

#include "plugins/circuit/circuitdocument.h"

using namespace KDevelop;

using namespace KTechLab;

class CircuitCreateTest : public QObject
{
  Q_OBJECT

  private slots:

    void init(){
        AutoTestShell::init();
        m_core = new KDevelop::TestCore();
        // m_core->initialize(KDevelop::Core::NoUi );
        m_core->initialize(KDevelop::Core::Default );
/*        
        QVariantList args;
        plugin = new KTLCircuitPlugin( 0, args);
         */
    }
    
    
    void cleanup(){
        m_core->cleanup();
        delete m_core;
    }
    
    void listPlugins() {
      
        IPlugin * ktlcircuit = 
                TestCore::self()->pluginController()
                ->loadPlugin("ktlcircuit");
            
        qDebug() << ktlcircuit << "\n";
        
        QStringList constraints;
        constraints << QString("'%1' in [X-KDevelop-SupportedMimeTypes]")
            .arg("application/x-circuit");
        QList<KDevelop::IPlugin*> plugins = TestCore::self()
            ->pluginController()
            ->allPluginsForExtension( "org.kdevelop.IDocument", constraints );
        
        // if this list contains no elements, then here's a problem
        qDebug() << "plugins size: " << plugins.size() << "\n";
        qDebug() << "plugins = " << plugins ;
        for(int i=0; i<plugins.size(); i++)
            qDebug() << "plugin[" << i << "] = " << plugins.at(i) << "\n";
        
        // TODO cast that plugin to kdlcircuitplugin
    }
    /*
    void createDocument(){
        // this test will make the core to abort
        // because it has no gui
        qDebug() << "starting\n";
        KUrl url("");
        CircuitDocument *doc = new CircuitDocument( url, m_core);
        
        qDebug() << "doc.items: " << doc->items() << "\n";
    }
    */
    void createModel(){
      
        qDebug() << "plugin = " << plugin << "\n";
//        std::cout << "just testing\n";
        
        CircuitModel *model = new CircuitModel();
        
        QVariantMap r1; // = new QVariantMap();
        r1.insert("type", QString("ec_resistor"));
        r1.insert("id","resistor1");
        
        model->addComponent(r1); // take that !
    }
    
  private:
    
    KDevelop::TestCore* m_core;
    KTLCircuitPlugin *plugin ;
};

// QTEST_MAIN(CircuitCreateTest)

QTEST_KDEMAIN(CircuitCreateTest , GUI)

#include "circuitcreate.moc"
