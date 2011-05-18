/*
    KTechLab, and IDE for electronics
    Copyright (C) 2010 Zoltan Padrah

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "directsimulatortest.h"

#include <simulationmanager.h>
#include <interfaces/idocumentmodel.h>
#include <interfaces/simulator/isimulationmanager.h>
#include <circuit/simulator/genericelementfactory.h>
#include <plugins/simulator/circuittransientsimulator.h>
#include <plugins/basic_ec/elements/resistance.h>
#include <plugins/basic_ec/elements/capacitance.h>
#include <plugins/basic_ec/elements/voltagesource.h>

#include <QtTest/QtTest>

using namespace KTechLab;


class TestElementFactory : public GenericElementFactory
{
protected:
    virtual IElement * createOrRegister(bool create, const QByteArray& type,
                                        QVariantMap parentInModel = QVariantMap())
    {
        SUPPORT_ELEMENT(Resistance,"Resistance")
        SUPPORT_ELEMENT(Capacitance,"Capacitance")
        SUPPORT_ELEMENT(VoltageSource,"VoltageSource")
        return 0;
    }
};

TestElementFactory *fact;

void KTechLab::DirectSimulatorTest::initTestCase()
{
    SimulationManager::initialize();
    fact = new TestElementFactory();
    ISimulationManager::self()->registerElementFactory(fact);
    model = new IDocumentModel(QDomDocument("KTechlab"));
    transSim = new CircuitTransientSimulator(model);
    simulator = transSim;
    model = 0;
}

void KTechLab::DirectSimulatorTest::cleanupTestCase()
{
    delete model;
    delete simulator;
    ISimulationManager::self()->unregisterElementFactory(fact);
    delete fact;
}

void DirectSimulatorTest::init()
{
    // clean up the model
    /* HACK the model has no methods to remove
        components from it, so the model is recreated
        every time before a new test case
        */
    if(model)
        delete model;

    model = new IDocumentModel(QDomDocument("KTechlab"));
    transSim = new CircuitTransientSimulator(model);
    simulator = transSim;
}

void KTechLab::DirectSimulatorTest::addResistor()
{
    QVariantMap r1;
    r1.insert("id", "R1");
    r1.insert("type", "Resistance");
    model->addComponent(r1);

    simulator->documentStructureChanged();
    transSim->simulationTimerTicked();
    transSim->dumpDebugInfo();
}

void DirectSimulatorTest::addCapacitor()
{
    QVariantMap c1;
    c1.insert("id", "C1");
    c1.insert("type", "Capacitance");
    model->addComponent(c1);

    simulator->documentStructureChanged();
    transSim->simulationTimerTicked();
    transSim->dumpDebugInfo();
}

void DirectSimulatorTest::addVoltageSource()
{
    QVariantMap v1;
    v1.insert("id", "V1");
    v1.insert("type", "VoltageSource");
    model->addComponent(v1);

    simulator->documentStructureChanged();
    transSim->simulationTimerTicked();
    transSim->dumpDebugInfo();
}

void DirectSimulatorTest::resistorAndSource()
{
    QVariantMap v1;
    v1.insert("id", "V1");
    v1.insert("type", "VoltageSource");

    QVariantMap r1;
    r1.insert("id", "R1");
    r1.insert("type", "Resistance");

    QVariantMap con1;
    con1.insert("id", "con1");
    con1.insert("start-node-is-child", "1");
    con1.insert("start-node-parent", "R1");
    con1.insert("start-node-cid", "p1");
    con1.insert("end-node-is-child", "1");
    con1.insert("end-node-parent", "V1");
    con1.insert("end-node-cid", "p1");

    QVariantMap con2;
    con2.insert("id", "con2");
    con2.insert("start-node-is-child", "1");
    con2.insert("start-node-parent", "R1");
    con2.insert("start-node-cid", "n1");
    con2.insert("end-node-is-child", "1");
    con2.insert("end-node-parent", "V1");
    con2.insert("end-node-cid", "n1");

    model->addComponent(v1);
    model->addComponent(r1);
    model->addConnector(con1);
    model->addConnector(con2);

    simulator->documentStructureChanged();
    transSim->simulationTimerTicked();
    transSim->dumpDebugInfo();
}

QTEST_MAIN(DirectSimulatorTest)
#include "directsimulatortest.moc"
