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

#include "ktlprojecttest.h"
#include <tests/autotestshell.h>
#include <tests/testcore.h>
#include <tests/kdevsignalspy.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/iproject.h>
#include <QtTest/QTest>
#include <qtest_kde.h>
#include <KUrl>
#include <QDebug>
#include <project/interfaces/iprojectfilemanager.h>
#include <project/projectmodel.h>
#include <src/simulationmanager.h>

using namespace KTechLab;

void KTLProjectTest::initTestCase()
{
    qDebug("creating KDevelop environment");
    SimulationManager::initialize();
    KDevelop::AutoTestShell::init();
    m_core = new KDevelop::TestCore();
    m_core->initialize( KDevelop::Core::Default );
    m_project = 0;
}

void KTLProjectTest::cleanupTestCase()
{
    m_core->cleanup();
    delete m_core;
}

void KTLProjectTest::loadKTLProject()
{
    qDebug("loading KTechLab project");
    KUrl projectUrl( QString(PROJECTS_SOURCE_DIR)+QString("/test-project/test-project.ktl4") );
    QVERIFY( QFile(projectUrl.toLocalFile()).exists() );
    KDevelop::ICore::self()->projectController()->openProject( projectUrl );
    KDevelop::KDevSignalSpy* projectspy =
            new KDevelop::KDevSignalSpy ( KDevelop::ICore::self()->projectController(),
                                          SIGNAL ( projectOpened ( KDevelop::IProject* ) ) );

    bool projectOpened = projectspy->wait ( 5000 );
    QVERIFY( projectOpened );
    KDevelop::IProject* project = KDevelop::Core::self()->projectController()->projectAt(0);
    QVERIFY( project && project->name() == "test-project" );
    m_project = project;
    delete projectspy;
}

void KTLProjectTest::addSubProject()
{
    if (!m_project)
        QSKIP("no project seems to be loaded", SkipSingle);

    KUrl folderUrl = m_project->folder();
    folderUrl.addPath("addTestFolder");
    KDevelop::IProjectFileManager* manager = m_project->projectFileManager();
    m_testFolderItem = manager->addFolder(folderUrl, m_project->projectItem());

    QVERIFY( QDir(folderUrl.toLocalFile()).exists() );
    QVERIFY( m_project->inProject(folderUrl) );
}

void KTLProjectTest::renameSubProject()
{
    if (!m_project && !m_testFolderItem)
        QSKIP("no project seems to be loaded or test folder not added", SkipSingle);

    KUrl folderUrl = m_project->folder();
    folderUrl.addPath("renameTestFolder");
    KDevelop::IProjectFileManager* manager = m_project->projectFileManager();
    QVERIFY( manager->renameFolder(m_testFolderItem,folderUrl) );

    QVERIFY( QDir(folderUrl.toLocalFile()).exists() );
    QVERIFY( m_project->inProject(folderUrl) );
}

void KTLProjectTest::removeSubProject()
{
    if (!m_project && !m_testFolderItem)
        QSKIP("no project seems to be loaded or test folder not added", SkipSingle);

    KUrl folderUrl = m_project->folder();
    folderUrl.addPath("renameTestFolder");

    KDevelop::IProjectFileManager* manager = m_project->projectFileManager();
    #if KDEV_PLUGIN_VERSION >= 11
    QVERIFY( manager->removeFilesAndFolders(QList<KDevelop::ProjectBaseItem*>() << m_testFolderItem) );
    #else
    QVERIFY( manager->removeFolder(m_testFolderItem));
    #endif

    QVERIFY( !QDir(folderUrl.toLocalFile()).exists() );
    QVERIFY( !m_project->inProject(folderUrl) );
}

void KTLProjectTest::addFile()
{
    if (!m_project)
        QSKIP("no project seems to be loaded", SkipSingle);

    KDevelop::IProjectFileManager* manager = m_project->projectFileManager();
    KUrl newFile = m_project->folder();
    newFile.addPath("unittest.circuit");

    m_testFileItem = manager->addFile(newFile, m_project->projectItem());

    QVERIFY( QFile(newFile.toLocalFile()).exists() );
    QVERIFY( m_project->inProject(newFile) );
}

void KTLProjectTest::renameFile()
{
    if (!m_project && !m_testFileItem)
        QSKIP("no project seems to be loaded or test file not added", SkipSingle);

    KDevelop::IProjectFileManager* manager = m_project->projectFileManager();
    KUrl newFile = m_project->folder();
    newFile.addPath("renameunittest.circuit");

    QVERIFY( manager->renameFile(m_testFileItem, newFile) );

    QVERIFY( QFile(newFile.toLocalFile()).exists() );
    QVERIFY( m_project->inProject(newFile) );
}

void KTLProjectTest::removeFile()
{
    if (!m_project && !m_testFileItem)
        QSKIP("no project seems to be loaded or test file not added", SkipSingle);

    KDevelop::IProjectFileManager* manager = m_project->projectFileManager();

    KUrl fileUrl = m_testFileItem->url();

    #if KDEV_PLUGIN_VERSION >= 11
    QVERIFY( manager->removeFilesAndFolders(QList<KDevelop::ProjectBaseItem*>() << m_testFolderItem) );
    #else
    QVERIFY( manager->removeFolder(m_testFolderItem));
    #endif

    QVERIFY( !QFile(fileUrl.toLocalFile()).exists() );
    QVERIFY( !m_project->inProject(fileUrl) );
}

QTEST_KDEMAIN(KTLProjectTest, GUI)
#include "ktlprojecttest.moc"
