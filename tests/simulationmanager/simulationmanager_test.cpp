
#include "simulationmanager_test.h"

#include <QtTest/QtTest>

#include "simulationmanager.cpp"

using namespace KTechLab;

void SimulationManagerTests::test_simulatorKey() {
    KUrl theUrl("someUrl");
    SimulatorMapKey key1(theUrl.url(), QString("sim"));

    QMap<SimulatorMapKey, bool> simulatorMap;
    simulatorMap.insert(key1, true);

    SimulatorMapKey key2(theUrl.url(), QString("sim"));

    bool val = simulatorMap.contains(key2);
    QVERIFY(val);
}

QTEST_MAIN(SimulationManagerTests)
