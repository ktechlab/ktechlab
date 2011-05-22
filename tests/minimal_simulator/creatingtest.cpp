/*
    KTechLab, an IDE for electronics
    Copyright (C) 2010 Zoltan Padrah
      zoltan_padrah@users.sourceforge.net

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

#include "creatingtest.h"
#include <circuit.h>
#include <ecvoltagesource.h>
#include <resistor.h>
#include <electronicconnector.h>
#include <ecnode.h>

#define private public
#include <simulator.h>
#undef private

#include <QDebug>
#include <QtTest/QtTest>
#include <ktlconfig.h>


#define MAX_CURRENT_ERROR 1e-9

#define ASSERT_DOUBLE_EQUALS(expr,val) \
    Q_ASSERT( qAbs( (expr) - (val) ) < MAX_CURRENT_ERROR)


void CreatingTest::cleanupTestCase()
{
    qDebug() << "Cleaning up...";
    KtlConfig::destroy();
    Simulator::destroy();
}


void CreatingTest::initAndStep(Circuit* circ)
{
    Q_ASSERT(circ);

    circ->init();

    Simulator * s = Simulator::self();
    s->attachCircuit(circ);
    s->slotSetSimulating(true);
    s->step();
    circ->updateCurrents();
    s->slotSetSimulating(false);
}

void CreatingTest::emptyCircuitTest()
{
    Circuit circ;

    circ.init();

    Simulator::self()->attachCircuit(&circ);
    Simulator::self()->slotSetSimulating(true);
    Simulator::self()->step();
    circ.updateCurrents();

    Simulator::self()->slotSetSimulating(false);
    Simulator::self()->detachCircuit(&circ);
}

void CreatingTest::localVariablesTest()
{
    {
        Circuit circ;
        ECCell v1(circ);
        Resistor r1(circ);
        ElectronicConnector cc1(v1.pinByName("n1"), r1.pinByName("n1"));
        ElectronicConnector cc2(v1.pinByName("p1"), r1.pinByName("p1"));

        circ.init();

        Simulator::self()->attachCircuit(&circ);
        Simulator::self()->slotSetSimulating(true);
        Simulator::self()->step();
        circ.updateCurrents();
        Simulator::self()->slotSetSimulating(false);

        qDebug() << "cc1 current: " << cc1.wire(0)->current();
        qDebug() << "cc2 current: " << cc2.wire(0)->current();
        qDebug() << "v1 gives current: " << v1.pinByName("n1")->pin()->sourceCurrent();

        Q_ASSERT(qAbs(qAbs(cc1.wire(0)->current()) - 5) < MAX_CURRENT_ERROR);
        Q_ASSERT(qAbs(qAbs(cc2.wire(0)->current()) - 5) < MAX_CURRENT_ERROR);
        Q_ASSERT(qAbs(cc1.wire(0)->current() + cc2.wire(0)->current()) < MAX_CURRENT_ERROR);


        ASSERT_DOUBLE_EQUALS(v1.pinByName("n1")->pin()->sourceCurrent(), 5);
        Q_ASSERT(circ.components().size() == 2);

        {
            Resistor r2(circ);
            // FIXME in the current state, these lines of code create invalid circuit structure
            ElectronicConnector cc3(r1.pinByName("n1"), r2.pinByName("n1"));
            ElectronicConnector cc4(r1.pinByName("p1"), r2.pinByName("p1"));

            initAndStep(&circ);

            qDebug() << "r2 pin voltage:"
                << r2.pinByName("n1")->pin()->voltage()
                << " and "
                << r2.pinByName("p1")->pin()->voltage();
            qDebug() << "cc3 current: " << cc3.wire(0)->current();
            qDebug() << "cc4 current: " << cc4.wire(0)->current();
            qDebug() << "v1 gives current: " << v1.pinByName("n1")->pin()->sourceCurrent();

            ASSERT_DOUBLE_EQUALS(v1.pinByName("n1")->pin()->sourceCurrent(), 10);
            Q_ASSERT(circ.components().size() == 3);
        }
        Q_ASSERT(circ.components().size() == 2);
        {
            Resistor *r3 = new Resistor(circ);
            ElectronicConnector *cc6;
            cc6 = new ElectronicConnector(v1.pinByName("n1"), r3->pinByName("n1"));

            QSignalSpy destroyedSpy(cc6, SIGNAL( destroyed()));
            QVERIFY( destroyedSpy.isValid() );


            initAndStep(&circ);

            Q_ASSERT( destroyedSpy.count() == 0);
            delete r3;
            Q_ASSERT( destroyedSpy.count() == 1);

        }
        Q_ASSERT(circ.components().size() == 2);
    }
    Q_ASSERT(Simulator::self()->m_ordinaryCircuits->size() == 0);
}

QTEST_MAIN(CreatingTest)
// #include "creatingtest.moc"
