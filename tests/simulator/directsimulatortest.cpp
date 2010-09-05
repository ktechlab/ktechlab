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

#include <interfaces/idocumentmodel.h>
#include <plugins/simulator/circuittransientsimulator.h>

#include <QtTest/QtTest>

using namespace KTechLab;


void KTechLab::DirectSimulatorTest::initTestCase()
{
    model = new IDocumentModel;
    transSim = new CircuitTransientSimulator(model);
    simulator = transSim;
}

void KTechLab::DirectSimulatorTest::cleanupTestCase()
{
    delete model;
    delete simulator;
}

void KTechLab::DirectSimulatorTest::addResistor()
{
    QVariantMap r1;
    r1.insert("type", "Resistor");
    model->addComponent(r1);

    simulator->documentStructureChanged();
    transSim->simulationTimerTicked();
}

QTEST_MAIN(DirectSimulatorTest)
#include "directsimulatortest.moc"
