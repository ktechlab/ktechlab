/***************************************************************************
*   Copyright (C) 2011 Zoltan Padrah <zoltan_padrah@users.sourceforge.net>*
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
***************************************************************************/

#include "ktlguiplugin.h"

#include <KGenericFactory>
#include <KAboutData>

#include <stdio.h>
#include <sublime/mainwindow.h>
#include <KActionMenu>
#include <KStandardAction>
#include <KActionCollection>

#include "newfiledlg.h"
#include <interfaces/icore.h>
#include <interfaces/iuicontroller.h>
#include <KFileDialog>
#include <shell/core.h>
#include <KIO/NetAccess>
#include <KMessageBox>
#include <kplugininfo.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/iplugincontroller.h>
#include <interfaces/idocumentcontroller.h>
#include <QTemporaryFile>


using namespace KTechLab;

K_PLUGIN_FACTORY(KTLGuiPluginFactory, registerPlugin<KTLGuiPlugin>(); )

K_EXPORT_PLUGIN(KTLGuiPluginFactory(
    KAboutData("ktlgui","ktlgui",
        ki18n("KTechLab GUI"),
        "0.1",
        ki18n("This plugin provides GUI to KTechLab"),
        KAboutData::License_LGPL)))

KTLGuiPlugin::KTLGuiPlugin(QObject* parent, const QVariantList& /* args */)
  : IPlugin( KTLGuiPluginFactory::componentData(), parent )
{
    printf("creating gui plugin \n");

    // create new file dialog
    m_mainWindow = core()->uiController()->activeMainWindow()->window();
    m_newFileDlg = new NewFileDlg(m_mainWindow);
    // hook up the new file dialog
    connect(m_newFileDlg,SIGNAL(signalFileNewAssembly()),
            this, SLOT(slotFileNewAssembly()));
    connect(m_newFileDlg,SIGNAL(signalFileNewC()),
            this, SLOT(slotFileNewC()));
    connect(m_newFileDlg,SIGNAL(signalFileNewCircuit()),
            this, SLOT(slotFileNewCircuit()));
    connect(m_newFileDlg,SIGNAL(signalFileNewFlowCode()),
            this, SLOT(slotFileNewFlowCode()));
    connect(m_newFileDlg,SIGNAL(signalFileNewMechanics()),
            this, SLOT(slotFileNewMechanics()));
    connect(m_newFileDlg,SIGNAL(signalFileNewMicrobe()),
            this, SLOT(slotFileNewMicrobe()));

    printf("gui plugin created\n");
}

KTLGuiPlugin::~KTLGuiPlugin()
{
    printf("gui plugin destroyed\n");
}

void KTLGuiPlugin::createActionsForMainWindow(Sublime::MainWindow* window,
                                              QString& xmlFile,
                                              KActionCollection& actions)
{
    xmlFile = "ktechlabui.rc" ;

    KStandardAction::open( this, SLOT(slotFileOpen()), window->actionCollection() );

    /*
    KStandardAction::openRecent( this, SLOT(slotFileOpenRecent()), actionCollection() );
    KStandardAction::save( this, SLOT(slotFileSave()), actionCollection() );
    KStandardAction::saveAs( this, SLOT(slotFileSaveAs()), actionCollection() );
    KStandardAction::close( this, SLOT(slotViewClose()), actionCollection() );
    KStandardAction::print( this, SLOT(slotFilePrint()), actionCollection() );
    KStandardAction::quit( this, SLOT(slotFileQuit()), actionCollection() );
    KStandardAction::undo( this, SLOT(slotUndo()), actionCollection() );
    KStandardAction::redo( this, SLOT(slotRedo()), actionCollection() );
    KStandardAction::cut( this, SLOT(slotCut()), actionCollection() );
    KStandardAction::copy( this, SLOT(slotCopy()), actionCollection() );
    KStandardAction::paste( this, SLOT(slotPaste()), actionCollection() );
    */

    KStandardAction::openNew(this, SLOT(slotFileNew()), window->actionCollection() );

    KActionMenu *newActionMenu = new KActionMenu( KIcon("document-new"), i18n("&New"), this );

    actionCollection()->addAction( KStandardAction::name(KStandardAction::New), newActionMenu );

    connect( newActionMenu, SIGNAL(activated()), this, SLOT(slotFileNew()) );

    KAction *action;
    action = actionCollection()->addAction( QString("newfile_asm") );
    action->setText( i18n("Assembly") );
    action->setIcon( KIcon("") );
    connect( action, SIGNAL(triggered()), this, SLOT(slotFileNewAssembly()) );
    newActionMenu->addAction( action );

    action = actionCollection()->addAction( QString("newfile_c") );
    action->setText( i18n("C source") );
    action->setIcon( KIcon("text-x-csrc") );
    connect( action, SIGNAL(triggered()), this, SLOT(slotFileNewC()) );
    newActionMenu->addAction( action );

    action = actionCollection()->addAction( QString("ktechlab_circuit") );
    action->setText( i18n("Circuit") );
    action->setIcon( KIcon("ktechlab_circuit") );
    connect( action, SIGNAL(triggered()), this, SLOT(slotFileNewCircuit()) );
    newActionMenu->addAction( action );

    action = actionCollection()->addAction( QString("ktechlab_flowcode") );
    action->setText( i18n("FlowCode") );
    action->setIcon( KIcon("ktechlab_flowcode") );
    connect( action, SIGNAL(triggered()), this, SLOT(slotFileNewFlowCode()) );
    newActionMenu->addAction( action );

    action = actionCollection()->addAction( QString("ktechlab_mechanics") );
    action->setText( i18n("Mechanics") );
    action->setIcon( KIcon("") );
    connect( action, SIGNAL(triggered()), this, SLOT(slotFileNewMechanics()) );
    newActionMenu->addAction( action );

    action = actionCollection()->addAction( QString("ktechlab_microbe") );
    action->setText( i18n("Microbe") );
    action->setIcon( KIcon("ktechlab_microbe") );
    connect( action, SIGNAL(triggered()), this, SLOT(slotFileNewMicrobe()) );
    newActionMenu->addAction( action );

    KDevelop::IPlugin::createActionsForMainWindow(window, xmlFile, actions);

    window->setupGUI();

    // FIXME calculate size dynamicaly, based on window contents
    window->resize(750, 470);
}


void KTLGuiPlugin::slotFileOpen()
{
    printf("slotFileOpen activated\n");

    KUrl file = KFileDialog::getOpenUrl( KUrl("kfiledialog:///<keyword>"), "*",
                                         m_mainWindow );

    if ( file.isEmpty() )
        return;

    if ( !file.isValid() )
        return;

    QString target;
    // the below code is what you should normally do.  in this
    // example case, we want the url to our own.  you probably
    // want to use this code instead for your app

    // download the contents
    if ( !KIO::NetAccess::download( file, target, m_mainWindow ) )
    {
        // If the file could not be downloaded, for example does not
        // exist on disk, NetAccess will tell us what error to use
        KMessageBox::error(m_mainWindow, KIO::NetAccess::lastErrorString());

        return;
    }

    // now url points always to local storage
    KUrl url(target);

    if ( url.url().endsWith( ".ktechlab", Qt::CaseInsensitive ) )
    {
        // FIXME: create an IProject (i.e. KTechlab::Project) to open the url,
        // since this will prevent KTechLab from crashing...

        // This is a ktechlab project; it has to be handled separetly from a
        // normal file.

        core()->projectController()->openProject( url );
        return;
    }

    //     addRecentFile( url );

    // set our caption
    // setCaption( url.url() );

    //get interface for extension
    QStringList constraints;
    constraints << QString("'%1' in [X-KDevelop-SupportedMimeTypes]").arg(
            "application/x-circuit" );

    // load in the file (target is always local)
    KDevelop::ICore *theCore = core();
    Q_ASSERT( theCore != 0 );
    KDevelop::IPluginController *pc = theCore->pluginController();
    Q_ASSERT( pc != 0 );
    QList<KPluginInfo> plugins = pc->queryExtensionPlugins( "KTLDocument",
                                                            constraints );

    foreach (KPluginInfo p, plugins) {
        qDebug() << p.name();
    }

    if(! url.url().endsWith( ".ktechlab", Qt::CaseInsensitive )){
        qWarning() << "File extension not known, trying as circuit anyway!\n";
    }

    // try to open
    theCore->documentController()->openDocument(url, "ktlcircuit");

    // and remove the temp file
    KIO::NetAccess::removeTempFile( target );
}

void KTLGuiPlugin::slotFileNew()
{
    printf("slotFileNew activated\n");
    m_newFileDlg->show();
}

void KTLGuiPlugin::slotFileNewAssembly()
{
    printf("slotFileNewAssembly activated\n");
    // get a temporary file name
    QTemporaryFile tmpFile(QDir::tempPath().append(QDir::separator())
        .append("ktlXXXXXX.asm"));
    tmpFile.setAutoRemove(false);
    tmpFile.open();
    qDebug() << "creating temporary file: " << tmpFile.fileName()
        << "pattern: " << tmpFile.fileTemplate();

    KUrl url(tmpFile.fileName());
    core()->documentController()->openDocument(url);
}

void KTLGuiPlugin::slotFileNewC()
{
    printf("slotFileNewC activated\n");
}

void KTLGuiPlugin::slotFileNewCircuit()
{
    printf("slotFileNewCircuit activated\n");
    // get a temporary file name
    QTemporaryFile tmpFile(QDir::tempPath().append(QDir::separator())
        .append("ktlXXXXXX.circuit"));
    tmpFile.setAutoRemove(false);
    tmpFile.open();
    qDebug() << "creating temporary file: " << tmpFile.fileName()
        << "pattern: " << tmpFile.fileTemplate();
    // write a minial circuit document into the temporary file
    tmpFile.write("<!DOCTYPE KTechlab>\n"
                  "<document type=\"circuit\" >"
                  "</document>"
                  );
    tmpFile.close();
    KUrl url(tmpFile.fileName());
    core()->documentController()->openDocument(url, "ktlcircuit");
}

void KTLGuiPlugin::slotFileNewFlowCode()
{
    printf("slotFileNewFlowCode activated\n");
}

void KTLGuiPlugin::slotFileNewMechanics()
{
    printf("slotFileNewMechanics activated\n");
}

void KTLGuiPlugin::slotFileNewMicrobe()
{
    printf("slotFileNewMicrobe activated\n");
}

#include "ktlguiplugin.moc"
