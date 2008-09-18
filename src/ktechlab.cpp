/******************************************************************************
 * Main Window Class                                                          *
 *                                                                            *
 * Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                        *
 *                                                                            *
 * This program is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU General Public License as             *
 * published by the Free Software Foundation; either version 2 of             *
 * the License, or (at your option) any later version.                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU General Public License for more details.                               *
 *                                                                            *
 * You should have received a copy of the GNU General Public License          *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.      *
 ******************************************************************************/

#include "ktechlab.h"

#include <QMenu>

#include <KApplication>
#include <KUrl>
#include <KTextEdit>
#include <KLocalizedString>
#include <KMenuBar>
#include <KAction>
#include <KStandardAction>
#include <KActionCollection>
#include <KActionMenu>

#include <KDebug>

KTechlab::KTechlab() : KXmlGuiWindow()
{
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();
    setupGUI( Default );
}

KTechlab::~KTechlab()
{
    return;
}

void KTechlab::load( KUrl url )
{
    return;
}

void KTechlab::slotFileNewAssembly()
{
    kDebug() << "triggered()";
}

void KTechlab::slotFileNewC()
{
    kDebug() << "triggered()";
}

void KTechlab::slotFileNewCircuit()
{
    kDebug() << "triggered()";
}

void KTechlab::slotFileNewFlowCode()
{
    kDebug() << "triggered()";
}

void KTechlab::slotFileNewMechanics()
{
    kDebug() << "triggered()";
}

void KTechlab::slotFileNewMicrobe()
{
    kDebug() << "triggered()";
}

void KTechlab::slotFileNew()
{
    kDebug() << "triggered()";
}

void KTechlab::slotFileOpen()
{
}

void KTechlab::slotFileOpenRecent()
{
}

void KTechlab::slotFileSave()
{
}

void KTechlab::slotFileSaveAs()
{
}

void KTechlab::slotFilePrint()
{
}

void KTechlab::slotFileQuit()
{
    KApplication::kApplication()->quit();
}

void KTechlab::slotViewClose()
{
}

void KTechlab::slotUndo()
{
}

void KTechlab::slotRedo()
{
}

void KTechlab::slotCut()
{
}

void KTechlab::slotCopy()
{
}

void KTechlab::slotPaste()
{
}

void KTechlab::setupActions()
{
}

void KTechlab::createMenus()
{
    fileMenu = menuBar()->addMenu( i18n("&File") );
}

void KTechlab::createActions()
{
    KStandardAction::openNew( this, SLOT(slotFileNew()), actionCollection() );
    KStandardAction::open( this, SLOT(slotFileOpen()), actionCollection() );
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

    action = actionCollection()->addAction( QString("rotate_clockwise") );
    action->setText( i18n("Rotate Clockwise") );
    action->setIcon( KIcon("object-rotate-right") );
    connect( action, SIGNAL(triggered()), this, SLOT(slotComponentRotateClockwise()) );

    action = actionCollection()->addAction( QString("rotate_counterclockwise") );
    action->setText( i18n("Rotate Counter-Clockwise") );
    action->setIcon( KIcon("object-rotate-left") );
    connect( action, SIGNAL(triggered()), this, SLOT(slotComponentRotateClockwise()) );
}

void KTechlab::createToolBars()
{
}

void KTechlab::createStatusBar()
{
}

void KTechlab::readSettings()
{
}
