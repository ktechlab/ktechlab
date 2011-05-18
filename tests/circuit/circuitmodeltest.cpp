/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include "circuitmodeltest.h"

#include <circuit/circuitmodel.h>

#include <KIO/NetAccess>
#include <KMessageBox>
#include <QtTest/QTest>
#include <qtest_kde.h>

void CircuitModelTest::initTestCase()
{
    model = 0;
}

void CircuitModelTest::initNew()
{
    QVERIFY(!model);

    model = new KTechLab::CircuitModel(QDomDocument("KTechlab"));

    delete model;
    model = 0;
}

void CircuitModelTest::loadFromFile()
{
    QVERIFY(!model);

    KUrl url(QByteArray(PROJECTS_SOURCE_DIR)+"/test-project/test.circuit");

    QString errorMessage, tempFile;
    QVERIFY( KIO::NetAccess::download( url, tempFile, 0 ) );

    QFile file(tempFile);
    QVERIFY(file.open(QIODevice::ReadOnly));

    QDomDocument dom( "KTechlab" );
    QVERIFY( dom.setContent( &file, &errorMessage ) );
    file.close();
    KIO::NetAccess::removeTempFile(tempFile);
    model = new KTechLab::CircuitModel( dom );

    QVERIFY( model->rowCount(model->index(0,0)) == 8 );

    delete model;
    model = 0;
}

QTEST_KDEMAIN(CircuitModelTest, GUI)
#include "circuitmodeltest.moc"
