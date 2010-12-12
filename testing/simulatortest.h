#ifndef SIMULATORTEST_H_
#define SIMULATORTEST_H_

#include <QtTest/QtTest>


class SimulatorTest : public QObject {
    Q_OBJECT

private slots:

    void createTest();
    void testSourceAndResistance();
    void testSourceAnd4ResistanceInParallel();

    void testComponent_SourceAndResistor();
};

#endif

