
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


    ElementMap *v4m = new ElementMap(v4);
    ElementMap *r1m = new ElementMap(r1);

    Pin *pin1 = v4m->pin(0);
    Pin *pin2 = v4m->pin(1);

    Pin *pinR1 = r1m->pin(0);
    Pin *pinR2 = r1m->pin(1);

    Wire *wire1 = new Wire(pin1, pinR1);
    Wire *wire2 = new Wire(pin2, pinR2);

    circ->addElementMap(r1m);
    circ->addElementMap(v4m);

    circ->init();

    sim->attachCircuit(circ);

    qDebug() << "simulator: " << sim;

    sim->slotSetSimulating(true);
    sim->step();

    circ->updateCurrents();
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

void SimulatorTest::testSourceAndResistance()
{
    qDebug() << " ---------------------------------------- ";

    Simulator * sim = Simulator::self();

    sim->slotSetSimulating(false);

    Circuit *circ = new Circuit();

    VoltageSource *v4 = new VoltageSource(8.0);
    Resistance *r1 = new Resistance(1000.0);

    Pin *pin1; 
    Pin *pin2;

    Pin *pinR1;
    Pin *pinR2;

    ElementMap *v4m = new ElementMap( v4);
    pin1 = v4m->pin(0);
    pin2 = v4m->pin(1);
    ElementMap *r1m = new ElementMap( r1);
    pinR1 = r1m->pin(0);
    pinR2 = r1m->pin(1);

    circ->addElementMap(r1m);
    circ->addElementMap(v4m);

    Wire *wire1 = new Wire(pin1, pinR1);
    Wire *wire2 = new Wire(pin2, pinR2);

    circ->init();

    sim->attachCircuit(circ);

    sim->slotSetSimulating(true);
    sim->step();

    // try to make the currents work
    circ->updateCurrents();

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

    Pin *pinC12a = new Pin();
    Pin *pinC12b = new Pin();
    Pin *pinC34a = new Pin();
    Pin *pinC34b = new Pin();

    Pin *pinC1234a = new Pin();
    Pin *pinC1234b = new Pin();

    circ->addElement(v1);
    circ->addElement(r1);
    circ->addElement(r2);
    circ->addElement(r3);
    circ->addElement(r4);

    ElementMap *v1m = new ElementMap(v1);
    Pin *pin1 = v1m->pin(0);
    Pin *pin2 = v1m->pin(1);
    ElementMap *r1m = new ElementMap(r1);
    Pin *pinR11 = r1m->pin(0);
    Pin *pinR12 = r1m->pin(1);
    ElementMap *r2m = new ElementMap(r2);
    Pin *pinR21 = r2m->pin(0);
    Pin *pinR22 = r2m->pin(1);
    ElementMap *r3m = new ElementMap(r3);
    Pin *pinR31 = r3m->pin(0);
    Pin *pinR32 = r3m->pin(1);
    ElementMap *r4m = new ElementMap(r4);
    Pin *pinR41 = r4m->pin(0);
    Pin *pinR42 = r4m->pin(1);

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

    circ->addElementMap(v1m);
    circ->addElementMap(r1m);
    circ->addElementMap(r2m);
    circ->addElementMap(r3m);
    circ->addElementMap(r4m);

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


    circ->init();

    sim->attachCircuit(circ);

    sim->slotSetSimulating(true);
    sim->step();

    // try to make the currents work
    circ->updateCurrents();

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

    qDebug() << "wireAll1 current known:" << wireAll1->currentIsKnown() << "value:" << wireAll1->current();
    qDebug() << "wire12a1 current known:" << wire12a1->currentIsKnown() << "value:" << wire12a1->current();
    qDebug() << "wire12b1 current known:" << wire12b1->currentIsKnown() << "value:" << wire12b1->current();
    qDebug() << "wire34a1 current known:" << wire34a1->currentIsKnown() << "value:" << wire34a1->current();
    qDebug() << "wire1234a1 current known:" << wire1234a1->currentIsKnown() << "value:" << wire1234a1->current();
    qDebug() << "wire1234a2 current known:" << wire1234a2->currentIsKnown() << "value:" << wire1234a2->current();
    qDebug() << "wire1234b1 current known:" << wire1234b1->currentIsKnown() << "value:" << wire1234b1->current();
    qDebug() << "wire1234b2 current known:" << wire1234b2->currentIsKnown() << "value:" << wire1234b2->current();
    qDebug() << "wireAll1 current known:" << wireAll1->currentIsKnown() << "value:" << wireAll1->current();
    qDebug() << "wireAll2 current known:" << wireAll2->currentIsKnown() << "value:" << wireAll2->current();

    qDebug() << "pin1   current known:" << pin1->currentIsKnown() << "value:" << pin1->sourceCurrent();
    qDebug() << "pinR11 current known:" << pinR11->currentIsKnown() << "value:" << pinR11->sourceCurrent();
    qDebug() << "pinR21 current known:" << pinR21->currentIsKnown() << "value:" << pinR21->sourceCurrent();
    qDebug() << "pinR31 current known:" << pinR31->currentIsKnown() << "value:" << pinR31->sourceCurrent();
    qDebug() << "pinR41 current known:" << pinR41->currentIsKnown() << "value:" << pinR41->sourceCurrent();
}

QTEST_MAIN(SimulatorTest)
#include "simulatortest.moc"

