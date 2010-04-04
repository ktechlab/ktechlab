/*
    Test handling of KTechLab projects
    Copyright (C) 2010 Julian Bäume <julian@svg4all.de>

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

#include "addcomponents.h"

#include "plugins/circuit/circuitmodel.h"

#include <interfaces/iplugin.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/idocumentcontroller.h>

#include <tests/autotestshell.h>
#include <tests/testcore.h>
#include <tests/kdevsignalspy.h>

#include <QtTest/QTest>
#include <qtest_kde.h>

#include <QDebug>

#include <iostream>

using namespace KTechLab;

void AddComponentsTest::initTestCase()
{
    qDebug("creating KDevelop environment");
    AutoTestShell::init();
    m_core = new KDevelop::TestCore();
    
    QVERIFY( m_core );
    
    m_core->initialize( KDevelop::Core::Default );
}

void AddComponentsTest::cleanupTestCase()
{
    m_core->cleanup();
    delete m_core;
}

void AddComponentsTest::listPlugins(){
    qDebug("listing ktlcircuit plugins");

    QVERIFY( m_core->pluginController() );

    KDevelop::IPlugin * ktlcircuit = 
        m_core->pluginController()
        ->loadPlugin("ktlcircuit");

    QVERIFY( ktlcircuit );

    qDebug() << ktlcircuit ;

}

void AddComponentsTest::openDocument(){

    KUrl circuitUrl(
        QString(PROJECTS_SOURCE_DIR)+
        QString("/test-project/test.circuit") );
    
    QVERIFY( QFile(circuitUrl.toLocalFile()).exists() );

    m_core->documentController()->openDocument( circuitUrl, "ktlcircuit");
    
    int openDocuments = m_core->documentController()->openDocuments().size() ;
    
    qDebug() << "open document count: " << openDocuments ;
    QVERIFY( openDocuments == 1 );
}

void AddComponentsTest::addResistor(){
/*  
  
            
    QVariantMap r1; // = new QVariantMap();
    r1.insert("type", QString("ec_resistor"));
    r1.insert("id","resistor1");
            
    model->addComponent(r1); // take that !
  */
}

QTEST_KDEMAIN(AddComponentsTest , GUI)
#include "addcomponents.moc"
