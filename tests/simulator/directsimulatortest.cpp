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
#include <interfaces/simulator/genericelementfactory.h>
#include <plugins/simulator/circuittransientsimulator.h>
#include <plugins/basic_ec/elements/resistance.h>
#include <plugins/basic_ec/elements/capacitance.h>

#include <QtTest/QtTest>

using namespace KTechLab;


DECLARE_ELEMENT_FACTORY(
    TestElementFactory,
    SUPPORT_ELEMENT(Resistance)
    SUPPORT_ELEMENT(Capacitance)
    );

TestElementFactory *fact;

void KTechLab::DirectSimulatorTest::initTestCase()
{
    SimulationManager::initialize();
    fact = new TestElementFactory();
    ISimulationManager::self()->registerElementFactory(fact);
    model = new IDocumentModel;
    transSim = new CircuitTransientSimulator(model);
    simulator = transSim;
}

void KTechLab::DirectSimulatorTest::cleanupTestCase()
{
    delete model;
    delete simulator;
    ISimulationManager::self()->unregisterElementFactory(fact);
    delete fact;
}

void KTechLab::DirectSimulatorTest::addResistor()
{
    QVariantMap r1;
    r1.insert("id", "R1");
    r1.insert("type", "Resistance");
    model->addComponent(r1);

    simulator->documentStructureChanged();
    transSim->simulationTimerTicked();
}

QTEST_MAIN(DirectSimulatorTest)
#include "directsimulatortest.moc"
