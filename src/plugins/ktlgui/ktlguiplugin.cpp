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
    */

    KDevelop::IPlugin::createActionsForMainWindow(window, xmlFile, actions);

    window->setupGUI();

    // FIXME calculate size dynamicaly, based on window contents
    window->resize(750, 470);
}

void KTLGuiPlugin::slotFileOpen()
{
    printf("slotFileOpen activated\n");
}

void KTLGuiPlugin::slotFileNew()
{
    printf("slotFileNew activated\n");
    m_newFileDlg->show();
}

void KTLGuiPlugin::slotFileNewAssembly()
{
    printf("slotFileNewAssembly activated\n");
}

void KTLGuiPlugin::slotFileNewC()
{
    printf("slotFileNewC activated\n");
}

void KTLGuiPlugin::slotFileNewCircuit()
{
    printf("slotFileNewCircuit activated\n");
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
