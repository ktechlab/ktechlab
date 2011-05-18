/*
 *    Test automatic_router plugin
 *    Copyright (C) 2010 Julian Bäume <julian@svg4all.de>
 * 
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 * 
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 * 
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 */

#include "automaticroutertest.h"
#include <qtest_kde.h>
#include <tests/autotestshell.h>
#include <tests/testcore.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/irouterplugin.h>
#include <interfaces/iroutinginformation.h>
#include <plugins/circuit/circuitscene.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/icomponentdocument.h>

using namespace KTechLab;

void KTechLab::AutomaticRouterTest::initTestCase()
{
    qDebug("creating KDevelop environment");
    KDevelop::AutoTestShell::init();
    m_core = new KDevelop::TestCore();
    m_core->initialize( KDevelop::Core::Default );

    QVERIFY( m_core->pluginController() );
    m_router = m_core->pluginController()->extensionForPlugin<IRouterPlugin>("org.ktechlab.IRouterPlugin", "ktlautomatic_router");
    QVERIFY( m_router );
    initDocumentScenes();
    m_router->setDocumentScene(m_testScenes.value("test.circuit"));
    m_currentScene = m_testScenes.value("test.circuit");
}
void AutomaticRouterTest::cleanupTestCase()
{
    m_core->cleanup();
    delete m_core;
}
void AutomaticRouterTest::initDocumentScenes()
{
    KUrl testUrl( QString(PROJECTS_SOURCE_DIR)+QString("/test-project/test.circuit") );
    IComponentDocument* doc = dynamic_cast<IComponentDocument*>(m_core->documentController()->openDocument(testUrl));
    QVERIFY( doc->isActive() );

    m_testScenes.insert("test.circuit",doc->documentScene());
}

void AutomaticRouterTest::mapLineRoute()
{
    QPointF p1(360, 104);
    QPointF p2(360, 64);
    m_currentScene->routingInfo()->mapRoute(p1,p2);

    foreach(const QPointF &p, m_currentScene->routingInfo()->route()){
        QVERIFY( p.x() == 360 );
    }

    // should fail, because this is outside the bounding-rect of the scene
    // so the route should be empty
    p1 = QPointF(0,0);
    p2 = QPointF(100,0);
    m_currentScene->routingInfo()->mapRoute(p1,p2);
    QVERIFY(m_currentScene->routingInfo()->route().isEmpty());
}

void AutomaticRouterTest::visualizeData()
{
    QRectF sceneRect = m_currentScene->sceneRect();
    QImage i = m_currentScene->routingInfo()->visualizedData(sceneRect).toImage();
    QVERIFY( i.size() == sceneRect.size() );
    i = m_currentScene->routingInfo()->visualizedData(sceneRect.adjusted(50,15,-50,-15)).toImage();
    QVERIFY( i.size() == sceneRect.adjusted(50,15,-50,-15).size() );
    i = m_currentScene->routingInfo()->visualizedData(sceneRect.adjusted(-50,-15,50,15)).toImage();
    QVERIFY( i.size() == sceneRect.adjusted(-50,-15,50,15).size() );
    // TODO: compare images pixel-wise
}

QTEST_KDEMAIN(AutomaticRouterTest, GUI)
#include "automaticroutertest.moc"
