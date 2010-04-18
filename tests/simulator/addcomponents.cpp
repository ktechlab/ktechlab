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
#include "plugins/circuit/circuitdocument.h"
#include "interfaces/idocumentplugin.h"

#include <interfaces/iplugin.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/ipartcontroller.h>
// #include <interfaces/idocumentfactory.h>

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

    KDevelop::IDocument * newdoc = 
        m_core->documentController()->openDocument( circuitUrl, "ktlcircuit");
    
    int openDocuments = m_core->documentController()->openDocuments().size() ;
    
    qDebug() << "open document count: " << openDocuments ;
    QVERIFY( openDocuments == 1 );
    
    qDebug() << "mime comment: " << newdoc->mimeType()->comment() ;

    qDebug() << "trying to close document...";
    
    m_core->documentController()->closeAllDocuments();
    
    openDocuments = m_core->documentController()->openDocuments().size() ;
    
    qDebug() << "open document count: " << openDocuments ;
    QVERIFY( openDocuments == 0 );

}


void AddComponentsTest::getPluginObject(){
  
    QStringList constraints;

    constraints << QString("'%1' in [X-KDevelop-SupportedMimeTypes]")
        .arg("application/x-circuit");

    QList<KDevelop::IPlugin*> plugins =
        KDevelop::Core::self()->pluginController()
            ->allPluginsForExtension( "org.kdevelop.IDocument", constraints );

    QVERIFY( ! plugins.isEmpty()) ;

    IDocumentPlugin *plugin = qobject_cast<IDocumentPlugin*>( plugins.first() );

    qDebug() << plugin;
}

void AddComponentsTest::addResistor(){

    // suppose we have no open documents
    QVERIFY( m_core->documentController()->openDocuments().size() == 0);

    //is there a better way to create a document ?
    KDevelop::IDocumentFactory * fact =
       m_core->documentController()->factory("application/x-circuit");
   
    qDebug() << "factory = " << fact ;
   
    KUrl emptyDoc( i18n("Untitled") ); 
   
    KDevelop::IDocument *mydoc = fact->create( emptyDoc, m_core);
   
    qDebug() << "document: " << mydoc ;
    
    // QVERIFY( m_core->documentController()->openDocuments().size() == 1);
   

   
    
    // something like this would be better
    /*
    KDevelop::IDocument * newdoc = 
            m_core->documentController()
                ->openDocument( emptyDoc , "ktlcircuit" );
    
    qDebug() << "newdoc: " << newdoc ;
    */

    qDebug() << "document mime comment: " << mydoc->mimeType()->patterns() ;

    /*
    CircuitDocument *doc = dynamic_cast<CircuitDocument *>( mydoc );
    QVERIFY( doc );
    
    CircuitModel * model = dynamic_cast<CircuitModel *>( doc->model() );
    QVERIFY( model );
    
    QVariantMap r1; // = new QVariantMap();
    r1.insert("type", QString("ec_resistor"));
    r1.insert("id","resistor1");
        
    model->addComponent(r1); // take that !
    */
    
    // clean up a little
    m_core->documentController()->closeAllDocuments();
    
    QVERIFY( m_core->documentController()->openDocuments().size() == 0);
}

QTEST_KDEMAIN(AddComponentsTest , GUI)
#include "addcomponents.moc"
