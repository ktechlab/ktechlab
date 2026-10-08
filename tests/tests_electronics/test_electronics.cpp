#include "circuit.h"
#include "capacitance.h"
#include "diode.h"
#include "elementset.h"
#include "inductance.h"
#include "pin.h"
#include "resistance.h"
#include "voltagepoint.h"
#include "wire.h"

#include <QDebug>
#include <QTest>


// Calculates the Lambert W function for large x
// Using the approximation W(x) = lnx-lnlnx+o(1)
// See https://en.wikipedia.org/wiki/Lambert_W_function
static double lambert(double x)
{
    // TODO: What is the + o(1) part
    Q_ASSERT(x > 1e10);
    const double r = std::log(x) - std::log(std::log(x));
    qDebug() << "lambert(x=" << x << ") = " << r;
    return r;
}

// Checks that actual is within expected +- the tolerance
#define QCOMPARE_TOL(actual, expected, tol) if (!std::isfinite(actual) ||  !std::isfinite(expected) || std::abs(actual - expected) > tol) QCOMPARE(actual, expected);


class KtlElectronicsTests: public QObject
{
    Q_OBJECT


private Q_SLOTS:
    void initTestCase()
    {
    }

    void pinSanityTest()
    {
        Pin p;
        QCOMPARE(p.voltage(), 0);
        QCOMPARE(p.current(), 0);
        QVERIFY(p.numWires() == 0);
        QVERIFY(!p.currentIsKnown());

        p.setVoltage(10);
        QCOMPARE(p.voltage(), 10);

        p.mergeCurrent(0.001);
        QCOMPARE(p.current(), 0.001);
        p.resetCurrent();
        QCOMPARE(p.current(), 0);

        QVERIFY(p.groundType() == Pin::gt_never);
        p.setGroundType(Pin::gt_always);
        QVERIFY(p.groundType() == Pin::gt_always);

    }

    void wireConnectionTest()
    {
        /// Wire from a -- b
        Pin a, b;
        Wire w(&a, &b);
        QVERIFY(a.numWires() == 1);
        QVERIFY(b.numWires() == 1);
        QVERIFY(a.outputWireList().contains(&w));
        QVERIFY(a.inputWireList().isEmpty());
        QVERIFY(b.inputWireList().contains(&w));
        QVERIFY(b.outputWireList().isEmpty());
        auto localPins = a.localConnectedPins();
        QCOMPARE(localPins.length(), 1);
        QVERIFY(localPins.contains(&b));
        localPins = b.localConnectedPins();
        QCOMPARE(localPins.length(), 1);
        QVERIFY(localPins.contains(&a));
    }

    void wireConnectionTest2()
    {
        /// Wires from a -- b -- c
        Pin a, b, c;
        Wire w1(&a, &b);
        Wire w2(&b, &c);
        QVERIFY(a.outputWireList().contains(&w1));
        QVERIFY(a.numWires() == 1);
        QVERIFY(b.inputWireList().contains(&w1));
        QVERIFY(b.outputWireList().contains(&w2));
        QVERIFY(b.numWires() == 2);
        QVERIFY(c.inputWireList().contains(&w2));
        QVERIFY(c.numWires() == 1);

        auto localPins = a.localConnectedPins();
        QCOMPARE(localPins.length(), 1);
        QVERIFY(localPins.contains(&b));

        localPins = b.localConnectedPins();
        QCOMPARE(localPins.length(), 2);
        QVERIFY(localPins.contains(&a) && localPins.contains(&c));

        localPins = c.localConnectedPins();
        QCOMPARE(localPins.length(), 1);
        QVERIFY(localPins.contains(&b));
    }

    void wireCurrentTest1()
    {
        /// Wire from a -- b
        Pin a, b;
        Wire w(&a, &b);
        a.setVoltage(1);
        b.setVoltage(1);
        QCOMPARE(w.voltage(), 1);

        // Current is not known on either pin
        QVERIFY(!w.calculateCurrent());
        QVERIFY(!w.currentIsKnown());

        // Check first branch of calculate current
        QVERIFY(a.numWires() == 1);
        a.mergeCurrent(0.1);
        a.setCurrentKnown(true);
        QVERIFY(w.calculateCurrent());
        QVERIFY(w.currentIsKnown());
        QCOMPARE(w.current(), 0.1);

        // Check Pin::calculateCurrentFromWires from output wire
        QVERIFY(!b.currentIsKnown());
        QVERIFY(b.calculateCurrentFromWires());
        QCOMPARE(b.current(), -w.current());
        QVERIFY(b.currentIsKnown());

        // Check second branch of calculate current
        a.setCurrentKnown(false);
        a.resetCurrent();
        QVERIFY(b.numWires() == 1);
        b.resetCurrent();
        b.mergeCurrent(1.0);
        b.setCurrentKnown(true);
        w.calculateCurrent();
        QCOMPARE(w.current(), -1.0); // Negative since output pin

        // Check Pin::calculateCurrentFromWires from input wire
        QVERIFY(!a.currentIsKnown());
        QVERIFY(a.calculateCurrentFromWires());
        QCOMPARE(a.current(), w.current());
        QVERIFY(a.currentIsKnown());

    }

    void wireCurrentTest2()
    {
        /// Two wires sharing a common pin/node
        //  a -- b -- c
        Pin a, b, c;
        Wire w1(&a, &b);
        Wire w2(&b, &c);

        a.setVoltage(1);
        b.setVoltage(1);
        c.setVoltage(1);
        QCOMPARE(w1.voltage(), 1);
        QCOMPARE(w2.voltage(), 1);

        QVERIFY(!w1.calculateCurrent());
        QVERIFY(!w2.calculateCurrent());
        QVERIFY(!w1.currentIsKnown());
        QVERIFY(!w2.currentIsKnown());

        // Set current middle node to test
        // case where not all wires have known current
        b.resetCurrent(); // Middle node current in == current out
        b.setCurrentKnown(true);
        QVERIFY(!w1.calculateCurrent());
        QVERIFY(!w1.currentIsKnown());
        QVERIFY(!w2.calculateCurrent());
        QVERIFY(!w2.currentIsKnown());

        // Set current on first node
        // This casues w2 to check the startPin and numWires > 2 case
        a.mergeCurrent(0.1);
        a.setCurrentKnown(true);
        QVERIFY(w1.calculateCurrent());
        QVERIFY(w1.currentIsKnown());
        QCOMPARE(w1.current(), a.current());

        QVERIFY(w2.calculateCurrent());
        QVERIFY(w2.currentIsKnown());
        QCOMPARE(w2.current(), a.current());

        // Clear and set current on last node
        // This casues w1 to check the endPin and numWires > 2 case
        w1.setCurrentKnown(false);
        w2.setCurrentKnown(false);
        a.resetCurrent();
        a.setCurrentKnown(false);
        c.mergeCurrent(0.1);
        c.setCurrentKnown(true);
        QVERIFY(w2.calculateCurrent());
        QVERIFY(w2.currentIsKnown());
        QCOMPARE(w2.current(), -c.current());
        QVERIFY(w1.calculateCurrent());
        QVERIFY(w1.currentIsKnown());
        QCOMPARE(w1.current(), -c.current());
    }

    void wireCurrentTest3()
    {
        // Three wires sharing a common pin/node
        //  a -- b -- c
        //       |
        //       d
        Pin a, b, c, d;
        Wire w1(&a, &b);
        Wire w2(&b, &c);
        Wire w3(&b, &d);
        a.mergeCurrent(0.1);
        a.setCurrentKnown(true);
        b.resetCurrent(); // By KCL b's current is known to be 0
        b.setCurrentKnown(true);
        c.mergeCurrent(-0.2);
        c.setCurrentKnown(true);
        QVERIFY(w1.calculateCurrent());
        QVERIFY(w1.currentIsKnown());
        QCOMPARE(w1.current(), 0.1);
        QVERIFY(w2.calculateCurrent());
        QVERIFY(w2.currentIsKnown());
        QCOMPARE(w2.current(), 0.2);
        QVERIFY(w3.calculateCurrent());
        QVERIFY(w3.currentIsKnown());
        QCOMPARE(w3.current(), -0.1);
    }

    void resistanceTest1()
    {
        // Circuit with (V1) - [R1] - GND
        Circuit c;
        ElementSet eset(&c, 1, 1);
        QCOMPARE(eset.cnodeCount(), 1);
        QCOMPARE(eset.cbranchCount(), 1);
        QVERIFY(eset.ground()->isGround);

        VoltagePoint v1(5.0);
        QVERIFY(v1.type() == Element::Element_VoltagePoint);
        QCOMPARE(v1.voltage(), 5.0);
        Resistance r1(1000);
        QVERIFY(r1.type() == Element::Element_Resistance);
        QCOMPARE(r1.resistance(), 1000);
        QCOMPARE(r1.conductance(), 0.001);

        eset.addElement(&v1);
        eset.addElement(&r1);
        QVERIFY(!eset.containsNonLinear());

        v1.setCNodes(0);
        v1.setCBranches(0);
        r1.setCNodes(0, -1);

        eset.createMatrixMap();

        v1.add_initial_dc();
        r1.add_initial_dc();

        eset.doLinear(true);
        eset.displayEquations();

        QCOMPARE(eset.cnodes()[0]->v, 5.0);
        QCOMPARE(eset.cbranches()[0]->i, 0.005);
    }

    void resistanceSeriesTest2()
    {
        // Circuit with (V1) - [R1] - [R2] - GND
        Circuit c;
        ElementSet eset(&c, 2, 1);
        QCOMPARE(eset.cnodeCount(), 2);
        QCOMPARE(eset.cbranchCount(), 1);

        VoltagePoint v1(12.0);
        Resistance r1(1000);
        Resistance r2(3000);
        eset.addElement(&v1);
        eset.addElement(&r1);
        eset.addElement(&r2);
        QVERIFY(!eset.containsNonLinear());

        v1.setCNodes(0);
        v1.setCBranches(0);
        r1.setCNodes(0, 1);
        r2.setCNodes(1, -1);

        eset.createMatrixMap();

        v1.add_initial_dc();
        r1.add_initial_dc();
        r2.add_initial_dc();

        eset.doLinear(true);
        eset.displayEquations();

        QCOMPARE(eset.cnodes()[0]->v, 12.0);
        QCOMPARE(eset.cnodes()[1]->v, 9.0);
        QCOMPARE(eset.cbranches()[0]->i, 0.003);
    }

    void capacitanceTest1()
    {
        // Circuit with (V1) - [R1] - |C1| - GND
        // See https://en.wikipedia.org/wiki/RC_circuit#Time-domain_considerations
        Circuit c;
        ElementSet eset(&c, 2, 1);
        QCOMPARE(eset.cnodeCount(), 2);
        QCOMPARE(eset.cbranchCount(), 1);

        const double dt = 1./10000.;
        VoltagePoint v1(12.0);
        Resistance r1(1000);
        Capacitance c1(22e-6, dt);
        QVERIFY(c1.type() == Element::Element_Capacitance);
        eset.addElement(&v1);
        eset.addElement(&r1);
        eset.addElement(&c1);
        QVERIFY(!eset.containsNonLinear());

        v1.setCNodes(0);
        v1.setCBranches(0);
        r1.setCNodes(0, 1);
        c1.setCNodes(1, -1);

        eset.createMatrixMap();

        v1.add_initial_dc();
        r1.add_initial_dc();
        c1.add_initial_dc();

        const double tolerance = 0.05;
        const double RC = r1.resistance()*c1.capacitance();
        for (uint i = 1; i < 10000; i++) {
            c1.time_step();
            eset.doLinear(true);
            if (i == 1) {
                eset.displayEquations();
            }
            const double t = i * dt;
            const double Vc = 12 * (1 - std::exp(-t/RC));
            const double I = (12-Vc)/r1.resistance();
            if (i % 100 == 0) {
                qDebug() << "t=" << t << "Vc=" << Vc << "V1=" << eset.cnodes()[1]->v
                << "I=" << I << "I1=" << eset.cbranches()[0]->i;
            }
            QCOMPARE_TOL(eset.cnodes()[0]->v, 12, tolerance);
            QCOMPARE_TOL(eset.cnodes()[1]->v, Vc, tolerance);
            QCOMPARE_TOL(eset.cbranches()[0]->i, I, tolerance);
        }

        QCOMPARE_TOL(eset.cnodes()[1]->v, 12, tolerance);
        QCOMPARE_TOL(eset.cbranches()[0]->i, 0, tolerance);
    }

    void inductanceTest1()
    {
        // Circuit with (V1) - [R1] - |L1| - GND
        // See https://en.wikipedia.org/wiki/RL_circuit#Time-domain_considerations
        Circuit c;
        ElementSet eset(&c, 2, 2);
        QCOMPARE(eset.cnodeCount(), 2);
        QCOMPARE(eset.cbranchCount(), 2);

        const double dt = 1./10000.;
        VoltagePoint v1(12.0);
        Resistance r1(10);
        Inductance l1(47e-3, dt);
        QVERIFY(l1.type() == Element::Element_Inductance);
        eset.addElement(&v1);
        eset.addElement(&r1);
        eset.addElement(&l1);
        QVERIFY(!eset.containsNonLinear());

        v1.setCNodes(0);
        v1.setCBranches(0);
        r1.setCNodes(0, 1);
        l1.setCNodes(1, -1);
        l1.setCBranches(1);

        eset.createMatrixMap();

        v1.add_initial_dc();
        r1.add_initial_dc();
        l1.add_initial_dc();

        const double tolerance = 0.05;
        const double R_L = r1.resistance()/l1.inductance();
        for (uint i = 1; i < 10000; i++) {
            l1.time_step();
            eset.doLinear(true);
            if (i == 1) {
                eset.displayEquations();
            }
            const double t = i * dt;
            const double Vl = 12 * std::exp(-t*R_L);
            const double I = (12-Vl)/r1.resistance();
            if (i == 1 || i % 100 == 0) {
                qDebug() << "t=" << t << "Vl=" << Vl << "V1=" << eset.cnodes()[1]->v
                << "I=" << I << "I1=" << eset.cbranches()[0]->i << "I2=" << eset.cbranches()[1]->i;
            }
            QCOMPARE_TOL(eset.cnodes()[0]->v, 12, tolerance);
            QCOMPARE_TOL(eset.cnodes()[1]->v, Vl, tolerance);
            QCOMPARE_TOL(eset.cbranches()[0]->i, I,  tolerance);
            QCOMPARE_TOL(eset.cbranches()[1]->i, I, tolerance);
        }

        QCOMPARE_TOL(eset.cnodes()[1]->v, 0, tolerance);
        QCOMPARE_TOL(eset.cbranches()[0]->i, 1.2, tolerance);
    }

    void diodeForwardBiasTest1()
    {
        // Circuit with (V1) - [D1] - |R1| - GND
        // This checks the diode in forward biased region
        // See https://en.wikipedia.org/wiki/Diode_modelling
        Circuit c;
        ElementSet eset(&c, 2, 1);
        QCOMPARE(eset.cnodeCount(), 2);
        QCOMPARE(eset.cbranchCount(), 1);

        VoltagePoint v1(12.0);
        Resistance r1(100);
        // Is = 10e-12 for Silicon diode
        Diode d1({1e-12, 1.0, 4.7});
        QVERIFY(d1.type() == Element::Element_Diode);
        eset.addElement(&v1);
        eset.addElement(&r1);
        eset.addElement(&d1);
        QVERIFY(eset.containsNonLinear());

        v1.setCNodes(0);
        v1.setCBranches(0);
        d1.setCNodes(0, 1);
        r1.setCNodes(1, -1);

        eset.createMatrixMap();

        v1.add_initial_dc();
        r1.add_initial_dc();
        d1.add_initial_dc();

        eset.doNonLinear(150);
        eset.displayEquations();

        const double R = r1.resistance();
        const double Is = d1.settings().I_S;
        const double n = d1.settings().N;
        const double Vs = 12;
        const double nVT = n*V_T;
        const double tolerance = 0.05;
        const double I = nVT/R*lambert(Is*R/nVT*std::exp(Vs/nVT));
        const double Vr = I*R;
        const double Vd = Vs-Vr;
        qDebug() << "Vd=" << Vd << "Vr=" << Vr  << "V1=" << eset.cnodes()[1]->v
        << "I=" << I << "I1=" << eset.cbranches()[0]->i;
        QCOMPARE_TOL(eset.cnodes()[0]->v, Vs, tolerance);
        QCOMPARE_TOL(eset.cnodes()[1]->v, Vr, tolerance);
        QCOMPARE_TOL(eset.cbranches()[0]->i, I, tolerance);
    }

    void diodeReverseBiasTest1()
    {
        // Circuit with (V1) - [D1] - |R1| - GND
        // This checks the diode in reverse biased region
        // See https://en.wikipedia.org/wiki/Diode_modelling
        Circuit c;
        ElementSet eset(&c, 2, 1);
        QCOMPARE(eset.cnodeCount(), 2);
        QCOMPARE(eset.cbranchCount(), 1);

        VoltagePoint v1(-3.0);
        Resistance r1(100);
        Diode d1;
        eset.addElement(&v1);
        eset.addElement(&r1);
        eset.addElement(&d1);
        QVERIFY(eset.containsNonLinear());

        v1.setCNodes(0);
        v1.setCBranches(0);
        d1.setCNodes(0, 1);
        r1.setCNodes(1, -1);

        eset.createMatrixMap();

        v1.add_initial_dc();
        r1.add_initial_dc();
        d1.add_initial_dc();

        eset.doNonLinear(150);
        eset.displayEquations();

        QCOMPARE(eset.cnodes()[0]->v, -3.0);
        QCOMPARE(eset.cnodes()[1]->v, 0.0);
        QCOMPARE_TOL(eset.cbranches()[0]->i, 0, 1e-6);
    }

    void diodeReverseBreakdownTest1()
    {
        // Circuit with (V1) - [D1] - |R1| - GND
        // This checks the diode in reverse breakdown region
        // See https://en.wikipedia.org/wiki/Diode_modelling
        Circuit c;
        ElementSet eset(&c, 2, 1);
        QCOMPARE(eset.cnodeCount(), 2);
        QCOMPARE(eset.cbranchCount(), 1);

        VoltagePoint v1(-12.0);
        Resistance r1(100);
        Diode d1({1e-12, 1.0, 6.0});
        eset.addElement(&v1);
        eset.addElement(&r1);
        eset.addElement(&d1);
        QVERIFY(eset.containsNonLinear());

        v1.setCNodes(0);
        v1.setCBranches(0);
        d1.setCNodes(0, 1);
        r1.setCNodes(1, -1);

        eset.createMatrixMap();

        v1.add_initial_dc();
        r1.add_initial_dc();
        d1.add_initial_dc();

        eset.doNonLinear(150);
        eset.displayEquations();

        const double R = r1.resistance();
        const double Is = d1.settings().I_S;
        const double Vb = d1.settings().V_B;
        const double n = d1.settings().N;
        const double Vs = -12;
        const double nVT = n*V_T;
        const double tolerance = 0.005;
        const double I = -nVT/R*lambert(Is*R/nVT*std::exp(-(Vs+Vb)/nVT));
        const double Vr = I*R;
        const double Vd = Vs-Vr;
        qDebug() << "Vd=" << Vd << "Vr=" << Vr  << "V1=" << eset.cnodes()[1]->v
        << "I=" << I << "I1=" << eset.cbranches()[0]->i;

        QCOMPARE_TOL(eset.cnodes()[0]->v, Vs, tolerance);
        QCOMPARE_TOL(eset.cnodes()[1]->v, Vr, tolerance);
        QCOMPARE_TOL(eset.cbranches()[0]->i, I, tolerance);
    }

    void cleanupTestCase()
    {
    }
};

QTEST_MAIN(KtlElectronicsTests)
#include "test_electronics.moc"

