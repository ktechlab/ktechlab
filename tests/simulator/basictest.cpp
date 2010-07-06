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

#include "basictest.h"

#include <interfaces/iplugin.h>
#include <interfaces/iplugincontroller.h>

#include <tests/autotestshell.h>
#include <tests/testcore.h>
#include <tests/kdevsignalspy.h>

#include <QtTest/QTest>
#include <qtest_kde.h>

#include <QDebug>

using namespace KTechLab;
using namespace KDevelop;

void BasicTest::initTestCase()
{
    qDebug("creating KDevelop environment");
    AutoTestShell::init();
    m_core = new KDevelop::TestCore();
    
    QVERIFY( m_core );
    
    m_core->initialize( KDevelop::Core::Default );
}

void BasicTest::cleanupTestCase()
{
    m_core->cleanup();
    delete m_core;
}

void BasicTest::listPlugins(){
    qDebug("listing ktlcircuit plugins");

    QVERIFY( m_core->pluginController() );

    KDevelop::IPlugin * ktlcircuit = 
        m_core->pluginController()
        ->loadPlugin("ktlcircuit");
            
    QVERIFY( ktlcircuit );
        
    qDebug() << ktlcircuit << "\n";

}

QTEST_KDEMAIN(BasicTest , GUI)
#include "basictest.moc"
