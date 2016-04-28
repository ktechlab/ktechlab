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
#include "dummyfactory.h"

#include "circuit/circuitmodel.h"
#include "plugins/circuit/circuitdocument.h"
#include "interfaces/idocumentplugin.h"
#include "interfaces/simulator/isimulationmanager.h"
#include "simulationmanager.h"

#include <interfaces/idocumentcontroller.h>
// #include <interfaces/idocumentfactory.h>
#include <interfaces/iplugin.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/ipartcontroller.h>

#include <tests/autotestshell.h>
#include <tests/testcore.h>
#include <tests/kdevsignalspy.h>

#include <QtTest/QTest>
#include <qtest_kde.h>

#include <QDebug>

#include <iostream>

using namespace KTechLab;
using namespace KDevelop;

void AddComponentsTest::initTestCase()
{
    qDebug("creating KDevelop environment");
    // simulation manager
    SimulationManager::initialize();
    AutoTestShell::init();
    m_core = new KDevelop::TestCore();
    
    QVERIFY( m_core );
    
    m_core->initialize( KDevelop::Core::Default );
    
    
    m_simManager = ISimulationManager::self();
    QVERIFY( m_simManager );
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
        m_core->pluginController()->loadPlugin("ktlcircuit");

    QVERIFY( ktlcircuit );
    
    KDevelop::IPlugin * ktlsimulator =
        m_core->pluginController()->loadPlugin("ktlsimulator");

    QVERIFY( ktlsimulator );

}

void AddComponentsTest::openDocument(){

    KUrl circuitUrl(
        QString(PROJECTS_SOURCE_DIR)+
        QString("/test-project/test.circuit") );
    
    QVERIFY( QFile(circuitUrl.toLocalFile()).exists() );

    KDevelop::IDocument * newdoc = 
        m_core->documentController()->openDocument( circuitUrl, "ktlcircuit");
    
    int openDocuments = m_core->documentController()->openDocuments().size() ;
    
    qDebug() << "open document count: " << openDocuments ;
    QVERIFY( openDocuments == 1 );
    
    qDebug() << "mime name: " << newdoc->mimeType()->name() ;
    qDebug() << "trying to close document...";
    
    m_core->documentController()->closeAllDocuments();
    
    openDocuments = m_core->documentController()->openDocuments().size() ;
    
    qDebug() << "open document count: " << openDocuments ;
    QVERIFY( openDocuments == 0 );

}

void AddComponentsTest::seeSimulationManagerStatus(){
    qDebug() << "status: \n";
    qDebug() << "mime types names: " << m_simManager->registeredDocumentMimeTypeNames();
    qDebug() << "registered factores: " << m_simManager->registeredFactories();
    qDebug() << "simulation types: " << m_simManager->registeredSimulationTypes() ;
}
    
void AddComponentsTest::elementFactoryTest(){
    qDebug() << "starting\n";

    m_core->pluginController()->unloadPlugin("ktlsimulator");
    m_core->pluginController()->unloadPlugin("ktlbasicec");
    
    // qDebug() << "before adding factory: \n";
    QVERIFY( m_simManager->registeredDocumentMimeTypeNames().size() == 0);
    QVERIFY( m_simManager->registeredSimulationTypes().size() == 0);
    QVERIFY( m_simManager->registeredFactories().size()== 0 );
  
    DummyElementFactory *f = new DummyElementFactory();
    m_simManager->registerElementFactory(f);
    
    QVERIFY( m_simManager->registeredFactories().size() == 1);
    
    // try to create a dummu
    IElementFactory *fgot;
    // no such simulation...
    QList<IElementFactory *> flist = m_simManager->registeredFactories("foobar");
    QVERIFY( flist.size() == 0 );
    // this one should exist
    flist = m_simManager->registeredFactories("dummy-simulation");
    QVERIFY( flist.size() == 1 );
        // make sure it's not corrupted
    fgot = flist.first();
    QVERIFY( fgot->supportedDocumentMimeTypeName() == "dummy-mimetype");
    
    // try to create a dummy
    
        // nonexistent
    IElement *elem = fgot->createElement("whatever", QVariantMap());
    QVERIFY( !elem );
        // existent
    elem = fgot->createElement("dummy-element", QVariantMap());
    QVERIFY( elem );
    DummyElement *delem = dynamic_cast<DummyElement*>(elem);
    QVERIFY( delem );
    
    // call a method of it
    delem->dummyMethod();
    
    m_simManager->unregisterElementFactory(f);
    
    //qDebug() << "\n\nafter removed factory: \n";
    QVERIFY( m_simManager->registeredDocumentMimeTypeNames().size() == 0);
    QVERIFY( m_simManager->registeredSimulationTypes().size() == 0);
    QVERIFY( m_simManager->registeredFactories().size() == 0 );
    
    // cleanup
    delete f;
    delete delem;
    
    qDebug() << "done\n";

}

void AddComponentsTest::getPluginObject(){
  
#if KDEV_PLUGIN_VERSION < 17
    QStringList constraints;
    constraints << QString("'%1' in [X-KDevelop-SupportedMimeTypes]")
        .arg("application/x-circuit");
#else
    QVariantMap constraints;
    constraints.insert("X-KDevelop-SupportedMimeTypes", "application/x-circuit");
#endif

    QList<KDevelop::IPlugin*> plugins =
        KDevelop::Core::self()->pluginController()
            ->allPluginsForExtension( "org.kdevelop.IDocument", constraints );

    QVERIFY( ! plugins.isEmpty()) ;

    IDocumentPlugin *plugin = qobject_cast<IDocumentPlugin*>( plugins.first() );

    QVERIFY( plugin );
    qDebug() << "circuit plugin not null\n";
}

void AddComponentsTest::addResistor(){

    // suppose we have no open documents
    QVERIFY( m_core->documentController()->openDocuments().size() == 0);

    // load simulation plugin
    IPlugin *simplugin = m_core->pluginController()->loadPlugin("ktlsimulator");
    QVERIFY( simplugin );
    IPlugin *basic_ec = m_core->pluginController()->loadPlugin("ktlbasicec");
    QVERIFY( basic_ec );

    // see the situation
    seeSimulationManagerStatus();

    // create a temporary file
    QTemporaryFile tempFile(QDir::tempPath() + "/circuitXXXXXX.circuit");
    tempFile.setAutoRemove(false);
    // open it
    bool tempOK = tempFile.open();
    QVERIFY( tempOK );
    
    // get the URL of the file
    KUrl docUrl( tempFile.fileName() );

    // open that URL
    KDevelop::IDocument *mydoc = m_core->documentController()->openDocument(docUrl);
    QVERIFY( mydoc );
    QVERIFY( m_core->documentController()->openDocuments().size() == 1);

    qDebug() << "document mimetype name: " << mydoc->mimeType()->name() ;
    
    QVERIFY( mydoc->mimeType()->name().compare("application/x-circuit") == 0 );
    
    IComponentDocument *doc = dynamic_cast<IComponentDocument*>( mydoc );
    QVERIFY( doc );
    
    IDocumentModel* model = dynamic_cast<IDocumentModel*>( doc->documentModel() );
    QVERIFY( model );
        
    QVariantMap* r1 = new QVariantMap();
    r1->insert("type", QString("ec_resistor"));
    r1->insert("id","resistor1");
        
    model->addComponent(*r1); // take that !
    
    // this should exist
    QVERIFY( model->components().contains("resistor1") );
    
    // this not 
    QVERIFY( model->components().contains("r1") == false );
    
    // create a simulator for the document
    ISimulator *sim = m_simManager->simulatorForDocument(doc, "transient");
    QVERIFY( sim );
    
    // clean up a little
    m_core->documentController()->closeAllDocuments();

    tempFile.remove();
    
    QVERIFY( m_core->documentController()->openDocuments().size() == 0);
}

void AddComponentsTest::openTestCircuit()
{
    QString testFilePath(PROJECTS_SOURCE_DIR);
    testFilePath.append("/test-project/test.circuit");
    qDebug() << "file name: " << testFilePath << "\n";
    KUrl testUrl(testFilePath);
        // test if the test is valid
    QFile testFile(testFilePath);
    QVERIFY( testFile.exists() );
        // open the file
    KDevelop::IDocument *testdoc = m_core->documentController()->openDocument(testUrl);
    QVERIFY(testdoc);
        // cast to component document
    IComponentDocument *compDoc = dynamic_cast<IComponentDocument*>(testdoc);
    QVERIFY(compDoc);
        // get the model
    IDocumentModel *model = compDoc->documentModel();
    QVERIFY( model );
        // dump the model
    QVariantMap components = model->components();
    qDebug() << "components in the test file:\n";
    foreach(QVariant modelVariant, components){
        qDebug() << modelVariant << "\n";
    }
    QVariantMap connectors = model->connectors();
    foreach(QVariant modelVariant, connectors){
        qDebug() << modelVariant << "\n";
    }
    
}


QTEST_KDEMAIN(AddComponentsTest , GUI)
#include "addcomponents.moc"
