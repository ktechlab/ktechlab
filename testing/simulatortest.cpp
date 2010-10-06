
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

QList<ElementMap*> allEms;

void setupElement(Element *elem, Pin *pin1, Pin *pin2);

void SimulatorTest::init()
{
    qDebug() << "----------------- init --------------\ncleaning up" << allEms.size() << "elementmaps.";
    foreach(ElementMap *m, allEms){
        delete m;
    }
    allEms.clear();
}

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
    /*
    pin1->calculateCurrentFromWires();
    pin2->calculateCurrentFromWires();
    pinR1->calculateCurrentFromWires();
    pinR2->calculateCurrentFromWires();
    */
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
    allEms.append(em);
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

    // set the current on the pins
    foreach(ElementMap *em, allEms){
        em->mergeCurrents();
    }

    /*
    v4->updateCurrents();
    r1->updateCurrents();
*/
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

void SimulatorTest::testSourceAnd4ResistanceInParallel()
{
    Simulator * sim = Simulator::self();

    sim->slotSetSimulating(false);

    Circuit *circ = new Circuit();

    VoltageSource *v1 = new VoltageSource(8.0);
    Resistance *r1 = new Resistance(9000.0);
    Resistance *r2 = new Resistance(8000.0);
    Resistance *r3 = new Resistance(4000.0);
    Resistance *r4 = new Resistance(2000.0);

    Pin *pin1 = new Pin();
    Pin *pin2 = new Pin();

    Pin *pinR11 = new Pin();
    Pin *pinR12 = new Pin();

    Pin *pinR21 = new Pin();
    Pin *pinR22 = new Pin();

    Pin *pinR31 = new Pin();
    Pin *pinR32 = new Pin();

    Pin *pinR41 = new Pin();
    Pin *pinR42 = new Pin();

    Pin *pinC12a = new Pin();
    Pin *pinC12b = new Pin();
    Pin *pinC34a = new Pin();
    Pin *pinC34b = new Pin();

    Pin *pinC1234a = new Pin();
    Pin *pinC1234b = new Pin();

    // wires...
    Wire *wire12a1 = new Wire(pinR11, pinC12a);
    Wire *wire12a2 = new Wire(pinR21, pinC12a);

    Wire *wire12b1 = new Wire(pinR12, pinC12b);
    Wire *wire12b2 = new Wire(pinR22, pinC12b);

    Wire *wire34a1 = new Wire(pinR31, pinC34a);
    Wire *wire34a2 = new Wire(pinR41, pinC34a);

    Wire *wire34b1 = new Wire(pinR32, pinC34b);
    Wire *wire34b2 = new Wire(pinR42, pinC34b);

    Wire *wire1234a1 = new Wire(pinC12a, pinC1234a);
    Wire *wire1234a2 = new Wire(pinC34a, pinC1234a);
    Wire *wire1234b1 = new Wire(pinC12b, pinC1234b);
    Wire *wire1234b2 = new Wire(pinC34b, pinC1234b);

    Wire *wireAll1 = new Wire(pinC1234a, pin1);
    Wire *wireAll2 = new Wire(pinC1234b, pin2);

    QList<Wire*> allwires;
    allwires << wire12a1 << wire12a2 << wire12b1 << wire12b2 << wire34a1 << wire34a2 << wire34b1 << wire34b2
        << wire1234a1 << wire1234a2 << wire1234b1 << wire1234b2 << wireAll1 << wireAll2 ;

    circ->addElement(v1);
    circ->addElement(r1);
    circ->addElement(r2);
    circ->addElement(r3);
    circ->addElement(r4);

    circ->addPin(pin1);
    circ->addPin(pin2);

    circ->addPin(pinR11);
    circ->addPin(pinR12);
    circ->addPin(pinR21);
    circ->addPin(pinR22);
    circ->addPin(pinR31);
    circ->addPin(pinR32);
    circ->addPin(pinR41);
    circ->addPin(pinR42);

    circ->addPin(pinC12a);
    circ->addPin(pinC12b);
    circ->addPin(pinC34a);
    circ->addPin(pinC34b);
    circ->addPin(pinC1234a);
    circ->addPin(pinC1234b);

    circ->init();

    setupElement(v1, pin1, pin2);
    setupElement(r1, pinR11, pinR12);
    setupElement(r2, pinR21, pinR22);
    setupElement(r3, pinR31, pinR32);
    setupElement(r4, pinR41, pinR42);

    sim->attachCircuit(circ);

    sim->slotSetSimulating(true);
    sim->step();

    // try to make the currents work
    circ->updateCurrents();

    foreach(ElementMap *em, allEms){
        em->mergeCurrents();
    }

    circ->displayEquations();

    qDebug() << "pin1  id:" << pin1->eqId() << " voltage:" << pin1->voltage();
    qDebug() << "pin2  id:" << pin2->eqId() << " voltage:" << pin2->voltage();
    qDebug() << "pinC12a  id:" << pinC12a->eqId() << " voltage:" << pinC12a->voltage();

    qDebug() << "wireAll1 current known:" << wireAll1->currentIsKnown() << "value:" << wireAll1->current();
    qDebug() << "wire12a1 current known:" << wire12a1->currentIsKnown() << "value:" << wire12a1->current();
    qDebug() << "wire12b1 current known:" << wire12b1->currentIsKnown() << "value:" << wire12b1->current();

    // create a list of wires
    QList<Pin*> allpins; // = new QList<Pin*>;
    std::set<Pin*> * allPinsStd = circ->getPins();
    std::set<Pin*>::iterator end = allPinsStd->end();
    for(std::set<Pin*>::iterator it = allPinsStd->begin(); it != end; ++it){
        allpins.append(*it);
    }

    foreach(Pin *pin, allpins){
        qDebug() << "pin eq id: " << pin->eqId();
    }

    r2->updateCurrents();
    qDebug() << "wire current for 12a2: " << wire12a2->current();

    r3->updateCurrents();
    qDebug() << "wire current for wire34a2: " << wire34a2->current();

    qDebug() << "pinR11 current known:" << pinR11->currentIsKnown() << "value:" << pinR11->sourceCurrent();

    foreach(Pin *pin, allpins){
        pin->calculateCurrentFromWires();
    }
    /*
    pin1->calculateCurrentFromWires();
    pin2->calculateCurrentFromWires();
    pinR11->calculateCurrentFromWires();
    pinR12->calculateCurrentFromWires();
    pinR21->calculateCurrentFromWires();
    pinR22->calculateCurrentFromWires();
    pinR31->calculateCurrentFromWires();
    pinR32->calculateCurrentFromWires();
    pinR41->calculateCurrentFromWires();
    pinR42->calculateCurrentFromWires();
    pinC12a->calculateCurrentFromWires();
    pinC12b ->calculateCurrentFromWires();
    pinC34a ->calculateCurrentFromWires();
    pinC34b ->calculateCurrentFromWires();
    pinC1234a ->calculateCurrentFromWires();
    pinC1234b ->calculateCurrentFromWires();
    */
    qDebug() << "wireAll1 current known:" << wireAll1->currentIsKnown() << "value:" << wireAll1->current();
    qDebug() << "wire12a1 current known:" << wire12a1->currentIsKnown() << "value:" << wire12a1->current();
    qDebug() << "wire12b1 current known:" << wire12b1->currentIsKnown() << "value:" << wire12b1->current();
    qDebug() << "wire34a1 current known:" << wire34a1->currentIsKnown() << "value:" << wire34a1->current();

    qDebug() << "pi1    current known:" << pin1->currentIsKnown() << "value:" << pin1->sourceCurrent();
    qDebug() << "pinR11 current known:" << pinR11->currentIsKnown() << "value:" << pinR11->sourceCurrent();
    qDebug() << "pinR21 current known:" << pinR21->currentIsKnown() << "value:" << pinR21->sourceCurrent();
    qDebug() << "pinR31 current known:" << pinR31->currentIsKnown() << "value:" << pinR31->sourceCurrent();
    qDebug() << "pinR41 current known:" << pinR41->currentIsKnown() << "value:" << pinR41->sourceCurrent();
}

QTEST_MAIN(SimulatorTest)
#include "simulatortest.moc"

