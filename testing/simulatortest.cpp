
// hack the C++ access control
#define private public
#include "simulator.h"
#undef private

#include "bjt.h"
#include "capacitance.h"
#include "cccs.h"
#include "ccvs.h"
#include "currentsignal.h"
#include "currentsource.h"
#include "diode.h"
#include "inductance.h"
#include "jfet.h"
#include "mosfet.h"
#include "opamp.h"
#include "resistance.h"
#include "vccs.h"
#include "vcvs.h"
#include "voltagepoint.h"
#include "voltagesignal.h"
#include "voltagesource.h"

#include "qdebug.h"

#include "simulatortest.h"

void SimulatorTest::createTest(){

    Simulator * sim = Simulator::self();

    sim->slotSetSimulating(false);

    Circuit *circ = new Circuit();

    BJT *q1 = new BJT(true);
    Capacitance *c1 = new Capacitance(1e-6, 1e-9);
    CCCS *cccs1 = new CCCS( 2.0 );
    CCVS *ccvs1 = new CCVS( 1.5 );
    CurrentSignal *i1 = new CurrentSignal(1e-6, 1e-3);
    CurrentSource *i2 = new CurrentSource(2e-3);
    Diode *d1 = new Diode();
    Inductance *l1 = new Inductance(1e-6, 1e-9);
    JFET *q2 = new JFET( JFET::nJFET);
    MOSFET *q3 = new MOSFET( MOSFET::neMOSFET );
    OpAmp *ic1 = new OpAmp();
    Resistance *r1 = new Resistance(1000.0);
    VCCS *vccs1 = new VCCS( 1.7);
    VCVS *vcvs1 = new VCVS( 1.9);
    VoltagePoint *v2 = new VoltagePoint(6.0);
    VoltageSignal *v3 = new VoltageSignal(1e-6, 3.0);
    VoltageSource *v4 = new VoltageSource(7.0);
    
    circ->addElement(q1);
    circ->addElement(c1);
    circ->addElement(cccs1);
    circ->addElement(ccvs1);
    circ->addElement(i1);
    circ->addElement(i2);
    circ->addElement(d1);
    circ->addElement(l1);
    circ->addElement(q2);
    circ->addElement(q3);
    circ->addElement(ic1);
    circ->addElement(r1);
    circ->addElement(vccs1);
    circ->addElement(vcvs1);
    circ->addElement(v2);
    circ->addElement(v3);
    circ->addElement(v4);

    circ->init();
    sim->attachCircuit(circ);

    qDebug() << "simulator: " << sim;

    sim->slotSetSimulating(true);
    sim->step();
    sim->step();
    sim->step();
}


QTEST_MAIN(SimulatorTest)
#include "simulatortest.moc"

