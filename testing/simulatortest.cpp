
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

// hack the circuit's "addElement" method
#define protected public
#include "circuit.h"
#undef protected

#include "qdebug.h"

#include "simulatortest.h"

#include <connector.h>
#include <ecnode.h>

// components
#include <resistor.h>
#include <ecvoltagesource.h>
#include <electronicconnector.h>
#include <ecnode.h>
#include <ecfixedvoltage.h>
#include <capacitor.h>
#include <eccurrentsource.h>

#include <ktlconfig.h>


#define MAX_COMPARE_ERROR 1e-6

#define ASSERT_DOUBLE_EQUALS(a,b) \
    Q_ASSERT( qAbs((a) - (b)) < MAX_COMPARE_ERROR )


const double maxCurrentError = 1e-6;
const double maxVoltageError = 1e-6;

void SimulatorTest::initTestCase()
{
    qDebug() << "Starting testing..";
}

void SimulatorTest::cleanupTestCase()
{
    qDebug() << "Cleaning up...";
    KtlConfig::destroy();
    Simulator::destroy();
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

/*    
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
*/

    ElementMap *v4m = new ElementMap(v4);
    ElementMap *r1m = new ElementMap(r1);

    Pin *pin1 = v4m->pin(0);
    Pin *pin2 = v4m->pin(1);

    // we manipulate the elements directly, so these are needed
    pin1->setGroundType(Pin::gt_medium);
    pin2->setGroundType(Pin::gt_medium);

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

    qDebug() << "pin1  id:" << pin1->eqId() << " voltage:" << pin1->voltage() << " current in: " << pin1->sourceCurrent();
    qDebug() << "pin2  id:" << pin2->eqId() << " voltage:" << pin2->voltage() << " current in: " << pin2->sourceCurrent();
    qDebug() << "pinR1 id:" << pinR1->eqId() << " voltage:" << pinR1->voltage() << " current in: " << pinR1->sourceCurrent();
    qDebug() << "pinR2 id:" << pinR2->eqId() << " voltage:" << pinR2->voltage() << " current in: " << pinR2->sourceCurrent();
    qDebug() << "wire1 current known:" << wire1->currentIsKnown() << "value:" << wire1->current();
    qDebug() << "wire2 current known:" << wire2->currentIsKnown() << "value:" << wire2->current();

    Q_ASSERT( QABS( pin2->voltage() - pin1->voltage() - 7 ) < maxVoltageError );
    Q_ASSERT( QABS( pinR2->voltage() - pinR1->voltage() - 7 ) < maxVoltageError );
    Q_ASSERT( QABS( pin1->voltage() - pinR1->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin2->voltage() - pinR2->voltage() ) < maxVoltageError );
    Q_ASSERT( wire1->currentIsKnown() );
    Q_ASSERT( wire2->currentIsKnown() );
    Q_ASSERT( QABS( QABS( wire1->current() ) - 0.007 ) < maxCurrentError );
    Q_ASSERT( QABS( QABS( wire2->current() ) - 0.007 ) < maxCurrentError );

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

    Q_ASSERT( QABS( pin2->voltage() - pin1->voltage() - 7 ) < maxVoltageError );
    Q_ASSERT( QABS( pinR2->voltage() - pinR1->voltage() - 7 ) < maxVoltageError );
    Q_ASSERT( QABS( pin1->voltage() - pinR1->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin2->voltage() - pinR2->voltage() ) < maxVoltageError );
    Q_ASSERT( wire1->currentIsKnown() );
    Q_ASSERT( wire2->currentIsKnown() );
    Q_ASSERT( QABS( QABS( wire1->current() ) - 0.007 ) < maxCurrentError );
    Q_ASSERT( QABS( QABS( wire2->current() ) - 0.007 ) < maxCurrentError );

    sim->slotSetSimulating(false);
    sim->detachCircuit(circ);

    circ->removeElementMap(r1m);
    circ->removeElementMap(v4m);
    delete r1m;
    delete v4m;
    delete circ;

    // r1, v4 are already deleted!
    delete q1;
    delete c1;
    delete cccs1;
    delete ccvs1;
    delete i1;
    delete i2;
    delete d1;
    delete l1;
    delete q2;
    delete q3;
    delete ic1;
    delete vccs1;
    delete vcvs1;
    delete v2;
    delete v3;
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
    // we manipulate the elements directly, so these are needed
    pin1->setGroundType(Pin::gt_medium);
    pin2->setGroundType(Pin::gt_medium);
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

    circ->updateCurrents();

    circ->displayEquations();

    qDebug() << "pin1  id:" << pin1->eqId() << " voltage:" << pin1->voltage();
    qDebug() << "pin2  id:" << pin2->eqId() << " voltage:" << pin2->voltage();
    qDebug() << "pinR1 id:" << pinR1->eqId() << " voltage:" << pinR1->voltage();
    qDebug() << "pinR2 id:" << pinR2->eqId() << " voltage:" << pinR2->voltage();
    qDebug() << "wire1 current known:" << wire1->currentIsKnown() << "value:" << wire1->current();
    qDebug() << "wire2 current known:" << wire2->currentIsKnown() << "value:" << wire2->current();

    Q_ASSERT( QABS( pin2->voltage() - pin1->voltage() - 8 ) < maxVoltageError );
    Q_ASSERT( QABS( pinR2->voltage() - pinR1->voltage() - 8 ) < maxVoltageError );
    Q_ASSERT( QABS( pin1->voltage() - pinR1->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin2->voltage() - pinR2->voltage() ) < maxVoltageError );
    Q_ASSERT( wire1->currentIsKnown() );
    Q_ASSERT( wire2->currentIsKnown() );
    Q_ASSERT( QABS( QABS( wire1->current() ) - 0.008 ) < maxCurrentError );
    Q_ASSERT( QABS( QABS( wire2->current() ) - 0.008 ) < maxCurrentError );

    sim->slotSetSimulating(false);
    sim->detachCircuit(circ);

    delete wire1;
    delete wire2;
    circ->removeElementMap(r1m);
    circ->removeElementMap(v4m);
    delete r1m;
    delete v4m;
    delete circ;
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

    ElementMap *v1m = new ElementMap(v1);
    Pin *pin1 = v1m->pin(0);
    Pin *pin2 = v1m->pin(1);

    // we manipulate the elements directly, so these are needed
    pin1->setGroundType(Pin::gt_medium);
    pin2->setGroundType(Pin::gt_medium);

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

    // voltages across components
    Q_ASSERT( QABS( pin2->voltage() - pin1->voltage() - 8 ) < maxVoltageError );
    Q_ASSERT( QABS( pinR12->voltage() - pinR11->voltage() - 8 ) < maxVoltageError );
    Q_ASSERT( QABS( pinR22->voltage() - pinR21->voltage() - 8 ) < maxVoltageError );
    Q_ASSERT( QABS( pinR32->voltage() - pinR31->voltage() - 8 ) < maxVoltageError );
    Q_ASSERT( QABS( pinR42->voltage() - pinR41->voltage() - 8 ) < maxVoltageError );
    // voltages on nodes, part 1
    Q_ASSERT( QABS( pin1->voltage() - pinR11->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin1->voltage() - pinR21->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin1->voltage() - pinR31->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin1->voltage() - pinR41->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin1->voltage() - pinC12a->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin1->voltage() - pinC34a->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin1->voltage() - pinC1234a->voltage() ) < maxVoltageError );
    // part 2
    Q_ASSERT( QABS( pin2->voltage() - pinR12->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin2->voltage() - pinR22->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin2->voltage() - pinR32->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin2->voltage() - pinR42->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin2->voltage() - pinC12b->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin2->voltage() - pinC34b->voltage() ) < maxVoltageError );
    Q_ASSERT( QABS( pin2->voltage() - pinC1234b->voltage() ) < maxVoltageError );
    // pin currents are known?
    foreach(Pin *pin, allpins){
        Q_ASSERT( pin->currentIsKnown());
    }
    // wire currents ?
    foreach(Wire *wire, allwires){
        Q_ASSERT( wire->currentIsKnown());
    }
    // current values?
    Q_ASSERT( QABS( QABS( wire12a1->current() ) - 0.0008888889 ) < maxCurrentError );
    Q_ASSERT( QABS( QABS( wire12a2->current() ) - 0.001 ) < maxCurrentError );
    Q_ASSERT( QABS( QABS( wire34a1->current() ) - 0.002 ) < maxCurrentError );
    Q_ASSERT( QABS( QABS( wire34a2->current() ) - 0.004) < maxCurrentError );

    Q_ASSERT( QABS( QABS( wire1234a1->current() ) - 0.00188889 ) < maxCurrentError );
    Q_ASSERT( QABS( QABS( wire1234a2->current() ) - 0.006 ) < maxCurrentError );

    Q_ASSERT( QABS( QABS( wireAll1->current() ) - 0.00788889 ) < maxCurrentError );
    // other part
    Q_ASSERT( QABS( QABS( wire12b1->current() ) - 0.0008888889 ) < maxCurrentError );
    Q_ASSERT( QABS( QABS( wire12b2->current() ) - 0.001 ) < maxCurrentError );
    Q_ASSERT( QABS( QABS( wire34b1->current() ) - 0.002 ) < maxCurrentError );
    Q_ASSERT( QABS( QABS( wire34b2->current() ) - 0.004) < maxCurrentError );

    Q_ASSERT( QABS( QABS( wire1234b1->current() ) - 0.00188889 ) < maxCurrentError );
    Q_ASSERT( QABS( QABS( wire1234b2->current() ) - 0.006 ) < maxCurrentError );

    Q_ASSERT( QABS( QABS( wireAll2->current() ) - 0.00788889 ) < maxCurrentError );

    sim->slotSetSimulating(false);
    sim->detachCircuit(circ);

    // clean up
    delete wire1234a1;
    delete wire1234a2;
    delete wire1234b1;
    delete wire1234b2;
    delete wire12a1;
    delete wire12a2;
    delete wire12b1;
    delete wire12b2;
    delete wire34a1;
    delete wire34a2;
    delete wire34b1;
    delete wire34b2;
    delete wireAll1;
    delete wireAll2;
    circ->removeElementMap(r1m);
    circ->removeElementMap(r2m);
    circ->removeElementMap(r3m);
    circ->removeElementMap(r4m);
    circ->removeElementMap(v1m);
    delete pinC12a;
    delete pinC12b;
    delete pinC34a;
    delete pinC34b;
    delete pinC1234a;
    delete pinC1234b;
    delete r1m;
    delete r2m;
    delete r3m;
    delete r4m;
    delete v1m;
    delete circ;
}

void SimulatorTest::testComponent_SourceAndResistor()
{
    qDebug() << "starting";
    Circuit *circ = new Circuit();

    Resistor *r1 = new Resistor(*circ);
    ECCell *v1 = new ECCell(*circ);
    ElectronicConnector *c1 = new ElectronicConnector(r1->pinByName("n1"), v1->pinByName("n1"));
    ElectronicConnector *c2 = new ElectronicConnector(r1->pinByName("p1"), v1->pinByName("p1"));

    Simulator * sim = Simulator::self();
    sim->slotSetSimulating(false);

    circ->init();

    sim->attachCircuit(circ);

    sim->slotSetSimulating(true);

    sim->step();
    circ->updateCurrents();

    circ->displayEquations();

    qDebug() << "c1 current: " << c1->wire()->current();
    qDebug() << "c2 current: " << c2->wire()->current();
    qDebug() << "voltages: R1:1 R1:2 V1:1 V1:2\n  "
        << r1->pinByName("n1")->pin()->voltage() << r1->pinByName("p1")->pin()->voltage()
        << v1->pinByName("n1")->pin()->voltage() << v1->pinByName("p1")->pin()->voltage();

    Q_ASSERT( QABS(c1->wire()->current() + 5.0) < maxCurrentError);
    Q_ASSERT( QABS(c2->wire()->current() - 5.0) < maxCurrentError);
    Q_ASSERT( QABS(r1->pinByName("p1")->pin()->voltage() - r1->pinByName("n1")->pin()->voltage() - 5) < maxVoltageError);
    Q_ASSERT( QABS(v1->pinByName("p1")->pin()->voltage() - v1->pinByName("n1")->pin()->voltage() - 5) < maxVoltageError);

    // change resistor value
    r1->propertyByName("resistance")->setValue(2000);

    sim->step();
    circ->updateCurrents();

    circ->displayEquations();
    qDebug() << "c1 current: " << c1->wire()->current();
    qDebug() << "c2 current: " << c2->wire()->current();

    Q_ASSERT( QABS(c1->wire()->current() + 0.0025) < maxCurrentError);
    Q_ASSERT( QABS(c2->wire()->current() - 0.0025) < maxCurrentError);
    Q_ASSERT( QABS(r1->pinByName("p1")->pin()->voltage() - r1->pinByName("n1")->pin()->voltage() - 5) < maxVoltageError);
    Q_ASSERT( QABS(v1->pinByName("p1")->pin()->voltage() - v1->pinByName("n1")->pin()->voltage() - 5) < maxVoltageError);

    // change source value
    v1->propertyByName("voltage")->setValue(2);

    sim->step();
    circ->updateCurrents();

    circ->displayEquations();
    qDebug() << "c1 current: " << c1->wire()->current();
    qDebug() << "c2 current: " << c2->wire()->current();
    qDebug() << "voltages: R1:1 R1:2 V1:1 V1:2\n  "
        << r1->pinByName("n1")->pin()->voltage() << r1->pinByName("p1")->pin()->voltage()
        << v1->pinByName("n1")->pin()->voltage() << v1->pinByName("p1")->pin()->voltage();

    Q_ASSERT( QABS(c1->wire()->current() + 0.001) < maxCurrentError);
    Q_ASSERT( QABS(c2->wire()->current() - 0.001) < maxCurrentError);
    Q_ASSERT( QABS(r1->pinByName("p1")->pin()->voltage() - r1->pinByName("n1")->pin()->voltage() - 2) < maxVoltageError);
    Q_ASSERT( QABS(v1->pinByName("p1")->pin()->voltage() - v1->pinByName("n1")->pin()->voltage() - 2) < maxVoltageError);

    sim->slotSetSimulating(false);
    sim->detachCircuit(circ);

    // clean up
    delete c1;
    delete c2;
    delete r1;
    delete v1;
    delete circ;
}

void SimulatorTest::testComponent_voltageDivider()
{
    Circuit *circ = new Circuit();
    Resistor *r1 = new Resistor(*circ);
    Resistor *r2 = new Resistor(*circ);
    ECCell *v1 = new ECCell(*circ);

    r1->propertyByName("resistance")->setValue(2000);
    r2->propertyByName("resistance")->setValue(3000);

    v1->propertyByName("voltage")->setValue(5);

    ElectronicConnector *c1 = new ElectronicConnector(v1->pinByName("p1"), r1->pinByName("p1"));
    ElectronicConnector *c2 = new ElectronicConnector(r1->pinByName("n1"), r2->pinByName("p1"));
    ElectronicConnector *c3 = new ElectronicConnector(r2->pinByName("n1"), v1->pinByName("n1"));

    Simulator * sim = Simulator::self();
    sim->slotSetSimulating(false);

    circ->init();

    sim->attachCircuit(circ);

    sim->slotSetSimulating(true);

    sim->step();
    circ->updateCurrents();

    circ->displayEquations();
    qDebug() << "c1 current: " << c1->wire()->current();
    qDebug() << "c2 current: " << c2->wire()->current();
    qDebug() << "c3 current: " << c3->wire()->current();
    qDebug() << "v1 voltages: p1: " << v1->pinByName("p1")->pin()->voltage() << " n1:" << v1->pinByName("n1")->pin()->voltage();

    Q_ASSERT(QABS(QABS(c1->wire()->current()) - 0.001) < maxCurrentError);
    Q_ASSERT(QABS(QABS(c2->wire()->current()) - 0.001) < maxCurrentError);
    Q_ASSERT(QABS(QABS(c3->wire()->current()) - 0.001) < maxCurrentError);
    Q_ASSERT(QABS(v1->pinByName("p1")->pin()->voltage() - v1->pinByName("n1")->pin()->voltage() - 5) < maxVoltageError);
    Q_ASSERT(QABS(r1->pinByName("p1")->pin()->voltage() - r1->pinByName("n1")->pin()->voltage() - 2) < maxVoltageError);
    Q_ASSERT(QABS(r2->pinByName("p1")->pin()->voltage() - r2->pinByName("n1")->pin()->voltage() - 3) < maxVoltageError);

    sim->slotSetSimulating(false);
    sim->detachCircuit(circ);

    delete c1;
    delete c2;
    delete c3;
    delete r1;
    delete r2;
    delete v1;
    delete circ;
}

void SimulatorTest::testComponent_fixedVoltage()
{
    Circuit *circ = new Circuit();

    ECFixedVoltage *v1 = new ECFixedVoltage(*circ);
    v1->propertyByName("voltage")->setValue(5);
    ECFixedVoltage *v2 = new ECFixedVoltage(*circ);
    v2->propertyByName("voltage")->setValue(-5);

    Simulator * sim = Simulator::self();
    sim->slotSetSimulating(false);

    circ->init();

    sim->attachCircuit(circ);

    sim->slotSetSimulating(true);

    sim->step();
    circ->updateCurrents();

    circ->displayEquations();
    qDebug() << "v1: " << v1->pinByName("p1")->pin()->voltage();
    qDebug() << "v2: " << v2->pinByName("p1")->pin()->voltage();

    qDebug() << " ------------ step 2 ----------------";

    Resistor *r1 = new Resistor(*circ);
    r1->propertyByName("resistance")->setValue(1000);

    ElectronicConnector *c1 = new ElectronicConnector(r1->pinByName("n1"), v1->pinByName("p1"));
    ElectronicConnector *c2 = new ElectronicConnector(r1->pinByName("p1"), v2->pinByName("p1"));

    circ->init();

    sim->step();
    circ->updateCurrents();

    circ->displayEquations();
    qDebug() << "v1: " << v1->pinByName("p1")->pin()->voltage();
    qDebug() << "v2: " << v2->pinByName("p1")->pin()->voltage();
    qDebug() << "c1 current: " << c1->wire()->current();
    qDebug() << "c2 current: " << c2->wire()->current();

    Q_ASSERT( QABS( v1->pinByName("p1")->pin()->voltage() -
                    v2->pinByName("p1")->pin()->voltage() - 10) < maxVoltageError);
    Q_ASSERT( QABS(c2->wire()->current() - (-0.01)) < maxCurrentError);
    Q_ASSERT( QABS(c1->wire()->current() -   0.01 ) < maxCurrentError);

    sim->slotSetSimulating(false);
    sim->detachCircuit(circ);

    // cleanup
    delete c1;
    delete c2;
    delete r1;
    delete v1;
    delete v2;
    delete circ;
}

void SimulatorTest::pinReduceTest()
{
    Circuit c;
    ECNode n1(c);
    ECNode n2(c);
    ECNode n3(c);
    ECNode n4(c);
    ECNode n5(c);
    Resistor r1(c);
    Resistor v1(c);
    ElectronicConnector c1(&n1, r1.pinByName("n1"));
    ElectronicConnector c6(&n1, &n2);
    ElectronicConnector c2(&n2, v1.pinByName("n1"));
    ElectronicConnector c3(&n3, r1.pinByName("p1"));
    ElectronicConnector c4(&n3, &n5);
    ElectronicConnector c5(&n4, &n5);
    ElectronicConnector ce(&n4, v1.pinByName("p1"));

    c.init();
    c.displayEquations();
    c.updateCurrents();

    // all the pins above should map to the same equations
    Q_ASSERT( c.equationCount() == 2);
}

void SimulatorTest::pinReduceTestWithGround()
{
    Circuit c;
    ECNode n1(c);
    ECNode n2(c);
    ECNode n3(c);
    ECNode n4(c);
    ECNode n5(c);
    Resistor r1(c);
    Resistor v1(c);
    ElectronicConnector c1(&n1, r1.pinByName("n1"));
    ElectronicConnector c6(&n1, &n2);
    ElectronicConnector c2(&n2, v1.pinByName("n1"));
    ElectronicConnector c3(&n3, r1.pinByName("p1"));
    ElectronicConnector c4(&n3, &n5);
    ElectronicConnector c5(&n4, &n5);
    ElectronicConnector ce(&n4, v1.pinByName("p1"));

    n1.pin(0)->setGroundType(Pin::gt_medium);

    c.init();
    c.displayEquations();
    c.updateCurrents();

    // all the pins above should map to the same equations
    Q_ASSERT( c.equationCount() == 1);
}

void SimulatorTest::testComponent_currentSource()
{
    Circuit c;
    ECCurrentSource i1(c);
        // 20 mA
    Resistor r1(c);
        // 1 ohm, by default

    ElectronicConnector ec1(i1.pinByName("p1"), r1.pinByName("p1"));
    ElectronicConnector ec2(i1.pinByName("n1"), r1.pinByName("n1"));

    c.init();
    c.displayEquations();

    Simulator * sim = Simulator::self();
    sim->attachCircuit(&c);
    sim->slotSetSimulating(true);
    sim->step();

    c.updateCurrents();

    c.displayEquations();

    qDebug() << "ec1 current: " << ec1.wire()->current();
    qDebug() << "ec2 current: " << ec2.wire()->current();
    qDebug() << "r1 voltages: " << r1.pinByName("n1")->pin()->voltage()
        << r1.pinByName("p1")->pin()->voltage();
    qDebug() << "i1 voltages: " << i1.pinByName("n1")->pin()->voltage()
        << i1.pinByName("p1")->pin()->voltage();

    sim->detachCircuit(&c);

    ASSERT_DOUBLE_EQUALS( qAbs(ec1.wire()->current()), 0.02);
    ASSERT_DOUBLE_EQUALS( qAbs(ec2.wire()->current()), 0.02);
    ASSERT_DOUBLE_EQUALS(
        r1.pinByName("p1")->pin()->voltage() -
        r1.pinByName("n1")->pin()->voltage(),
        0.02 );
    ASSERT_DOUBLE_EQUALS(
        i1.pinByName("p1")->pin()->voltage() -
        i1.pinByName("n1")->pin()->voltage(),
        0.02 );
}


void SimulatorTest::testComponent_capacitor()
{
    Circuit c;
    Capacitor c1(c);
    ECCell v1(c);
    Resistor r1(c);

    ElectronicConnector cc1( v1.pinByName("p1"), r1.pinByName("n1"));
    ElectronicConnector cc2( r1.pinByName("p1"), c1.pinByName("p1"));
    ElectronicConnector cc3( c1.pinByName("n1"), v1.pinByName("n1"));

    c.init();

    Simulator * sim = Simulator::self();
    sim->attachCircuit(&c);
    sim->slotSetSimulating(true);
    sim->step();

    c.updateCurrents();

    c.displayEquations();

    qDebug() << "C1 voltages: " << c1.pinByName("p1")->pin()->voltage()
        << c1.pinByName("n1")->pin()->voltage();

    sim->step();
    c.displayEquations();

    qDebug() << "C1 voltages: " << c1.pinByName("p1")->pin()->voltage()
        << c1.pinByName("n1")->pin()->voltage();

    for(int i=0; i<15; i++) {
        sim->step();
        qDebug() << "C1 voltages: " << c1.pinByName("p1")->pin()->voltage()
        << c1.pinByName("n1")->pin()->voltage();
    }

    sim->detachCircuit(&c);
}



QTEST_MAIN(SimulatorTest)
#include "simulatortest.moc"

