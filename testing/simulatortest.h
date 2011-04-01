#ifndef SIMULATORTEST_H_
#define SIMULATORTEST_H_

#include <QtTest/QtTest>


class SimulatorTest : public QObject {
    Q_OBJECT

private slots:

    void initTestCase();
    void cleanupTestCase();

    void createTest();
    void testSourceAndResistance();
    void testSourceAnd4ResistanceInParallel();

    void pinReduceTest();
    void pinReduceTestWithGround();

    void testComponent_SourceAndResistor();
    void testComponent_voltageDivider();
    void testComponent_fixedVoltage();
    void testComponent_currentSource();
    void testComponent_capacitor();

};

#endif

