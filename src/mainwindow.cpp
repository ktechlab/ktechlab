/******************************************************************************
 * Main Window Class                                                          *
 *                                                                            *
 * Copyright (C) 2005 by David Saxton <david@bluehaze.org>                    *
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

#include "mainwindow.h"

#include <QMenu>
#include <QLayout>
#include <QListView>
#include <QSplitter>
#include <QMimeData>

#include <KApplication>
#include <KMessageBox>
#include <KUrl>
#include <KFileDialog>
#include <KTextEdit>
#include <KLocalizedString>
#include <KMenuBar>
#include <KAction>
#include <KStandardAction>
#include <KActionCollection>
#include <KActionMenu>
#include <KDebug>
#include <KIO/NetAccess>

#include <sublime/area.h>
#include <sublime/controller.h>
#include <interfaces/iplugincontroller.h>
#include <sublime/tooldocument.h>
#include <shell/core.h>
#include <shell/projectcontroller.h>
#include <shell/documentcontroller.h>

using namespace KTechLab;

MainWindow::MainWindow( Sublime::Controller *controller, Qt::WindowFlags flags )
: KDevelop::MainWindow( controller, flags )
{
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    setupGUI( Default );
}

MainWindow::~MainWindow()
{
}

KTabWidget * MainWindow::tabWidget()
{
    return 0;
}

/*
 * TODO: put this logic into some kind of controller
 */
void MainWindow::load( const KUrl &url )
{
    if ( !url.isValid() )
        return;

    if ( url.url().endsWith( ".ktechlab", Qt::CaseInsensitive ) )
    {
        // FIXME: create an IProject (i.e. KTechlab::Project) to open the url,
        // since this will prevent KTechLab from crashing...

        // This is a ktechlab project; it has to be handled separetly from a
        // normal file.

        KDevelop::Core::self()->projectController()->openProject( url );
        return;
    }

    QString target;
    // the below code is what you should normally do.  in this
    // example case, we want the url to our own.  you probably
    // want to use this code instead for your app

    // download the contents
    if ( !KIO::NetAccess::download( url, target, this ) )
    {
        // If the file could not be downloaded, for example does not
        // exist on disk, NetAccess will tell us what error to use
        KMessageBox::error(this, KIO::NetAccess::lastErrorString());

        return;
    }

//     addRecentFile( url );

    // set our caption
    setCaption( url.url() );

    //get interface for extension
    QStringList constraints;
    constraints << QString("'%1' in [X-KDevelop-SupportedMimeTypes]").arg(
            "application/x-circuit" );

    // load in the file (target is always local)
    KDevelop::Core *core = KDevelop::Core::self();
    Q_ASSERT( core != 0 );
    KDevelop::IPluginController *pc = core->pluginController();
    Q_ASSERT( pc != 0 );
    QList<KPluginInfo> plugins =
            pc->queryExtensionPlugins( "KTLDocument", constraints );

    foreach (KPluginInfo p, plugins) {
        qDebug() << p.name();
    }
    //KDevelop::PartDocument * document = new CircuitDocument( url, KDevelop::Core::self() );

    // and remove the temp file
    KIO::NetAccess::removeTempFile( target );
}

void MainWindow::init()
{
    Sublime::Document *componentView = new Sublime::ToolDocument( "ComponentView", controller(),
            new Sublime::SimpleToolWidgetFactory<QListView>( "ComponentView" ) );

    m_mainArea = new Sublime::Area( controller(), "ktlcircuit", i18n("Circuit") );
    m_mainArea->addToolView( componentView->createView(), Sublime::Left );
    m_mainArea->setIconName( "ktechlab_circuit" );
    controller()->addDefaultArea( m_mainArea );

    controller()->showArea(m_mainArea,this);

#if KDEV_PLUGIN_VERSION < 16
	// TODO see why do we need this
    setupAreaSelector();
#endif
}

void MainWindow::slotFileNewAssembly()
{
    kDebug() << "triggered()";
}

void MainWindow::slotFileNewC()
{
    kDebug() << "triggered()";
}

void MainWindow::slotFileNewCircuit()
{
    kDebug() << "triggered()";
}

void MainWindow::slotFileNewFlowCode()
{
    kDebug() << "triggered()";
}

void MainWindow::slotFileNewMechanics()
{
    kDebug() << "triggered()";
}

void MainWindow::slotFileNewMicrobe()
{
    kDebug() << "triggered()";
}

void MainWindow::slotFileNew()
{
    kDebug() << "triggered()";
}

void MainWindow::slotFileOpen()
{
    KUrl file = KFileDialog::getOpenUrl( KUrl("kfiledialog:///<keyword>"), "*", this );

    if ( !file.isEmpty() )
        load( file );
}

void MainWindow::slotFileOpenRecent()
{
}

void MainWindow::slotFileSave()
{
}

void MainWindow::slotFileSaveAs()
{
}

void MainWindow::slotFilePrint()
{
}

void MainWindow::slotFileQuit()
{
    KApplication::kApplication()->quit();
}

void MainWindow::slotViewClose()
{
}

void MainWindow::slotUndo()
{
}

void MainWindow::slotRedo()
{
}

void MainWindow::slotCut()
{
}

void MainWindow::slotCopy()
{
}

void MainWindow::slotPaste()
{
}

void MainWindow::slotComponentRotateCCW()
{}

void MainWindow::slotComponentRotateCW()
{}

void MainWindow::setupActions()
{
}

void MainWindow::createMenus()
{
}

void MainWindow::createActions()
{
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
}

void MainWindow::createToolBars()
{
}

void MainWindow::createStatusBar()
{
}

void MainWindow::readSettings()
{
}

