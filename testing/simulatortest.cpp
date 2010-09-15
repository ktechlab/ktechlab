
// hack the C++ access control
#define private public
#include "simulator.h"
#undef private

#include "bjt.h"
#include "capacitance.h"
#include "cccs.h"

#include "qdebug.h"

#include "simulatortest.h"

void SimulatorTest::createTest(){

    Simulator * sim = Simulator::self();

    sim->slotSetSimulating(false);

    Circuit *circ = new Circuit();

    BJT *q1 = new BJT(true);
    Capacitance *c1 = new Capacitance(1e-6, 1e-9);
    CCCS *cccs1 = new CCCS( 2.0 );
    
    circ->addElement(q1);
    circ->addElement(c1);
    circ->addElement(cccs1);

    sim->attachCircuit(circ);

    qDebug() << "simulator: " << sim;

    sim->step();
    sim->step();
    sim->step();
}


QTEST_MAIN(SimulatorTest)
#include "simulatortest.moc"

