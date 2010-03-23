/*
    Tests for circuit simulation. Tries to create a circuit. Can be used as 
    reference for further tests.
    
    
*/
#include <QtCore/QObject>

#include <QtTest>

#include <tests/autotestshell.h>
#include <tests/testcore.h>

#include <kdebug.h>

#include "plugins/circuit/circuitmodel.h"

using namespace KDevelop;

using namespace KTechLab;

class CircuitCreateTest : public QObject
{
  Q_OBJECT

  private slots:

    void init(){
        AutoTestShell::init();
        m_core = new KDevelop::TestCore();
        m_core->initialize( KDevelop::Core::NoUi );
    }
    
    void cleanup(){
        m_core->cleanup();
        delete m_core;
    }
    
    void createDocument(){
        CircuitModel m;
        QVariantMap map;
        m.addComponent(map); // take that !
    }
    
  private:
    
    KDevelop::TestCore* m_core;
    
};

QTEST_MAIN(CircuitCreateTest)

#include "circuitcreate.moc"
