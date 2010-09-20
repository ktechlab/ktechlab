
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

#include "pin.h"

#include "elementmap.h"

#include "qdebug.h"

#include "simulatortest.h"

void setupElement(Element *elem, Pin *pin1, Pin *pin2);

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

    Pin *pin1 = new Pin();
    Pin *pin2 = new Pin();

    Pin *pinR1 = new Pin();
    Pin *pinR2 = new Pin();

    Wire *wire1 = new Wire(pin1, pinR1);
    Wire *wire2 = new Wire(pin2, pinR2);


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

    circ->addPin(pin1);
    circ->addPin(pin2);

    circ->addPin(pinR1);
    circ->addPin(pinR2);

    circ->init();

    setupElement(v4, pin1, pin2);
    setupElement(r1, pinR1, pinR2);

    sim->attachCircuit(circ);

    qDebug() << "simulator: " << sim;

    sim->slotSetSimulating(true);
    sim->step();

    // try to make the currents work
    v4->updateCurrents();
    r1->updateCurrents();
    pin1->setCurrentIfOneWire();
    pin2->setCurrentIfOneWire();
    pinR1->setCurrentIfOneWire();
    pinR2->setCurrentIfOneWire();
    pin1->calculateCurrentFromWires();
    pin2->calculateCurrentFromWires();
    pinR1->calculateCurrentFromWires();
    pinR2->calculateCurrentFromWires();

//    circ->updateCurrents();
    circ->displayEquations();

    qDebug() << "pin1  id:" << pin1->eqId() << " voltage:" << pin1->voltage();
    qDebug() << "pin2  id:" << pin2->eqId() << " voltage:" << pin2->voltage();
    qDebug() << "pinR1 id:" << pinR1->eqId() << " voltage:" << pinR1->voltage();
    qDebug() << "pinR2 id:" << pinR2->eqId() << " voltage:" << pinR2->voltage();
    qDebug() << "wire1 current known:" << wire1->currentIsKnown() << "value:" << wire1->current();
    qDebug() << "wire2 current known:" << wire2->currentIsKnown() << "value:" << wire2->current();

    sim->step();

    qDebug() << "pin1 id:" << pin1->eqId() << " voltage:" << pin1->voltage();
    qDebug() << "pin2 id:" << pin2->eqId() << " voltage:" << pin2->voltage();
    qDebug() << "pinR1 id:" << pinR1->eqId() << " voltage:" << pinR1->voltage();
    qDebug() << "pinR2 id:" << pinR2->eqId() << " voltage:" << pinR2->voltage();

    sim->step();

    qDebug() << "pin1 id:" << pin1->eqId() << " voltage:" << pin1->voltage();
    qDebug() << "pin2 id:" << pin2->eqId() << " voltage:" << pin2->voltage();
    qDebug() << "pinR1 id:" << pinR1->eqId() << " voltage:" << pinR1->voltage();
    qDebug() << "pinR2 id:" << pinR2->eqId() << " voltage:" << pinR2->voltage();

}

void setupElement(Element *elem, Pin *pin1, Pin *pin2){
    // element map stores the relation between and element and pins
    ElementMap *em = new ElementMap;
    em->setElement(elem);
    em->putPin(0, pin1);
    em->putPin(1, pin2);
    pin1->addElement(elem);
    pin2->addElement(elem);
    
    // mabe pin1 with pin1 ? and 2 with 2?
    // pin1->addCircuitDependentPin(pin2);
    // pin2->addCircuitDependentPin(pin1);
    
    // mabe pin1 with pin1 ? and 2 with 2?
    // pin1->addGroundDependentPin(pin2);
    // pin2->addGroundDependentPin(pin1);
    
    // FIXME
    // em.interCircuitDependent.push_back(pinSet); // ...
    // em.interGroundDependent.push_back(pinSet);
    
    em->setupCNodes();
}


void SimulatorTest::testSourceAndResistance()
{

    Simulator * sim = Simulator::self();

    sim->slotSetSimulating(false);

    Circuit *circ = new Circuit();

    VoltageSource *v4 = new VoltageSource(8.0);
    Resistance *r1 = new Resistance(1000.0);

    Pin *pin1 = new Pin();
    Pin *pin2 = new Pin();

    Pin *pinR1 = new Pin();
    Pin *pinR2 = new Pin();

    Wire *wire1 = new Wire(pin1, pinR1);
    Wire *wire2 = new Wire(pin2, pinR2);

    circ->addElement(v4);
    circ->addElement(r1);

    circ->addPin(pin1);
    circ->addPin(pin2);

    circ->addPin(pinR1);
    circ->addPin(pinR2);

    circ->init();

    // need to setup the element _after_ the circuit has been initialized
    setupElement(v4, pin1, pin2);
    setupElement(r1, pinR1, pinR2);

    sim->attachCircuit(circ);

    sim->slotSetSimulating(true);
    sim->step();

    // try to make the currents work
    circ->updateCurrents();
    /*
    v4->updateCurrents();
    r1->updateCurrents();
*/
    pin1->setCurrentIfOneWire();
    pin2->setCurrentIfOneWire();
    pinR1->setCurrentIfOneWire();
    pinR2->setCurrentIfOneWire();

    pin1->calculateCurrentFromWires();
    pin2->calculateCurrentFromWires();
    pinR1->calculateCurrentFromWires();
    pinR2->calculateCurrentFromWires();

    circ->displayEquations();

    qDebug() << "pin1  id:" << pin1->eqId() << " voltage:" << pin1->voltage();
    qDebug() << "pin2  id:" << pin2->eqId() << " voltage:" << pin2->voltage();
    qDebug() << "pinR1 id:" << pinR1->eqId() << " voltage:" << pinR1->voltage();
    qDebug() << "pinR2 id:" << pinR2->eqId() << " voltage:" << pinR2->voltage();
    qDebug() << "wire1 current known:" << wire1->currentIsKnown() << "value:" << wire1->current();
    qDebug() << "wire2 current known:" << wire2->currentIsKnown() << "value:" << wire2->current();

}
QTEST_MAIN(SimulatorTest)
#include "simulatortest.moc"

