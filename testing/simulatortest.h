#ifndef SIMULATORTEST_H_
#define SIMULATORTEST_H_

#include <QtTest/QtTest>


class SimulatorTest : public QObject {
    Q_OBJECT

private slots:

    // method called before each test case
    void init();

    void createTest();
    void testSourceAndResistance();
    void testSourceAnd4ResistanceInParallel();
};

#endif

