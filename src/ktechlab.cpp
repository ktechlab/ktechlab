/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#define protected public
#include <kxmlguiclient.h>
#undef protected

#include "circuitdocument.h"
#include "componentmodellibrary.h"
#include "config.h"
#include "contexthelp.h"
#include "docmanager.h"
#include "filemetainfo.h"
#include "flowcodedocument.h"
#include "itemeditor.h"
#include "itemgroup.h"
#include "iteminterface.h"
#include "itemlibrary.h"
#include "ktechlab.h"
#include "core/ktlconfig.h"
#include "languagemanager.h"
#include "mechanicsdocument.h"
#include "microlibrary.h"
#include "newfiledlg.h"
#include "oscilloscope.h"
#include "projectmanager.h"
#include "recentfilesaction.h"
#include "scopescreen.h"
#include "settingsdlg.h"
#include "subcircuits.h"
#include "symbolviewer.h"
#include "textdocument.h"
#include "textview.h"
#include "viewcontainer.h"

#include <q3dockarea.h>
#include <qtimer.h>
#include <qtoolbutton.h>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QLabel>
#include <Q3PtrList>
#include <QPixmap>
#include <QDragEnterEvent>
#include <QDropEvent>

#include <kaccel.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kedittoolbar.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kio/netaccess.h>
#include <kkeydialog.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpopupmenu.h>
#include <kstandarddirs.h>
#include <ktabwidget.h> 
#include <kurldrag.h>
#include <kwin.h>

KTechlab *KTechlab::m_pSelf = 0;

KTechlab::KTechlab()
	: KateMDI::MainWindow( 0, "KTechlab" )
{
	m_pSelf = this;

	QTime ct;
	ct.start();

	m_bIsShown = false;
	m_pContainerDropSource = 0;
	m_pContainerDropReceived = 0;
	m_pContextMenuContainer = 0;
	m_pFocusedContainer = 0;
	m_pToolBarOverlayLabel = 0;

	m_pUpdateCaptionsTimer = new QTimer( this );
	connect( m_pUpdateCaptionsTimer, SIGNAL(timeout()), this, SLOT(slotUpdateCaptions()) );

	setMinimumSize( 400, 400 );

	setupTabWidget();
	setupToolDocks();
	setupActions();
	setupView();
	readProperties( KGlobal::config() );

//	kdDebug() << "Constructor time: " << ct.elapsed() << endl;
}

KTechlab::~KTechlab()
{
	fileMetaInfo()->saveAllMetaInfo();
	
	delete fileMetaInfo();
	delete itemLibrary(); // This better be the last time the item library is used!
	delete subcircuits();
}


void KTechlab::show()
{
	KateMDI::MainWindow::show();
	m_bIsShown = true;
}


void KTechlab::openFile( ViewArea * viewArea )
{
	KURL::List files = getFileURLs( false );
	if ( files.isEmpty() )
		return;
	
	load( files.first(), viewArea );
}


void KTechlab::load( const KURL & url, ViewArea * viewArea )
{
	if ( !url.isValid() )
		return;
	
	if ( url.url().endsWith( ".ktechlab", false ) )
	{
		// This is a ktechlab project; it has to be handled separetly from a
		// normal file.
		
		ProjectManager::self()->slotOpenProject( url );
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
	
	addRecentFile( url );
	
	// set our caption
	setCaption( url.prettyURL() );
	
	// load in the file (target is always local)
	DocManager::self()->openURL( target, viewArea );
	
	// and remove the temp file
	KIO::NetAccess::removeTempFile( target );
}


QStringList KTechlab::recentFiles()
{
	return m_recentFiles->items();
}


void KTechlab::setupToolDocks()
{
#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
	setToolViewStyle( KMultiTabBar::KDEV3ICON );
# endif
#endif
	
	QPixmap pm;
	KIconLoader * loader = KGlobal::iconLoader();
	KateMDI::ToolView * tv = 0l;
	
	tv = createToolView( ProjectManager::toolViewIdentifier(),
				 KMultiTabBar::Left,
				 loader->loadIcon( "attach", KIcon::Small ),
				 i18n("Project") );
	ProjectManager::self( tv );
	
	pm.load( locate( "appdata", "icons/circuit.png" ) );
	tv = createToolView( ComponentSelector::toolViewIdentifier(),
				 KMultiTabBar::Left,
				 pm,
				 i18n("Components") );
	ComponentSelector::self(tv);
	
	// Create an instance of the subcircuits interface, now that we have created the component selector
	subcircuits();
	Subcircuits::loadSubcircuits();
	
	pm.load( locate( "appdata", "icons/flowcode.png" ) );
	tv = createToolView( FlowPartSelector::toolViewIdentifier(),
				 KMultiTabBar::Left,
				 pm,
				 i18n("Flow Parts") );
	FlowPartSelector::self(tv);
	
#ifdef MECHANICS
	pm.load( locate( "appdata", "icons/mechanics.png" ) );
	tv = createToolView( MechanicsSelector::toolViewIdentifier(),
				 KMultiTabBar::Left,
				 pm,
				 i18n("Mechanics") );
	MechanicsSelector::self(tv);
#endif
	
	pm.load( locate( "appdata", "icons/item.png" ) );
	tv = createToolView( ItemEditor::toolViewIdentifier(),
				 KMultiTabBar::Right,
				 pm,
				 i18n("Item Editor") );
	ItemEditor::self(tv);
	
	tv = createToolView( ContextHelp::toolViewIdentifier(),
				KMultiTabBar::Right,
				loader->loadIcon( "contents", KIcon::Small ),
				i18n("Context Help") );
	ContextHelp::self(tv);
	
	tv = createToolView( LanguageManager::toolViewIdentifier(),
				 KMultiTabBar::Bottom,
				 loader->loadIcon( "log", KIcon::Small ),
				 i18n("Messages") );
	LanguageManager::self(tv);
	
#ifndef NO_GPSIM
	tv = createToolView( SymbolViewer::toolViewIdentifier(),
				 KMultiTabBar::Right,
				 loader->loadIcon( "blockdevice", KIcon::Small ),
				 i18n("Symbol Viewer") );
	SymbolViewer::self(tv);
#endif
	
	addOscilloscopeAsToolView(this);
#if 1
	//pm.load( locate( "appdata", "icons/oscilloscope.png" ) );
	tv = createToolView( ScopeScreen::toolViewIdentifier(),
	                     KMultiTabBar::Bottom,
	                     loader->loadIcon( "oscilloscope", KIcon::Small ),
	                     i18n("Scope Screen (Very Rough)") );
	ScopeScreen::self( tv );
#endif

	updateSidebarMinimumSizes();
}


void KTechlab::addWindow( ViewContainer * vc )
{
	if ( vc && !m_viewContainerList.contains(vc) )
	{
		m_viewContainerList << vc;
		connect( vc, SIGNAL(destroyed(QObject* )), this, SLOT(slotViewContainerDestroyed(QObject* )) );
	}
	
	m_viewContainerList.remove((ViewContainer*)0);
	slotUpdateTabWidget();
	slotDocModifiedChanged();
}


void KTechlab::setupView()
{
	setAcceptDrops(true);
	setStandardToolBarMenuEnabled(true);
	setXMLFile("ktechlabui.rc");
	createShellGUI(true);
	action("newfile_popup")->plug( toolBar("mainToolBar"), 0 );
	action("file_new")->unplug( toolBar("mainToolBar") );
	setupExampleActions();
	statusBar()->show();
}


void KTechlab::overlayToolBarScreenshot()
{
	return;
	
	if ( !m_pToolBarOverlayLabel )
	{
		m_pToolBarOverlayLabel = new QLabel( 0, 0, WStyle_StaysOnTop | WStyle_Customize | WStyle_NoBorder | WNoAutoErase | WType_Popup  );
		m_pToolBarOverlayLabel->hide();
		m_pToolBarOverlayLabel->setBackgroundMode( NoBackground );
	}
	
	if ( !m_bIsShown )
	{
		// The window isn't visible yet, so there's nothing to overlay (and if we tried,
		// it would appear as a strange floating toolbar).
		return;
	}
	
	if ( m_pToolBarOverlayLabel->isShown() )
	{
		// This is to avoid successive calls to removeGUIClient when we have
		// already popped it up for the first call, and don't want to take
		// another screenshot (as that would be without the toolbar).
		return;
	}
	
	Q3PtrListIterator<KToolBar> toolBarIterator();
	
// 	QWidget * toolsWidget = toolBar( "toolsToolBar" );
// 	QWidget * debugWidget = toolBar( "debugTB" );
	
	KToolBar * toolsWidget = static_cast<KToolBar*>(child( "toolsToolBar", "KToolBar" ));
	KToolBar * debugWidget = static_cast<KToolBar*>(child( "debugTB", "KToolBar" ));
	
	if ( !toolsWidget && !debugWidget )
		return;
	
	QWidget * parent = static_cast<QWidget*>(toolsWidget ? toolsWidget->parent() : debugWidget->parent());
	
	QRect grabRect;
	
	// 128 is a sanity check (widget can do strange things when being destroyed)
	
	if ( toolsWidget && toolsWidget->height() <= 128 )
		grabRect |= toolsWidget->geometry();
	if ( debugWidget && debugWidget->height() <= 128 )
		grabRect |= debugWidget->geometry();
	
	if ( !grabRect.isValid() )
		return;
	
	QPixmap shot = QPixmap::grabWidget( parent, grabRect.x(), grabRect.y(), grabRect.width(), grabRect.height() );
	
	m_pToolBarOverlayLabel->move( parent->mapToGlobal( grabRect.topLeft() ) );
	m_pToolBarOverlayLabel->setFixedSize( grabRect.size() );
	m_pToolBarOverlayLabel->setPixmap( shot );
	m_pToolBarOverlayLabel->show();
	
	QTimer::singleShot( 100, this, SLOT( hideToolBarOverlay() ) );
}


void KTechlab::hideToolBarOverlay()
{
	if ( !m_pToolBarOverlayLabel )
		return;
	
// 	QWidget * hiddenWidget = toolBar( "toolsToolBar" );
// 	if ( !hiddenWidget )
// 		return;
	
// 	hiddenWidget->setBackgroundMode( NoBackground );
// 	hiddenWidget->setWFlags( WNoAutoErase ); 
// 	hiddenWidget->setUpdatesEnabled( false );
	
	m_pToolBarOverlayLabel->hide();
}


void KTechlab::addNoRemoveGUIClient( KXMLGUIClient * client )
{
	if ( client && !m_noRemoveGUIClients.contains( client ) )
		m_noRemoveGUIClients << client;
}


void KTechlab::removeGUIClients()
{
	Q3ValueList<KXMLGUIClient*> clientsToRemove;
	
	Q3PtrList<KXMLGUIClient> clients = factory()->clients();
	for ( KXMLGUIClient * client = clients.first(); client; client = clients.next() )
	{
		if ( client && client != this && !m_noRemoveGUIClients.contains( client ) )
			clientsToRemove << client;
	}
	
	if ( clients.isEmpty() )
		return;
	
	overlayToolBarScreenshot();
	
	Q3ValueList<KXMLGUIClient*>::iterator end = clientsToRemove.end();
	for ( Q3ValueList<KXMLGUIClient*>::iterator it = clientsToRemove.begin(); it != end; ++it )
		factory()->removeClient(*it);
}


void KTechlab::setupTabWidget()
{	
	m_pViewContainerTabWidget = new KTabWidget(centralWidget());
	connect( tabWidget(), SIGNAL(currentChanged(QWidget* )), this, SLOT(slotViewContainerActivated(QWidget* )) );
	
	KConfig *config = kapp->config();
	config->setGroup("UI");
	
	bool CloseOnHover = config->readBoolEntry( "CloseOnHover", false );
	tabWidget()->setHoverCloseButton( CloseOnHover );
	
	bool CloseOnHoverDelay = config->readBoolEntry( "CloseOnHoverDelay", false );
	tabWidget()->setHoverCloseButtonDelayed( CloseOnHoverDelay );
	
	if (config->readBoolEntry( "ShowCloseTabsButton", true ))
	{
		QToolButton *but = new QToolButton(tabWidget());
		but->setIconSet(SmallIcon("tab_remove"));
		but->adjustSize();
		but->hide();
		connect( but, SIGNAL(clicked()), this, SLOT(slotViewContainerClose()) );
		tabWidget()->setCornerWidget(but, TopRight);
	}
	
	connect(tabWidget(), SIGNAL(contextMenu(QWidget*,const QPoint &)), this, SLOT(slotTabContext(QWidget*,const QPoint &)));
}


void KTechlab::slotUpdateTabWidget()
{
	m_viewContainerList.remove( (ViewContainer*)0 );
	
	bool noWindows = m_viewContainerList.isEmpty();
	
	if ( QWidget * button = tabWidget()->cornerWidget(TopRight) )
		button->setHidden( noWindows );
	
	if ( noWindows )
		setCaption( 0 );
}


void KTechlab::setupActions()
{
	KActionCollection *ac = actionCollection();
	
    KStdAction::openNew(			this, SLOT(slotFileNew()),					ac );
    KStdAction::open(				this, SLOT(slotFileOpen()),					ac );
    KStdAction::save(				this, SLOT(slotFileSave()),					ac );
    KStdAction::saveAs(				this, SLOT(slotFileSaveAs()),				ac );
	KStdAction::close(				this, SLOT(slotViewClose()),				ac );
    KStdAction::print(				this, SLOT(slotFilePrint()),				ac );
    KStdAction::quit(				this, SLOT(slotFileQuit()),					ac );
	KStdAction::undo(				this, SLOT(slotEditUndo()),					ac );
	KStdAction::redo(				this, SLOT(slotEditRedo()),					ac );
	KStdAction::cut(				this, SLOT(slotEditCut()),					ac );
	KStdAction::copy(				this, SLOT(slotEditCopy()),					ac );
	KStdAction::paste(				this, SLOT(slotEditPaste()),				ac );
	KStdAction::keyBindings(		this, SLOT(slotOptionsConfigureKeys()),		ac );
	KStdAction::configureToolbars(	this, SLOT(slotOptionsConfigureToolbars()),	ac );
	KStdAction::preferences(		this, SLOT(slotOptionsPreferences()),		ac );
	
	//BEGIN New file popup
	KToolBarPopupAction *p = new KToolBarPopupAction( i18n("&New"), "filenew", KStdAccel::shortcut(KStdAccel::New), this, SLOT(slotFileNew()), ac, "newfile_popup" );
	p->popupMenu()->insertTitle( i18n("New File") );
	(new KAction( i18n("Assembly"), "source", 0, this, SLOT(slotFileNewAssembly()), ac, "newfile_asm" ))->plug( p->popupMenu() );
	(new KAction( i18n("C source"), "source_c", 0, this, SLOT(slotFileNewC()), ac, "newfile_c" ))->plug( p->popupMenu() );
	(new KAction( i18n("Circuit"), "ktechlab_circuit", 0, this, SLOT(slotFileNewCircuit()), ac, "newfile_circuit" ))->plug( p->popupMenu() );
	(new KAction( i18n("FlowCode"), "ktechlab_flowcode", 0, this, SLOT(slotFileNewFlowCode()), ac, "newfile_flowcode" ))->plug( p->popupMenu() );
#ifdef MECHANICS
	(new KAction( i18n("Mechanics"), "ktechlab_mechanics", 0, this, SLOT(slotFileNewMechanics()), ac, "newfile_mechanics" ))->plug( p->popupMenu() );
#endif
	(new KAction( "Microbe", "ktechlab_microbe", 0, this, SLOT(slotFileNewMicrobe()), ac, "newfile_microbe" ))->plug( p->popupMenu() );
	//END New File popup
	

// 	m_recentFiles = KStdAction::openRecent( this, SLOT(load(const KURL&)), ac );
	m_recentFiles = new RecentFilesAction( "Recent Files", i18n("Open Recent"), this, SLOT(load(const KURL &)), ac, "file_open_recent" );
    m_statusbarAction = KStdAction::showStatusbar( this, SLOT(slotOptionsShowStatusbar()), ac );
	
	//BEGIN Project Actions
	ProjectManager *pm = ProjectManager::self();
	new KAction( i18n("New Project.."), "window_new",			0, pm, SLOT(slotNewProject()),			ac, 	"project_new" );
	new KAction( i18n("Open Project..."), "project_open",		0, pm, SLOT(slotOpenProject()),			ac, 	"project_open" );
// 	m_recentProjects = new KRecentFilesAction( i18n("Open &Recent Project..."), 0, ProjectManager::self(), SLOT(slotOpenProject(const KURL&)), ac, "project_open_recent" );
	m_recentProjects = new RecentFilesAction( "Recent Projects", i18n("Open &Recent Project..."), ProjectManager::self(), SLOT(slotOpenProject(const KURL&)), ac, "project_open_recent" );
	new KAction( i18n("Export to Makefile..."), "fileexport",	0, pm, SLOT(slotExportToMakefile()),		ac, "project_export_makefile" );
	new KAction( i18n("Create Subproject..."), 0,				0, pm, SLOT(slotCreateSubproject()),		ac, "project_create_subproject" );
	new KAction( i18n("Add Existing File..."), "fileopen",		0, pm, SLOT(slotAddFile()),					ac, "project_add_existing_file" );
	new KAction( i18n("Add Current File..."), "fileimport",		0, pm, SLOT(slotAddCurrentFile()),			ac, "project_add_current_file" );
// 	new KAction( i18n("Project Options"), "configure",			0, pm, SLOT(slotProjectOptions()),			ac, "project_options" );
	new KAction( i18n("Close Project"), "fileclose",			0, pm, SLOT(slotCloseProject()),			ac, "project_close" );
	new KAction( i18n("Remove from Project"), "editdelete",		0, pm, SLOT(slotRemoveSelected()),			ac, "project_remove_selected" );
	new KAction( i18n("Insert Existing File..."), "fileopen",	0, pm, SLOT(slotSubprojectAddExistingFile()),	ac, "subproject_add_existing_file" );
	new KAction( i18n("Insert Current File..."), "fileimport",	0, pm, SLOT(slotSubprojectAddCurrentFile()),ac, "subproject_add_current_file" );
	new KAction( i18n("Linker Options..."), "configure",		0, pm, SLOT(slotSubprojectLinkerOptions()),	ac, "project_item_linker_options" );
	new KAction( i18n("Build..."), "launch",					0, pm, SLOT(slotItemBuild()),				ac, "project_item_build" );
	new KAction( i18n("Upload..."), "convert_to_pic",			0, pm, SLOT(slotItemUpload()),				ac, "project_item_upload" );
	new KAction( i18n("Processing Options..."), "configure",	0, pm, SLOT(slotItemProcessingOptions()),	ac, "project_item_processing_options" );
	//END Project Actions
	
	new KAction( i18n("Split View Left/Right"), "view_right", Qt::CTRL|Qt::SHIFT|Qt::Key_L, this, SLOT(slotViewSplitLeftRight()), ac, "view_split_leftright" );
	new KAction( i18n("Split View Top/Bottom"), "view_bottom", Qt::CTRL|Qt::SHIFT|Qt::Key_T, this, SLOT(slotViewSplitTopBottom()), ac, "view_split_topbottom" );
	
	KToggleAction * ta = new KToggleAction( i18n("Run Simulation"), "player_play", Qt::Key_F10, 0, 0, ac, "simulation_run" );
	ta->setChecked(true);
	connect( ta, SIGNAL(toggled(bool )), Simulator::self(), SLOT(slotSetSimulating(bool )) );
#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION >= KDE_MAKE_VERSION(3,3,0)
		ta->setCheckedState( KGuiItem( i18n("Pause Simulation"), "player_pause", 0 ) );
# endif
#endif
	
	// We can call slotCloseProject now that the actions have been created
	ProjectManager::self()->updateActions();
	DocManager::self()->disableContextActions();
}


void KTechlab::setupExampleActions()
{
	QStringList categories;
	categories << "555" << "basic" << "diodes" << "jfets" << "mosfets" << "transistors" << "opamps";
	
	// A name for the example circuit can be generated from the filename (and
	// will be done so if the filename is not in this list). But the name
	// generation is not that intelligent (e.g. and.circuit should be called
	// "AND", not "And" - poor capitalization. And it also allows translation of
	// names.
	QStringMap filesToNames;
	filesToNames["internals.circuit"] = i18n("Internals");
	filesToNames["square-wave.circuit"] = i18n("Square Wave");
	filesToNames["2-way-switch.circuit"] = i18n("%1-Way Switch").arg(2);
	filesToNames["3-way-switch.circuit"] = i18n("%1-Way Switch").arg(3);
	filesToNames["capacitor.circuit"] = i18n("Capacitor");
	filesToNames["lrc.circuit"] = i18n("LRC");
	filesToNames["resistors-parallel.circuit"] = i18n("Resistors in Parallel");
	filesToNames["resistors-series.circuit"] = i18n("Resistors in Series");
	filesToNames["voltage-divider.circuit"] = i18n("Voltage Divider");
	filesToNames["full-wave-rectifier.circuit"] = i18n("Full Wave Rectifier");
	filesToNames["half-wave-rectifier.circuit"] = i18n("Half Wave Rectifier");
	filesToNames["inverter.circuit"] = i18n("Inverter");
	filesToNames["and.circuit"] = i18n("AND");
	filesToNames["nand.circuit"] = i18n("NAND");
	filesToNames["nor.circuit"] = i18n("NOR");
	filesToNames["or.circuit"] = i18n("OR");
	filesToNames["decoupler.circuit"] = i18n("Decoupler");
	filesToNames["inverting-amplifier.circuit"] = i18n("Inverting Amplifier");
	filesToNames["noninverting-amplifier.circuit"] = i18n("Noninverting Amplifier");
	filesToNames["instrumentation-amplifier.circuit"] = i18n("Instrumentation Amplifier");
	filesToNames["astable-multivibrator.circuit"] = i18n("Astable Multivibrator");
	filesToNames["inverter.circuit"] = i18n("Inverter");
	filesToNames["scmitt-trigger.circuit"] = i18n("Scmitt Trigger");
	filesToNames["switch.circuit"] = i18n("Switch");
	
	int at = 0;
	
	QStringList::iterator end = categories.end();
	for ( QStringList::iterator it = categories.begin(); it != end; ++it )
	{
		QString category = *it;
		QDir dir( locate( "appdata", "examples/" + category + "/" ) );
		
		KPopupMenu * m = static_cast<KPopupMenu*>(factory()->container( "examples_" + category, this ));
		if ( !m )
			continue;
		connect( m, SIGNAL(activated( int )), this, SLOT(openExample( int )) );
		
		QStringList files = dir.entryList();
		files.remove(".");
		files.remove("..");
		
		QStringList::iterator filesEnd = files.end();
		for ( QStringList::iterator filesIt = files.begin(); filesIt != filesEnd; ++filesIt )
		{
			QString fileName = *filesIt;
			
			QString name = filesToNames[ fileName ];
			if ( name.isEmpty() )
			{		
				name = fileName;
				name.remove(".circuit");
				name.replace("-"," ");
				name.replace("_"," ");
				
				// Capitalize the start of each word
				bool prevWasSpace = true;
				for ( unsigned i = 0; i < name.length(); ++i )
				{
					if ( prevWasSpace )
						name[i] = name[i].upper();
					prevWasSpace = name[i].isSpace();
				}
			}
			
			m->insertItem( name, at );
			m_exampleFiles[ at ] = dir.path() + "/" + fileName;
			at++;
		}
	}
}


void KTechlab::openExample( int id )
{
	DocManager::self()->openURL( m_exampleFiles[ id ] );
}


void KTechlab::slotViewContainerActivated( QWidget * viewContainer )
{
	m_pFocusedContainer = dynamic_cast<ViewContainer*>(viewContainer);
	m_pFocusedContainer->setFocus();
}


void KTechlab::slotViewContainerDestroyed( QObject * object )
{
	m_viewContainerList.remove( static_cast<ViewContainer*>(object) );
	m_viewContainerList.remove( (ViewContainer*)0 );
	slotUpdateTabWidget();
}


KAction * KTechlab::action( const QString & name ) const
{
	KAction * action = actionCollection()->action(name);
	if ( !action )
		kdError() << k_funcinfo << "No such action: " << name << endl;
	return action;
}


void KTechlab::saveProperties( KConfig *conf )
{
	// Dumbass KMainWindow - can't handle my width/height correctly. Whoever thought of the "+1" hack anyway?!
	conf->setGroup("UI");
	conf->writeEntry( "Width", width() );
	conf->writeEntry( "Height", height() );
	conf->writeEntry( "WinState", KWin::windowInfo( winId(), NET::WMState ).state() );
	
#ifndef NO_GPSIM
	SymbolViewer::self()->saveProperties( conf );
#endif
	
	if ( ProjectManager::self()->currentProject() )
	{
		conf->setGroup("Project");
		conf->writePathEntry( "Open", ProjectManager::self()->currentProject()->url().prettyURL() );
	}
	else
		conf->deleteGroup("Project");
	
	//BEGIN Open Views State
	// Remvoe old entries describing the save state - we don't want a horrible mish-mash of saved states
	const QStringList groupList = conf->groupList();
	const QStringList::const_iterator groupListEnd = groupList.end();
	for ( QStringList::const_iterator it = groupList.begin(); it != groupListEnd; ++it )
	{
		if ( (*it).startsWith("ViewContainer") )
			conf->deleteGroup(*it);
	}
	
	uint viewContainerId = 1;
	const ViewContainerList::iterator vcEnd = m_viewContainerList.end();
	for ( ViewContainerList::iterator it = m_viewContainerList.begin(); it != vcEnd; ++it )
	{
		if ( !(*it) || !(*it)->canSaveUsefulStateInfo() )
			continue;
		
		// To make sure the ViewContainers are restored in the right order, we must create them in alphabetical order,
		// as KConfig stores them as such...
		const QString id = QString::number(viewContainerId++).rightJustify( 4, '0' );
			
		conf->setGroup( "ViewContainer " + id );
		(*it)->saveState(conf);
	}
	//END Open Views State
	
	saveSession( conf, "KateMDI" );
	// Piss off KMainWindow
	conf->setGroup("KateMDI");
	int scnum = QApplication::desktop()->screenNumber(parentWidget());
	QRect desk = QApplication::desktop()->screenGeometry(scnum);
	conf->deleteEntry( QString::fromLatin1("Width %1").arg(desk.width()) );
	conf->deleteEntry( QString::fromLatin1("Height %1").arg(desk.height()) );
	
	conf->sync();
}


void KTechlab::readProperties( KConfig *conf )
{
	startRestore( conf, "KateMDI" );
	
	m_recentFiles->loadEntries();
	m_recentProjects->loadEntries();
	
	//BEGIN Restore Open Views
	if ( KTLConfig::restoreDocumentsOnStartup() )
	{
		// If we have a lot of views open from last time, then opening them will take a long time.
		// So we want to enter the qt event loop to finish drawing the window et al before adding the views.
		qApp->processEvents();
	
		const QStringList groupList = conf->groupList();
		const QStringList::const_iterator groupListEnd = groupList.end();
		for ( QStringList::const_iterator it = groupList.begin(); it != groupListEnd; ++it )
		{
			if ( (*it).startsWith("ViewContainer") )
			{
				ViewContainer *viewContainer = new ViewContainer( *it );
			
				conf->setGroup(*it);
				viewContainer->restoreState( conf, *it );
			
				addWindow( viewContainer );
			}
		}
	}
	//END Restore Open Views
	
	conf->setGroup("Project");
	if ( conf->readPathEntry("Open") != QString::null )
		ProjectManager::self()->slotOpenProject( KURL( conf->readPathEntry("Open") ) );
	
#ifndef NO_GPSIM
	SymbolViewer::self()->readProperties( conf );
#endif
	
	finishRestore();
	
	// Dumbass KMainWindow - can't handle my width/height correctly. Whoever thought of the "+1" hack anyway?!
	conf->setGroup("UI");
	resize( conf->readNumEntry( "Width", 800 ), conf->readNumEntry( "Height", 500 ) );
	KWin::setState( winId(), conf->readLongNumEntry( "WinState", NET::Max ) );
}


void KTechlab::dragEnterEvent(QDragEnterEvent *event)
{
    // accept uri drops only
    event->accept(KURLDrag::canDecode(event));
}


void KTechlab::dropEvent(QDropEvent *event)
{
    // this is a very simplistic implementation of a drop event.  we
    // will only accept a dropped URL.  the Qt dnd code can do *much*
    // much more, so please read the docs there
    KURL::List urls;

    // see if we can decode a URI.. if not, just ignore it
    if (KURLDrag::decode(event, urls) && !urls.isEmpty())
    {
        // okay, we have a URI.. process it
        const KURL &url = urls.first();

        // load in the file
        load(url);
    }
}


void KTechlab::slotOptionsShowStatusbar()
{
    // this is all very cut and paste code for showing/hiding the
    // statusbar
    if (m_statusbarAction->isChecked())
        statusBar()->show();
    else
        statusBar()->hide();
}


void KTechlab::slotOptionsConfigureKeys()
{
//     KKeyDialog::configureKeys(actionCollection(), "ktechlabui.rc");
	KKeyDialog::configure( actionCollection(), this, true );
}


void KTechlab::slotOptionsConfigureToolbars()
{
	KEditToolbar *dlg = new KEditToolbar(guiFactory());

	if (dlg->exec())
	{
		createShellGUI( false );
		createShellGUI( true );
	}

	delete dlg;
}


void KTechlab::slotOptionsPreferences()
{
	// An instance of your dialog could be already created and could be cached,
	// in which case you want to display the cached dialog instead of creating
	// another one
	if ( KConfigDialog::showDialog( "settings" ) )
		return;
	
	// KConfigDialog didn't find an instance of this dialog, so lets create it:
	SettingsDlg* dialog = new SettingsDlg( this, "settings", KTLConfig::self() );
	
	// User edited the configuration - update your local copies of the
	// configuration data
	connect( dialog, SIGNAL(settingsChanged()), this, SLOT(slotUpdateConfiguration()) );
	dialog->show();
}


void KTechlab::slotUpdateConfiguration()
{
	emit configurationChanged();
}


void KTechlab::slotChangeStatusbar( const QString & text )
{
	// Avoid flicker by repeatedly displaying the same message, as QStatusBar does not check for this
	if ( m_lastStatusBarMessage == text )
		return;
	
    statusBar()->message(text);
	m_lastStatusBarMessage = text;
}


void KTechlab::slotTabContext( QWidget* widget,const QPoint & pos )
{
	// Shamelessly stolen from KDevelop...
	
	KPopupMenu * tabMenu = new KPopupMenu;
	tabMenu->insertTitle( (dynamic_cast<ViewContainer*>(widget))->caption() );

	//Find the document on whose tab the user clicked
	m_pContextMenuContainer = 0l;
	
	m_viewContainerList.remove((ViewContainer*)0l);
	
	const ViewContainerList::iterator vcEnd = m_viewContainerList.end();
	for ( ViewContainerList::iterator it = m_viewContainerList.begin(); it != vcEnd; ++it )
	{
		ViewContainer * viewContainer = *it;
		if ( viewContainer == widget )
		{
			m_pContextMenuContainer = viewContainer;
			
			tabMenu->insertItem( i18n("Close"), 0 );
			
			View *view = (viewContainer->viewCount() == 1) ? viewContainer->activeView() : 0l;
			
			if ( view && view->document()->isModified() )
				tabMenu->insertItem( i18n("Save"), 1 );
			
			if ( view && !view->document()->url().isEmpty() )
				tabMenu->insertItem( i18n("Reload"), 2 );
			
			if ( m_viewContainerList.count() > 1 )
				tabMenu->insertItem( i18n("Close All Others"), 4 );

		}
	}
	
	connect( tabMenu, SIGNAL( activated(int) ), this, SLOT(slotTabContextActivated(int)) );

	tabMenu->exec(pos);
	delete tabMenu;
}


void KTechlab::slotTabContextActivated( int id )
{
	// Shamelessly stolen from KDevelop...
	
	if( !m_pContextMenuContainer )
		return;
	
	View *view = m_pContextMenuContainer->activeView();
	if (!view)
		return;
	QPointer<Document> document = view->document();

	switch(id)
	{
		case 0:
		{
			m_pContextMenuContainer->closeViewContainer();
			break;
		}
		case 1:
			document->fileSave();
			break;
		case 2:
		{
			KURL url = document->url();
			if ( document->fileClose() )
			{
				delete document;
				DocManager::self()->openURL(url);
			}
			break;
		}
		case 4:
		{
			const ViewContainerList::iterator vcEnd = m_viewContainerList.end();
			for ( ViewContainerList::iterator it = m_viewContainerList.begin(); it != vcEnd; ++it )
			{
				ViewContainer *viewContainer = *it;
				if ( viewContainer && viewContainer != m_pContextMenuContainer )
				{
					if ( !viewContainer->closeViewContainer() )
						return;
				}
			}
			break;
		}
		default:
			break;
	}
}
	


void KTechlab::slotFileNewAssembly()
{
	TextDocument *document = DocManager::self()->createTextDocument();
	if (document)
		document->slotInitLanguage( TextDocument::ct_asm );
}
void KTechlab::slotFileNewMicrobe()
{
	TextDocument *document = DocManager::self()->createTextDocument();
	if (document)
		document->slotInitLanguage( TextDocument::ct_microbe );
}
void KTechlab::slotFileNewC()
{
	TextDocument *document = DocManager::self()->createTextDocument();
	if (document)
		document->slotInitLanguage( TextDocument::ct_c );
}
void KTechlab::slotFileNewCircuit()
{
	DocManager::self()->createCircuitDocument();
}
void KTechlab::slotFileNewFlowCode()
{
	slotFileNew();
}
void KTechlab::slotFileNewMechanics()
{
	DocManager::self()->createMechanicsDocument();
}

void KTechlab::slotFileNew()
{
	NewFileDlg *newFileDlg = new NewFileDlg(this);
	
	newFileDlg->exec();
	
	bool addToProject = newFileDlg->addToProject();
	bool accepted = newFileDlg->accepted();
	int finalType = newFileDlg->fileType();
	QString microID = newFileDlg->microID();
	int codeType = newFileDlg->codeType();
	
	delete newFileDlg;
	if (!accepted)
		return;
	
	Document *created = 0l;
	
	if ( finalType == Document::dt_circuit )
		created = DocManager::self()->createCircuitDocument();
	
	else if ( finalType == Document::dt_flowcode )
	{
		FlowCodeDocument * fcd = DocManager::self()->createFlowCodeDocument();
		fcd->setPicType(microID);
		created = fcd;
	}
	
	else if ( finalType == Document::dt_mechanics )
		created = DocManager::self()->createMechanicsDocument();
	
	else
	{
		// Presumably a text document
		TextDocument * textDocument = DocManager::self()->createTextDocument();
	
		if (textDocument)
			textDocument->slotInitLanguage( (TextDocument::CodeType)codeType );
		
		created = textDocument;
	}
	
	if ( created && addToProject )
		created->setAddToProjectOnSave(true);
}

void KTechlab::slotFileOpen()
{
    // this slot is called whenever the File->Open menu is selected,
    // the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
    // button is clicked
	
    // standard filedialog
	KURL::List urls = getFileURLs();
	const KURL::List::iterator end = urls.end();
	for ( KURL::List::iterator it = urls.begin(); it != end; ++ it)
		load(*it);
}

void KTechlab::addRecentFile( const KURL &url )
{
	m_recentFiles->addURL( url );
	emit recentFileAdded(url);
}


KURL::List KTechlab::getFileURLs( bool allowMultiple )
{
	QString filter;
	filter = QString("*|%1\n"
			"*.asm *.src *.inc|%2 (*.asm, *.src, *.inc)\n"
			"*.hex|%3 (*.hex)\n"
			"*.circuit|%4 (*.circuit)\n"
			"*.flowcode|FlowCode (*.flowcode)\n"
			"*.basic *.microbe|Microbe (*.microbe, *.basic)"
					/*"*.mechanics|Mechanics (*.mechanics)\n"*/)
					.arg(i18n("All Files"))
					.arg(i18n("Assembly Code"))
					.arg(i18n("Intel Hex"))
					.arg(i18n("Circuit"));
	
	if ( allowMultiple )
		return KFileDialog::getOpenURLs( QString::null, filter, 0l, i18n("Open Location") );
	
	else
		return KFileDialog::getOpenURL( QString::null, filter, 0l, i18n("Open Location") );
}


void KTechlab::slotDocModifiedChanged()
{
	//BEGIN Set tab icons
	KIconLoader *loader = KGlobal::iconLoader();
	const ViewContainerList::iterator vcEnd = m_viewContainerList.end();
	for ( ViewContainerList::iterator it = m_viewContainerList.begin(); it != vcEnd; ++it )
	{
		ViewContainer * vc = *it;
		if ( !vc || !vc->activeView() || !vc->activeView()->document() )
			continue;
		
		QString iconName;
		
		if ( vc->activeView()->document()->isModified() )
			iconName = "filesave";
		
		else switch ( vc->activeView()->document()->type() )
		{
			case Document::dt_circuit:
				iconName = "ktechlab_circuit";
				break;
				
			case Document::dt_flowcode:
				iconName = "ktechlab_flowcode";
				break;
				
			case Document::dt_mechanics:
				iconName = "ktechlab_mechanics";
				break;
				
			case Document::dt_text:
				iconName = "txt";
				break;
				
			case Document::dt_pinMapEditor:
				break;
				
			case Document::dt_none:
				iconName = "unknown";
				break;
		}
		
		tabWidget()->setTabIconSet( vc, loader->loadIcon( iconName, KIcon::Small ) );
	}
	//END Set tab icons
}


void KTechlab::requestUpdateCaptions()
{
	m_pUpdateCaptionsTimer->start( 0, true );
}


void KTechlab::slotUpdateCaptions()
{
	//BEGIN Set KTechlab caption
	Document *document = DocManager::self()->getFocusedDocument();
	QString newCaption;
	if ( document )
	{
		KURL url = document->url();
		if ( url.isEmpty() )
			newCaption = document->caption();
		else
		{
			if ( url.isLocalFile() && url.ref().isNull() && url.query().isNull() )
				newCaption = url.path();
			else
				newCaption = url.prettyURL();
		}
	}
	else
		newCaption = "";
	
	if (newCaption != caption().remove(" - KTechlab"))
		setCaption(newCaption);
	//END Set KTechlab caption
	
	
	//BEGIN Set tab captions
	emit needUpdateCaptions();
	
	if ( document && document->activeView() && document->activeView()->viewContainer() )
	{
		document->activeView()->viewContainer()->updateCaption();
	}
	//END Set tab captions
}


void KTechlab::slotDocUndoRedoChanged()
{
	Document *document = DocManager::self()->getFocusedDocument();
	if (!document)
		return;
	
	action("edit_undo")->setEnabled( document->isUndoAvailable() );
	action("edit_redo")->setEnabled( document->isRedoAvailable() );
}

void KTechlab::slotFileSave()
{
	Document *document = DocManager::self()->getFocusedDocument();
	if (document)
		document->fileSave();
}

void KTechlab::slotFileSaveAs()
{
	Document *document = DocManager::self()->getFocusedDocument();
	if (document)
		document->fileSaveAs();
}

void KTechlab::slotFilePrint()
{
	Document *document = DocManager::self()->getFocusedDocument();
	if (document)
		document->print();
}


bool KTechlab::queryClose()
{
	saveProperties( KGlobal::config() );
	
	if ( DocManager::self()->closeAll() && ProjectManager::self()->slotCloseProject() )
	{
		// Make ourself "deleted"
		m_pSelf = 0l;
		return true;
	}
	
	return false;
}

void KTechlab::slotFileQuit()
{
	// close the first window, the list makes the next one the first again.
	// This ensures that queryClose() is called on each window to ask for closing
	KMainWindow* w;
	if(memberList)
	{
		for( w=memberList->first(); w!=0; w=memberList->next() )
		{
			// only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
			// the window and the application stay open.
			if( !w->close() ) break;
		}
	}
	
    slotChangeStatusbar( i18n("Exiting...") );
}

void KTechlab::slotEditUndo()
{
	Document *document = DocManager::self()->getFocusedDocument();
	if (document)
		document->undo();
}

void KTechlab::slotEditRedo()
{
	Document *document = DocManager::self()->getFocusedDocument();
	if (document)
		document->redo();
}

void KTechlab::slotEditCut()
{
	Document *document = DocManager::self()->getFocusedDocument();
	if (document)
		document->cut();
}

void KTechlab::slotEditCopy()
{
	Document *document = DocManager::self()->getFocusedDocument();
	if (document)
		document->copy();
}

void KTechlab::slotEditPaste()
{
	Document *document = DocManager::self()->getFocusedDocument();
	if (document)
		document->paste();
}

void KTechlab::slotViewContainerClose()
{
	if (m_pFocusedContainer)
		m_pFocusedContainer->closeViewContainer();
}
void KTechlab::slotViewClose()
{
	View *view = DocManager::self()->getFocusedView();
	if (view)
		view->closeView();
}
void KTechlab::slotViewSplitLeftRight()
{
	View *view = DocManager::self()->getFocusedView();
	if (!view)
		return;
	ViewContainer *vc = view->viewContainer();
	uint vaId = vc->createViewArea( view->viewAreaId(), ViewArea::Right, true );
// 	view->document()->createView( vc, vaId );
	(void)vaId;
}
void KTechlab::slotViewSplitTopBottom()
{
	View *view = DocManager::self()->getFocusedView();
	if (!view)
		return;
	ViewContainer *vc = view->viewContainer();
	uint vaId = vc->createViewArea( view->viewAreaId(), ViewArea::Bottom, true );
// 	view->document()->createView( vc, vaId );
	(void)vaId;
}

#include "ktechlab.moc"
