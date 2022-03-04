/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <KXMLGUIClient>

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
#include "languagemanager.h"
#include "mechanicsdocument.h"
#include "microlibrary.h"
#include "newfiledlg.h"
#include "oscilloscope.h"
#include "projectmanager.h"
#include "scopescreen.h"
#include "settingsdlg.h"
#include "subcircuits.h"
#include "symbolviewer.h"
#include "textdocument.h"
#include "textview.h"
#include "viewcontainer.h"

// #include <q3dockarea.h>
#include <QTimer>
#include <QToolButton>
// #include <q3ptrlist.h>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QIcon>
#include <QMenu>
#include <QMimeData>
#include <QStandardPaths>
#include <QTabWidget>

#include <KActionCollection>
#include <KEditToolBar>
#include <KSharedConfig>
#include <KToolBar>

//#include <kkeydialog.h>
#include <KLocalizedString>
#include <KMessageBox>
//#include <kpopupmenu.h>
//#include <kwin.h>
#include <KRecentFilesAction>
#include <KShortcutsDialog>
#include <KStandardAction>
#include <KToolBarPopupAction>
#include <KWindowSystem>
#include <KXMLGUIFactory>

#include <ktlconfig.h>
#include <ktechlab_debug.h>

KTechlab *KTechlab::m_pSelf = nullptr;

KTechlab::KTechlab()
    : KateMDI::MainWindow(nullptr)
{
    setObjectName("KTechlab");
    m_pSelf = this;

    QTime ct;
    ct.start();

    m_bIsShown = false;
    m_pContainerDropSource = nullptr;
    m_pContainerDropReceived = nullptr;
    m_pContextMenuContainer = nullptr;
    m_pFocusedContainer = nullptr;
    m_pToolBarOverlayLabel = nullptr;

    if (QFontInfo(m_itemFont).pixelSize() > 11) {
        // It has to be > 11, not > 12, as (I think) pixelSize() rounds off the actual size
        m_itemFont.setPixelSize(12);
    }

    m_pUpdateCaptionsTimer = new QTimer(this);
    connect(m_pUpdateCaptionsTimer, &QTimer::timeout, this, &KTechlab::slotUpdateCaptions);

    setMinimumSize(400, 400);

    setupTabWidget();
    setupToolDocks();
    setupActions();
    setupView();
    // readProperties( KGlobal::config() );
    KSharedConfigPtr cfg = KSharedConfig::openConfig();
    readPropertiesInConfig(cfg.data());

    // 	qCDebug(KTL_LOG) << "Constructor time: " << ct.elapsed();
}

KTechlab::~KTechlab()
{
    fileMetaInfo()->saveAllMetaInfo();

    for (ViewContainerList::Iterator itVc = m_viewContainerList.begin(); itVc != m_viewContainerList.end(); ++itVc) {
        ViewContainer *vc = itVc->data();
        disconnect(vc, &ViewContainer::destroyed, this, &KTechlab::slotViewContainerDestroyed);
    }

    delete fileMetaInfo();
    delete itemLibrary(); // This better be the last time the item library is used!
    delete subcircuits();
}

void KTechlab::show()
{
    KateMDI::MainWindow::show();
    m_bIsShown = true;
}

void KTechlab::openFile(ViewArea *viewArea)
{
    const QList<QUrl> files = getFileURLs(false);
    if (files.isEmpty())
        return;

    load(files.first(), viewArea);
}

void KTechlab::load(const QUrl &url, ViewArea *viewArea)
{
    if (!url.isValid())
        return;

    loadInternal(url, false, viewArea);
}

void KTechlab::slotLoadRecent(const QUrl &url)
{
    loadInternal(url, true);
}

void KTechlab::loadInternal(const QUrl &url, bool isRecent, ViewArea *viewArea)
{
    if (url.fileName().endsWith(".ktechlab", Qt::CaseInsensitive)) {
        // This is a ktechlab project; it has to be handled separetly from a
        // normal file.

        ProjectManager::self()->slotOpenProject(url);
        return;
    }

    if (!isRecent)
        addRecentFile(url);

    // set our caption
    setCaption(url.toDisplayString(QUrl::PreferLocalFile));

    // load in the file (target is always local)
    DocManager::self()->openURL(url, viewArea);
}

QList<QUrl> KTechlab::recentFiles()
{
    return m_recentFiles->urls();
}

void KTechlab::setupToolDocks()
{
    setToolViewStyle(KMultiTabBar::KDEV3ICON);

    KateMDI::ToolView *tv = nullptr;

    tv = createToolView(ProjectManager::toolViewIdentifier(), KMultiTabBar::Left, QIcon::fromTheme("attach"), i18n("Project"));
    tv->setObjectName("ProjectManager-ToolView");
    ProjectManager::self(tv);

    tv = createToolView(ComponentSelector::toolViewIdentifier(), KMultiTabBar::Left, QIcon::fromTheme("circuit"), i18n("Components"));
    tv->setObjectName("ComponentSelector-ToolView");
    ComponentSelector::self(tv);

    // Create an instance of the subcircuits interface, now that we have created the component selector
    subcircuits();
    Subcircuits::loadSubcircuits();

    tv = createToolView(FlowPartSelector::toolViewIdentifier(), KMultiTabBar::Left, QIcon::fromTheme("flowcode"), i18n("Flow Parts"));
    tv->setObjectName("FlowPartSelector-ToolView");
    FlowPartSelector::self(tv);

#ifdef MECHANICS
    tv = createToolView(MechanicsSelector::toolViewIdentifier(), KMultiTabBar::Left, QIcon::fromTheme("mechanics"), i18n("Mechanics"));
    tv->setObjectName("MechanicsSelector-ToolView");
    MechanicsSelector::self(tv);
#endif

    tv = createToolView(ItemEditor::toolViewIdentifier(), KMultiTabBar::Right, QIcon::fromTheme("item"), i18n("Item Editor"));
    tv->setObjectName("ItemEditor-ToolView");
    ItemEditor::self(tv);

    tv = createToolView(ContextHelp::toolViewIdentifier(), KMultiTabBar::Right, QIcon::fromTheme("help-contents"), i18n("Context Help"));
    tv->setObjectName("ContextHelp-ToolView");
    ContextHelp::self(tv);

    tv = createToolView(LanguageManager::toolViewIdentifier(), KMultiTabBar::Bottom, QIcon::fromTheme("utilities-log-viewer"), i18n("Messages"));
    tv->setObjectName("LanguageManager-ToolView");
    LanguageManager::self(tv);

#ifndef NO_GPSIM
    tv = createToolView(SymbolViewer::toolViewIdentifier(), KMultiTabBar::Right, QIcon::fromTheme("blockdevice"), i18n("Symbol Viewer"));
    tv->setObjectName("SymbolViewer-ToolView");
    SymbolViewer::self(tv);
#endif

    addOscilloscopeAsToolView(this);
#if 1
    tv = createToolView(ScopeScreen::toolViewIdentifier(), KMultiTabBar::Bottom, QIcon::fromTheme("oscilloscope"), i18n("Scope Screen (Very Rough)"));
    tv->setObjectName("ScopeScreen-ToolView");
    ScopeScreen::self(tv);
#endif

    updateSidebarMinimumSizes();
}

void KTechlab::addWindow(ViewContainer *vc)
{
    if (vc && !m_viewContainerList.contains(vc)) {
        m_viewContainerList << vc;
        connect(vc, &ViewContainer::destroyed, this, &KTechlab::slotViewContainerDestroyed);
    }

    m_viewContainerList.removeAll((ViewContainer *)nullptr);
    slotUpdateTabWidget();
    slotDocModifiedChanged();
}

void KTechlab::setupView()
{
    setAcceptDrops(true);
    setStandardToolBarMenuEnabled(true);
    setXMLFile("ktechlabui.rc");
    createShellGUI(true);
    // 2017.09.04 - no need to explicitly add the new file selector to toolbar
    ////action("newfile_popup")->plug( toolBar("mainToolBar"), 0 );
    // actionByName("file_new")->addTo( toolBar("mainToolBar") );
    ////action("file_new")->unplug( toolBar("mainToolBar") );
    // actionByName("file_new")->removeFrom( toolBar("mainToolBar") );
    setupExampleActions();
    statusBar()->show();
}

void KTechlab::overlayToolBarScreenshot()
{
    return;

    if (!m_pToolBarOverlayLabel) {
        m_pToolBarOverlayLabel = new QLabel(nullptr,
                                            Qt::WindowStaysOnTopHint /* | Qt::WStyle_Customize */ |
                                                Qt::FramelessWindowHint
                                                /*| Qt::WNoAutoErase */
                                                | Qt::Popup);
        m_pToolBarOverlayLabel->hide();
        // m_pToolBarOverlayLabel->setBackgroundMode( Qt::NoBackground ); // 2018.12.02
        QPalette p;
        p.setColor(m_pToolBarOverlayLabel->backgroundRole(), Qt::transparent);
        m_pToolBarOverlayLabel->setPalette(p);
    }

    if (!m_bIsShown) {
        // The window isn't visible yet, so there's nothing to overlay (and if we tried,
        // it would appear as a strange floating toolbar).
        return;
    }

    if (m_pToolBarOverlayLabel->isVisible()) {
        // This is to avoid successive calls to removeGUIClient when we have
        // already popped it up for the first call, and don't want to take
        // another screenshot (as that would be without the toolbar).
        return;
    }

    // QPtrListIterator<KToolBar> toolBarIterator(); // unused?

    // 	QWidget * toolsWidget = toolBar( "toolsToolBar" );
    // 	QWidget * debugWidget = toolBar( "debugTB" );

    // KToolBar * toolsWidget = static_cast<KToolBar*>(child( "toolsToolBar", "KToolBar" )); // 2018.08.18 - see below
    // KToolBar * debugWidget = static_cast<KToolBar*>(child( "debugTB", "KToolBar" ));
    KToolBar *toolsWidget = findChild<KToolBar *>("toolsToolBar");
    KToolBar *debugWidget = findChild<KToolBar *>("debugTB");

    if (!toolsWidget && !debugWidget)
        return;

    QWidget *parent = static_cast<QWidget *>(toolsWidget ? toolsWidget->parent() : debugWidget->parent());

    QRect grabRect;

    // 128 is a sanity check (widget can do strange things when being destroyed)

    if (toolsWidget && toolsWidget->height() <= 128)
        grabRect |= toolsWidget->geometry();
    if (debugWidget && debugWidget->height() <= 128)
        grabRect |= debugWidget->geometry();

    if (!grabRect.isValid())
        return;

    QPixmap shot = parent->grab(grabRect);

    m_pToolBarOverlayLabel->move(parent->mapToGlobal(grabRect.topLeft()));
    m_pToolBarOverlayLabel->setFixedSize(grabRect.size());
    m_pToolBarOverlayLabel->setPixmap(shot);
    m_pToolBarOverlayLabel->show();

    QTimer::singleShot(100, this, SLOT(hideToolBarOverlay()));
}

void KTechlab::hideToolBarOverlay()
{
    if (!m_pToolBarOverlayLabel)
        return;

    // 	QWidget * hiddenWidget = toolBar( "toolsToolBar" );
    // 	if ( !hiddenWidget )
    // 		return;

    // 	hiddenWidget->setBackgroundMode( NoBackground );
    // 	hiddenWidget->setWFlags( WNoAutoErase );
    // 	hiddenWidget->setUpdatesEnabled( false );

    m_pToolBarOverlayLabel->hide();
}

void KTechlab::addNoRemoveGUIClient(KXMLGUIClient *client)
{
    if (client && !m_noRemoveGUIClients.contains(client))
        m_noRemoveGUIClients << client;
}

void KTechlab::removeGUIClients()
{
    QList<KXMLGUIClient *> clientsToRemove;

    QList<KXMLGUIClient *> clients = factory()->clients();
    // for ( KXMLGUIClient * client = clients.first(); client; client = clients.next() )
    for (QList<KXMLGUIClient *>::iterator itClient = clients.begin(); itClient != clients.end(); ++itClient) {
        // if ( client && client != this && !m_noRemoveGUIClients.contains( client ) )
        if ((*itClient != this) && (!m_noRemoveGUIClients.contains(*itClient)))
            clientsToRemove << *itClient;
    }

    if (clients.isEmpty())
        return;

    overlayToolBarScreenshot();

    QList<KXMLGUIClient *>::iterator end = clientsToRemove.end();
    for (QList<KXMLGUIClient *>::iterator it = clientsToRemove.begin(); it != end; ++it)
        factory()->removeClient(*it);
}

void KTechlab::setupTabWidget()
{
    m_pViewContainerTabWidget = new QTabWidget(centralWidget());
    if (centralWidget()->layout()) {
        centralWidget()->layout()->addWidget(m_pViewContainerTabWidget);
    } else {
        qCWarning(KTL_LOG) << " unexpected null layout for " << centralWidget();
    }
    connect(tabWidget(), &QTabWidget::currentChanged, this, &KTechlab::slotViewContainerActivated);

    // KConfig *config = kapp->config();
    // config->setGroup("UI");
    KSharedConfigPtr cfg = KSharedConfig::openConfig();
    KConfigGroup grUi = cfg->group("UI");

    bool CloseOnHover = grUi.readEntry("CloseOnHover", false);
    // tabWidget()->setHoverCloseButton( CloseOnHover ); // 2018.08.18 - see below
    tabWidget()->setTabsClosable(CloseOnHover);

    bool CloseOnHoverDelay = grUi.readEntry("CloseOnHoverDelay", false);
    // tabWidget()->setHoverCloseButtonDelayed( CloseOnHoverDelay ); // 2018.08.18 - see below
    tabWidget()->setTabsClosable(CloseOnHoverDelay);

    if (grUi.readEntry("ShowCloseTabsButton", true)) {
        QToolButton *but = new QToolButton(tabWidget());
        but->setIcon(QIcon::fromTheme("tab-close"));
        but->adjustSize();
        but->hide();
        connect(but, &QToolButton::clicked, this, &KTechlab::slotViewContainerClose);
        tabWidget()->setCornerWidget(but, Qt::TopRightCorner);
    }

    QTabBar *tabBar = tabWidget()->tabBar();
    tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tabBar, &QTabBar::customContextMenuRequested, this, &KTechlab::slotTabContext);
}

void KTechlab::slotUpdateTabWidget()
{
    m_viewContainerList.removeAll((ViewContainer *)nullptr);

    bool noWindows = m_viewContainerList.isEmpty();

    if (QWidget *button = tabWidget()->cornerWidget(Qt::TopRightCorner))
        button->setHidden(noWindows);

    if (noWindows)
        setCaption(nullptr);
}

void KTechlab::setupActions()
{
    KActionCollection *ac = actionCollection();
    // TODO move the actions from KTechLab object level to document view level for
    //  all types of documents; see actions marked with (1)! and TextView constructor

    // 2017.08.06 - using custom new action
    // KStandardAction::openNew(			this, SLOT(slotFileNew()),					ac );
    {
        QAction *openAction = KStandardAction::open(this, SLOT(slotFileOpen()), ac);
        openAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    }
    KStandardAction::save(this, SLOT(slotFileSave()), ac); // (1)!
    KStandardAction::saveAs(this, SLOT(slotFileSaveAs()), ac);
    KStandardAction::close(this, SLOT(slotViewClose()), ac);
    KStandardAction::print(this, SLOT(slotFilePrint()), ac); // (1)!
    KStandardAction::quit(this, SLOT(slotFileQuit()), ac);
    KStandardAction::undo(this, SLOT(slotEditUndo()), ac);   // (1)!
    KStandardAction::redo(this, SLOT(slotEditRedo()), ac);   // (1)!
    KStandardAction::cut(this, SLOT(slotEditCut()), ac);     // (1)!
    KStandardAction::copy(this, SLOT(slotEditCopy()), ac);   // (1)!
    KStandardAction::paste(this, SLOT(slotEditPaste()), ac); // (1)!
    KStandardAction::keyBindings(this, SLOT(slotOptionsConfigureKeys()), ac);
    KStandardAction::configureToolbars(this, SLOT(slotOptionsConfigureToolbars()), ac);
    KStandardAction::preferences(this, SLOT(slotOptionsPreferences()), ac);

    // BEGIN New file popup
    // KToolBarPopupAction *p = new KToolBarPopupAction( i18n("&New"), "document-new",
    //          KStandardShortcut::shortcut(KStandardShortcut::New), this, SLOT(slotFileNew()), ac, "newfile_popup" );
    KToolBarPopupAction *p = new KToolBarPopupAction(QIcon::fromTheme("document-new"), i18n("&New"), ac);
    p->setObjectName("file_new");
    p->setShortcuts(KStandardShortcut::shortcut(KStandardShortcut::New));
    connect(p, &KToolBarPopupAction::triggered, this, &KTechlab::slotFileNew);
    ac->addAction(p->objectName(), p);
    p->menu()->setTitle(i18n("New File"));
    {
        //(new QAction( i18n("Assembly"), "source", 0, this, SLOT(slotFileNewAssembly()), ac, "newfile_asm" ))->plug( p->menu() );
        QAction *a = new QAction(QIcon::fromTheme("source"), i18n("Assembly"), ac);
        a->setObjectName("newfile_asm");
        connect(a, &QAction::triggered, this, &KTechlab::slotFileNewAssembly);
        p->menu()->addAction(a);
        ac->addAction(a->objectName(), a);
    }
    {
        //(new QAction( i18n("C source"), "text-x-csrc", 0, this, SLOT(slotFileNewC()), ac, "newfile_c" ))->plug( p->menu() );
        QAction *a = new QAction(QIcon::fromTheme("text-x-csrc"), i18n("C source"), ac);
        a->setObjectName("newfile_c");
        connect(a, &QAction::triggered, this, &KTechlab::slotFileNewC);
        p->menu()->addAction(a);
        ac->addAction(a->objectName(), a);
    }
    {
        //(new QAction( i18n("Circuit"), "application-x-circuit", 0, this, SLOT(slotFileNewCircuit()), ac, "newfile_circuit" ))->plug( p->menu() );
        QAction *a = new QAction(QIcon::fromTheme("application-x-circuit"), i18n("Circuit"), ac);
        a->setObjectName("newfile_circuit");
        connect(a, &QAction::triggered, this, &KTechlab::slotFileNewCircuit);
        p->menu()->addAction(a);
        ac->addAction(a->objectName(), a);
    }
    {
        //(new QAction( i18n("FlowCode"), "application-x-flowcode", 0, this, SLOT(slotFileNewFlowCode()), ac, "newfile_flowcode" ))->plug( p->menu() );
        QAction *a = new QAction(QIcon::fromTheme("application-x-flowcode"), i18n("FlowCode"), ac);
        a->setObjectName("newfile_flowcode");
        connect(a, &QAction::triggered, this, &KTechlab::slotFileNewFlowCode);
        p->menu()->addAction(a);
        ac->addAction(a->objectName(), a);
    }
#ifdef MECHANICS
    {
        //(new QAction( i18n("Mechanics"), "ktechlab_mechanics", 0, this, SLOT(slotFileNewMechanics()), ac, "newfile_mechanics" ))->plug( p->menu() );
        QAction *a = new QAction(QIcon::fromTheme("" /* "ktechlab_mechanics" -- for the future */), i18n("Mechanics"), ac);
        a->setObjectName("newfile_mechanics");
        connect(a, &QAction::triggered, this, &KTechlab::slotFileNewMechanics);
        p->menu()->addAction(a);
        ac->addAction(a->objectName(), a);
    }
#endif
    {
        //(new QAction( "Microbe", "application-x-microbe", 0, this, SLOT(slotFileNewMicrobe()), ac, "newfile_microbe" ))->plug( p->menu() );
        QAction *a = new QAction(QIcon::fromTheme("application-x-microbe"), i18n("Microbe"), ac);
        a->setObjectName("newfile_microbe");
        connect(a, &QAction::triggered, this, &KTechlab::slotFileNewMicrobe);
        p->menu()->addAction(a);
        ac->addAction(a->objectName(), a);
    }
    // END New File popup

    m_recentFiles = KStandardAction::openRecent(this, SLOT(slotLoadRecent(QUrl)), ac);

    m_statusbarAction = KStandardAction::showStatusbar(this, SLOT(slotOptionsShowStatusbar()), ac);
    ac->addAction(m_statusbarAction->objectName(), m_statusbarAction);

    // BEGIN Project Actions
    ProjectManager *pm = ProjectManager::self();
    {
        // new QAction( i18n("New Project..."), "window-new",			0, pm, SLOT(slotNewProject()),			ac, 	"project_new" );
        QAction *a = new QAction(QIcon::fromTheme("window-new"), i18n("New Project..."), ac);
        a->setObjectName("project_new");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotNewProject);
        ac->addAction(a->objectName(), a);
    }
    {
        // new QAction( i18n("Open Project..."), "project-open",		0, pm, SLOT(slotOpenProject()),			ac, 	"project_open" );
        QAction *a = new QAction(QIcon::fromTheme("project-open"), i18n("Open Project..."), ac);
        a->setObjectName("project_open");
        connect(a, &QAction::triggered, pm, qOverload<>(&ProjectManager::slotOpenProject));
        ac->addAction(a->objectName(), a);
    }
    {
        m_recentProjects = new KRecentFilesAction(i18n("Open &Recent Project..."), ac);
        m_recentProjects->setObjectName("project_open_recent");
        connect(m_recentProjects, SIGNAL(urlSelected(QUrl)), ProjectManager::self(), SLOT(slotOpenProject(QUrl)));
        ac->addAction(m_recentProjects->objectName(), m_recentProjects);
    }
    {
        // new QAction( i18n("Export to Makefile..."), "document-export",	0, pm, SLOT(slotExportToMakefile()),		ac, "project_export_makefile" );
        QAction *a = new QAction(QIcon::fromTheme("document-export"), i18n("Export to Makefile..."), ac);
        a->setObjectName("project_export_makefile");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotExportToMakefile);
        ac->addAction(a->objectName(), a);
    }
    {
        // new QAction( i18n("Create Subproject..."), 0,				0, pm, SLOT(slotCreateSubproject()),		ac, "project_create_subproject" );
        QAction *a = new QAction(i18n("Create Subproject..."), ac);
        a->setObjectName("project_create_subproject");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotCreateSubproject);
        ac->addAction(a->objectName(), a);
    }
    {
        // new QAction( i18n("Add Existing File..."), "document-open",		0, pm, SLOT(slotAddFile()),					ac, "project_add_existing_file" );
        QAction *a = new QAction(QIcon::fromTheme("document-open"), i18n("Add Existing File..."), ac);
        a->setObjectName("project_add_existing_file");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotAddFile);
        ac->addAction(a->objectName(), a);
    }
    {
        // new QAction( i18n("Add Current File..."), "document-import",		0, pm, SLOT(slotAddCurrentFile()),			ac, "project_add_current_file" );
        QAction *a = new QAction(QIcon::fromTheme("document-import"), i18n("Add Current File..."), ac);
        a->setObjectName("project_add_current_file");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotAddCurrentFile);
        ac->addAction(a->objectName(), a);
    }
    // 	new QAction( i18n("Project Options"), "configure",			0, pm, SLOT(slotProjectOptions()),			ac, "project_options" );
    // ^ TODO why commented?
    {
        // new QAction( i18n("Close Project"), "window-close",			0, pm, SLOT(slotCloseProject()),			ac, "project_close" );
        QAction *a = new QAction(QIcon::fromTheme("window-close"), i18n("Close Project"), ac);
        a->setObjectName("project_close");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotCloseProject);
        ac->addAction(a->objectName(), a);
    }
    {
        // new QAction( i18n("Remove from Project"), "edit-delete",		0, pm, SLOT(slotRemoveSelected()),			ac, "project_remove_selected" );
        QAction *a = new QAction(QIcon::fromTheme("edit-delete"), i18n("Remove from Project"), ac);
        a->setObjectName("project_remove_selected");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotRemoveSelected);
        ac->addAction(a->objectName(), a);
    }
    {
        // new QAction( i18n("Insert Existing File..."), "document-open",	0, pm, SLOT(slotSubprojectAddExistingFile()),	ac, "subproject_add_existing_file" );
        QAction *a = new QAction(QIcon::fromTheme("document-open"), i18n("Insert Existing File..."), ac);
        a->setObjectName("subproject_add_existing_file");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotSubprojectAddExistingFile);
        ac->addAction(a->objectName(), a);
    }
    {
        // new QAction( i18n("Insert Current File..."), "document-import",	0, pm, SLOT(slotSubprojectAddCurrentFile()),ac, "subproject_add_current_file" );
        QAction *a = new QAction(QIcon::fromTheme("document-import"), i18n("Insert Current File..."), ac);
        a->setObjectName("subproject_add_current_file");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotSubprojectAddCurrentFile);
        ac->addAction(a->objectName(), a);
    }
    {
        // new QAction( i18n("Linker Options..."), "configure",		0, pm, SLOT(slotSubprojectLinkerOptions()),	ac, "project_item_linker_options" );
        QAction *a = new QAction(QIcon::fromTheme("configure"), i18n("Linker Options..."), ac);
        a->setObjectName("project_item_linker_options");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotSubprojectLinkerOptions);
        ac->addAction(a->objectName(), a);
    }
    {
        // new QAction( i18n("Build..."), "launch",					0, pm, SLOT(slotItemBuild()),				ac, "project_item_build" );
        QAction *a = new QAction(QIcon::fromTheme("run-build"), i18n("Build..."), ac);
        a->setObjectName("project_item_build");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotItemBuild);
        ac->addAction(a->objectName(), a);
    }
    {
        // new QAction( i18n("Upload..."), "convert_to_pic",			0, pm, SLOT(slotItemUpload()),				ac, "project_item_upload" );
        QAction *a = new QAction(QIcon::fromTheme("convert_to_pic"), i18n("Upload..."), ac);
        a->setObjectName("project_item_upload");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotItemUpload);
        ac->addAction(a->objectName(), a);
    }
    {
        // new QAction( i18n("Processing Options..."), "configure",	0, pm, SLOT(slotItemProcessingOptions()),	ac, "project_item_processing_options" );
        QAction *a = new QAction(QIcon::fromTheme("configure"), i18n("Processing Options..."), ac);
        a->setObjectName("project_item_processing_options");
        connect(a, &QAction::triggered, pm, &ProjectManager::slotItemProcessingOptions);
        ac->addAction(a->objectName(), a);
    }
    // END Project Actions

    {
        // new QAction( i18n("Split View Left/Right"), "view-split-left-right", Qt::CTRL|Qt::SHIFT|Qt::Key_L, this, SLOT(slotViewSplitLeftRight()), ac, "view_split_leftright" );
        QAction *a = new QAction(QIcon::fromTheme("view-split-left-right"), i18n("Split View Left/Right"), ac);
        a->setObjectName("view_split_leftright");
        connect(a, &QAction::triggered, this, &KTechlab::slotViewSplitLeftRight);
        ac->addAction(a->objectName(), a);
    }
    {
        // new QAction( i18n("Split View Top/Bottom"), "view-split-top-bottom", Qt::CTRL|Qt::SHIFT|Qt::Key_T, this, SLOT(slotViewSplitTopBottom()), ac, "view_split_topbottom" );
        QAction *a = new QAction(QIcon::fromTheme("view-split-top-bottom"), i18n("Split View Top/Bottom"), ac);
        a->setObjectName("view_split_topbottom");
        connect(a, &QAction::triggered, this, &KTechlab::slotViewSplitTopBottom);
        ac->addAction(a->objectName(), a);
    }
    {
        // KToggleAction * ta = new KToggleAction( i18n("Run Simulation"), "media-playback-start", Qt::Key_F10, 0, 0, ac, "simulation_run" );
        KToggleAction *ta = new KToggleAction(QIcon::fromTheme("media-playback-start"), i18n("Run Simulation"), ac);
        ac->setDefaultShortcut(ta, Qt::Key_F10);
        ta->setObjectName("simulation_run");
        ta->setChecked(true);
        connect(ta, &KToggleAction::toggled, Simulator::self(), &Simulator::slotSetSimulating);
        ta->setCheckedState(KGuiItem(i18n("Pause Simulation"), "media-playback-pause", nullptr));
        ac->addAction(ta->objectName(), ta);
    }

    // We can call slotCloseProject now that the actions have been created
    ProjectManager::self()->updateActions();
    DocManager::self()->disableContextActions();
}

void KTechlab::setupExampleActions()
{
    QStringList categories;
    categories << "555"
               << "basic"
               << "diodes"
               << "jfets"
               << "opamps"
               << "matrix_disp_driver"
               << "mosfets"
               << "transistors";

    // A name for the example circuit can be generated from the filename (and
    // will be done so if the filename is not in this list). But the name
    // generation is not that intelligent (e.g. and.circuit should be called
    // "AND", not "And" - poor capitalization. And it also allows translation of
    // names.
    QStringMap filesToNames;
    filesToNames["internals.circuit"] = i18n("Internals");
    filesToNames["square-wave.circuit"] = i18n("Square Wave");
    filesToNames["2-way-switch.circuit"] = i18n("2-Way Switch");
    filesToNames["3-way-switch.circuit"] = i18n("3-Way Switch");
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
    filesToNames["scmitt-trigger.circuit"] = i18n("Schmitt Trigger");
    filesToNames["switch.circuit"] = i18n("Switch");
    filesToNames["matrix_display.circuit"] = i18n("Matrix Display Driver");

    int at = 0;

    for (QString category : categories) {
        QDir dir(QStandardPaths::locate(QStandardPaths::AppDataLocation, "examples/" + category + "/", QStandardPaths::LocateDirectory));

        // K3PopupMenu * m = static_cast<K3PopupMenu*>(factory()->container( "examples_" + category, this ));
        QMenu *m = static_cast<QMenu *>(factory()->container("examples_" + category, this));
        if (!m) {
            qCWarning(KTL_LOG) << "failed to cast to popup menu: "
                       << "examples_" + category;
            continue;
        }
        connect(m, &QMenu::triggered, this, &KTechlab::openExample);

        QStringList files = dir.entryList();
        files.removeAll(".");
        files.removeAll("..");

        for (QString fileName : files) {
            QString name = filesToNames[fileName];
            if (name.isEmpty()) {
                name = fileName;
                name.remove(".circuit");
                name.replace("-", " ");
                name.replace("_", " ");

                // Capitalize the start of each word
                bool prevWasSpace = true;
                for (int i = 0; i < name.length(); ++i) {
                    if (prevWasSpace)
                        name[i] = name[i].toUpper();
                    prevWasSpace = name[i].isSpace();
                }
            }

            // m->insertItem( name, at, at ); // 2018.12.02
            m->addAction(name)->setData(at);
            m_exampleFiles[at] = dir.path() + "/" + fileName;
            at++;
        }
    }
}

void KTechlab::openExample(QAction *action)
{
    int id = action->data().toInt();
    DocManager::self()->openURL(QUrl::fromLocalFile(m_exampleFiles[id]));
}

void KTechlab::slotViewContainerActivated(int /*index*/)
{
    QWidget *viewContainer = m_pViewContainerTabWidget->currentWidget();
    if (!viewContainer) {
        return; // null viewContainer could be selected, when all documents are closed
    }
    m_pFocusedContainer = dynamic_cast<ViewContainer *>(viewContainer);
    m_pFocusedContainer->setFocus();
}

void KTechlab::slotViewContainerDestroyed(QObject *object)
{
    m_viewContainerList.removeAll(static_cast<ViewContainer *>(object));
    m_viewContainerList.removeAll((ViewContainer *)nullptr);
    slotUpdateTabWidget();
}

QAction *KTechlab::actionByName(const QString &name) const
{
    QAction *action = actionCollection()->action(name);
    if (!action)
        qCCritical(KTL_LOG) << "No such action: " << name;
    return action;
}

void KTechlab::savePropertiesInConfig(KConfig *conf)
{
    // Dumbass KMainWindow - can't handle my width/height correctly. Whoever thought of the "+1" hack anyway?!
    // conf->setGroup("UI");
    KConfigGroup grUi = conf->group("UI");
    grUi.writeEntry("Width", width());
    grUi.writeEntry("Height", height());
    // grUi.writeEntry( "WinState", KWin::windowInfo( winId(), NET::WMState ).state() );
    grUi.writeEntry("WinState", (qulonglong)KWindowInfo(winId(), NET::WMState).state());

#ifndef NO_GPSIM
    SymbolViewer::self()->saveProperties(conf);
#endif

    if (ProjectManager::self()->currentProject()) {
        // conf->setGroup("Project");
        KConfigGroup grProject = conf->group("Project");
        grProject.writePathEntry("Open", ProjectManager::self()->currentProject()->url().toDisplayString(QUrl::PreferLocalFile));
    } else {
        conf->deleteGroup("Project");
    }

    // BEGIN Open Views State
    // Remvoe old entries describing the save state - we don't want a horrible mish-mash of saved states
    const QStringList groupList = conf->groupList();
    const QStringList::const_iterator groupListEnd = groupList.end();
    for (QStringList::const_iterator it = groupList.begin(); it != groupListEnd; ++it) {
        if ((*it).startsWith("ViewContainer"))
            conf->deleteGroup(*it);
    }

    uint viewContainerId = 1;
    const ViewContainerList::iterator vcEnd = m_viewContainerList.end();
    for (ViewContainerList::iterator it = m_viewContainerList.begin(); it != vcEnd; ++it) {
        if (!(*it) || !(*it)->canSaveUsefulStateInfo())
            continue;

        // To make sure the ViewContainers are restored in the right order, we must create them in alphabetical order,
        // as KConfig stores them as such...
        const QString id = QString::number(viewContainerId++).rightJustified(4, '0');

        // conf->setGroup( "ViewContainer " + id );
        KConfigGroup grViewCont = conf->group("ViewContainer " + id);
        //(*it)->saveState(conf);
        (*it)->saveState(&grViewCont);
    }
    // END Open Views State

    KConfigGroup grKateMdi = conf->group("KateMDI");
    saveSession(&grKateMdi);
    // Piss off KMainWindow
    // conf->setGroup("KateMDI");
    int scnum = QApplication::desktop()->screenNumber(parentWidget());
    QRect desk = QApplication::desktop()->screenGeometry(scnum);
    grKateMdi.deleteEntry(QString::fromLatin1("Width %1").arg(desk.width()));
    grKateMdi.deleteEntry(QString::fromLatin1("Height %1").arg(desk.height()));

    conf->sync();
}

void KTechlab::readPropertiesInConfig(KConfig *conf)
{
    startRestore(conf, "KateMDI");

    m_recentFiles->loadEntries(conf->group("Recent Files"));
    m_recentProjects->loadEntries(conf->group("Recent Projects"));

    // BEGIN Restore Open Views
    if (KTLConfig::restoreDocumentsOnStartup()) {
        // If we have a lot of views open from last time, then opening them will take a long time.
        // So we want to enter the qt event loop to finish drawing the window et al before adding the views.
        qApp->processEvents();

        const QStringList groupList = conf->groupList();
        const QStringList::const_iterator groupListEnd = groupList.end();
        for (QStringList::const_iterator it = groupList.begin(); it != groupListEnd; ++it) {
            if ((*it).startsWith("ViewContainer")) {
                ViewContainer *viewContainer = new ViewContainer(*it);

                // conf->setGroup(*it);
                KConfigGroup grIt = conf->group(*it);
                viewContainer->restoreState(&grIt, *it);

                addWindow(viewContainer);
            }
        }
    }
    // END Restore Open Views

    // conf->setGroup("Project");
    KConfigGroup grProject = conf->group("Project");
    const QString openValue = grProject.readPathEntry("Open", QString());
    if (!openValue.isEmpty()) {
        ProjectManager::self()->slotOpenProject(QUrl::fromUserInput(openValue));
    }

#ifndef NO_GPSIM
    SymbolViewer::self()->readProperties(conf);
#endif

    finishRestore();

    // Dumbass KMainWindow - can't handle my width/height correctly. Whoever thought of the "+1" hack anyway?!
    // conf->setGroup("UI");
    KConfigGroup grUi = conf->group("UI");
    resize(grUi.readEntry("Width", 800), grUi.readEntry("Height", 500));
    const quint32 winStateDef = quint32(NET::Max);
    const quint32 winState = grUi.readEntry("WinState", winStateDef /* NET::Max */);
    KWindowSystem::setState(winId(), NET::States(winState));
    // grUi.readEntry( "WinState", (quint32) NET::Max ) );
}

void KTechlab::dragEnterEvent(QDragEnterEvent *event)
{
    // accept uri drops only
    event->setAccepted(event->mimeData()->hasUrls());
}

void KTechlab::dropEvent(QDropEvent *event)
{
    // this is a very simplistic implementation of a drop event.  we
    // will only accept a dropped URL.  the Qt dnd code can do *much*
    // much more, so please read the docs there
    const QList<QUrl> urls = event->mimeData()->urls();

    // see if we can decode a URI.. if not, just ignore it
    if (!urls.isEmpty()) {
        // okay, we have a URI.. process it
        const QUrl &url = urls.first();

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
    KShortcutsDialog::configure(actionCollection(), KShortcutsEditor::LetterShortcutsAllowed, this, true);
}

void KTechlab::slotOptionsConfigureToolbars()
{
    KEditToolBar *dlg = new KEditToolBar(guiFactory());

    if (dlg->exec()) {
        createShellGUI(false);
        createShellGUI(true);
    }

    delete dlg;
}

void KTechlab::slotOptionsPreferences()
{
    // An instance of your dialog could be already created and could be cached,
    // in which case you want to display the cached dialog instead of creating
    // another one
    if (KConfigDialog::showDialog("settings"))
        return;

    // KConfigDialog didn't find an instance of this dialog, so lets create it:
    SettingsDlg *dialog = new SettingsDlg(this, "settings", KTLConfig::self());

    // User edited the configuration - update your local copies of the
    // configuration data
    connect(dialog, &SettingsDlg::settingsChanged, this, &KTechlab::slotUpdateConfiguration);
    dialog->show();
}

void KTechlab::slotUpdateConfiguration()
{
    emit configurationChanged();
}

void KTechlab::slotChangeStatusbar(const QString &text)
{
    // Avoid flicker by repeatedly displaying the same message, as QStatusBar does not check for this
    if (m_lastStatusBarMessage == text)
        return;

    statusBar()->showMessage(text);
    m_lastStatusBarMessage = text;
}

void KTechlab::slotTabContext(const QPoint &pos)
{
    QTabBar *tabBar = tabWidget()->tabBar();
    QWidget *widget = tabWidget()->widget(tabBar->tabAt(pos));
    if (!widget) {
        return;
    }
    const QPoint globalPos = tabBar->mapToGlobal(pos);

    // Shamelessly stolen from KDevelop...

    QMenu *tabMenu = new QMenu;
    tabMenu->addSection((dynamic_cast<ViewContainer *>(widget))->windowTitle());

    // Find the document on whose tab the user clicked
    m_pContextMenuContainer = nullptr;

    m_viewContainerList.removeAll((ViewContainer *)nullptr);

    const ViewContainerList::iterator vcEnd = m_viewContainerList.end();
    for (ViewContainerList::iterator it = m_viewContainerList.begin(); it != vcEnd; ++it) {
        ViewContainer *viewContainer = *it;
        if (viewContainer == widget) {
            m_pContextMenuContainer = viewContainer;

            tabMenu->addAction(QIcon::fromTheme("tab-close"), i18n("Close"))->setData(0);

            View *view = (viewContainer->viewCount() == 1) ? viewContainer->activeView() : nullptr;

            if (view && view->document()->isModified())
                tabMenu->addAction(QIcon::fromTheme("document-save"), i18n("Save"))->setData(1);

            if (view && !view->document()->url().isEmpty())
                tabMenu->addAction(QIcon::fromTheme("view-refresh"), i18n("Reload"))->setData(2);

            if (m_viewContainerList.count() > 1)
                tabMenu->addAction(QIcon::fromTheme("tab-close-other"), i18n("Close All Others"))->setData(4);
        }
    }

    connect(tabMenu, &QMenu::triggered, this, &KTechlab::slotTabContextActivated);

    tabMenu->exec(globalPos);
    delete tabMenu;
}

void KTechlab::slotTabContextActivated(QAction *action)
{
    // Shamelessly stolen from KDevelop...

    if (!m_pContextMenuContainer || !action || action->data().isNull())
        return;

    View *view = m_pContextMenuContainer->activeView();
    if (!view)
        return;
    QPointer<Document> document = view->document();

    switch (action->data().toInt()) {
    case 0: {
        m_pContextMenuContainer->closeViewContainer();
        break;
    }
    case 1:
        document->fileSave();
        break;
    case 2: {
        QUrl url = document->url();
        if (document->fileClose()) {
            delete document;
            DocManager::self()->openURL(url);
        }
        break;
    }
    case 4: {
        const ViewContainerList::iterator vcEnd = m_viewContainerList.end();
        for (ViewContainerList::iterator it = m_viewContainerList.begin(); it != vcEnd; ++it) {
            ViewContainer *viewContainer = *it;
            if (viewContainer && viewContainer != m_pContextMenuContainer) {
                if (!viewContainer->closeViewContainer())
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
        document->slotInitLanguage(TextDocument::ct_asm);
}
void KTechlab::slotFileNewMicrobe()
{
    TextDocument *document = DocManager::self()->createTextDocument();
    if (document)
        document->slotInitLanguage(TextDocument::ct_microbe);
}
void KTechlab::slotFileNewC()
{
    TextDocument *document = DocManager::self()->createTextDocument();
    if (document)
        document->slotInitLanguage(TextDocument::ct_c);
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

    const int accepted = newFileDlg->exec();

    bool addToProject = newFileDlg->addToProject();
    int finalType = newFileDlg->fileType();
    QString microID = newFileDlg->microID();
    int codeType = newFileDlg->codeType();

    delete newFileDlg;
    if (accepted != QDialog::Accepted)
        return;

    Document *created = nullptr;

    if (finalType == Document::dt_circuit)
        created = DocManager::self()->createCircuitDocument();

    else if (finalType == Document::dt_flowcode) {
        FlowCodeDocument *fcd = DocManager::self()->createFlowCodeDocument();
        fcd->setPicType(microID);
        created = fcd;
    }

    else if (finalType == Document::dt_mechanics)
        created = DocManager::self()->createMechanicsDocument();

    else {
        // Presumably a text document
        TextDocument *textDocument = DocManager::self()->createTextDocument();

        if (textDocument)
            textDocument->slotInitLanguage((TextDocument::CodeType)codeType);

        created = textDocument;
    }

    if (created && addToProject)
        created->setAddToProjectOnSave(true);
}

void KTechlab::slotFileOpen()
{
    // this slot is called whenever the File->Open menu is selected,
    // the Open shortcut is pressed (usually CTRL+O) or the Open toolbar
    // button is clicked

    // standard filedialog
    const QList<QUrl> urls = getFileURLs();
    for (const QUrl &url : urls)
        load(url);
}

void KTechlab::addRecentFile(const QUrl &url)
{
    KSharedConfigPtr config = KSharedConfig::openConfig();
    m_recentFiles->addUrl(url);
    m_recentFiles->saveEntries(config->group("Recent Files"));
    config->sync();
    emit recentFileAdded(url);
}

QList<QUrl> KTechlab::getFileURLs(bool allowMultiple)
{
    QString filter;
    filter = QString(
                 "*|%1\n"
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

    if (allowMultiple)
        return QFileDialog::getOpenFileUrls(nullptr, i18n("Open Location"), QUrl(), filter);

    else {
        QUrl ret = QFileDialog::getOpenFileUrl(nullptr, i18n("Open Location"), QUrl(), filter);
        return {ret};
    }
}

void KTechlab::slotDocModifiedChanged()
{
    // BEGIN Set tab icons
    const ViewContainerList::iterator vcEnd = m_viewContainerList.end();
    for (ViewContainerList::iterator it = m_viewContainerList.begin(); it != vcEnd; ++it) {
        ViewContainer *vc = *it;
        if (!vc || !vc->activeView() || !vc->activeView()->document())
            continue;

        QIcon icon;

        if (vc->activeView()->document()->isModified()) {
            icon = QIcon::fromTheme("document-save");
        } else {
            switch (vc->activeView()->document()->type()) {
            case Document::dt_circuit:
                icon = QIcon::fromTheme("application-x-circuit");
                break;

            case Document::dt_flowcode:
                icon = QIcon::fromTheme("application-x-flowcode");
                break;

            case Document::dt_mechanics:
                icon = QIcon::fromTheme("ktechlab_mechanics");
                break;

            case Document::dt_text:
                icon = QIcon::fromTheme("text-x-generic");
                break;

            case Document::dt_pinMapEditor:
                break;

            case Document::dt_none:
                icon = QIcon::fromTheme("application-x-zerosize");
                break;
            }
        }

        tabWidget()->setTabIcon(tabWidget()->indexOf(vc), icon);
    }
    // END Set tab icons
}

void KTechlab::requestUpdateCaptions()
{
    m_pUpdateCaptionsTimer->setSingleShot(true);
    m_pUpdateCaptionsTimer->start(0 /*, true */);
}

void KTechlab::slotUpdateCaptions()
{
    // BEGIN Set KTechlab caption
    Document *document = DocManager::self()->getFocusedDocument();
    QString newCaption;
    if (document) {
        QUrl url = document->url();
        if (url.isEmpty())
            newCaption = document->caption();
        else {
            if (url.isLocalFile() && !url.hasFragment() && !url.hasQuery())
                newCaption = url.toLocalFile();
            else
                newCaption = url.toDisplayString(QUrl::PreferLocalFile);
        }
    } else
        newCaption = "";

    if (newCaption != windowTitle().remove(" - KTechlab")) {
        setWindowTitle(newCaption);
    }
    // END Set KTechlab caption

    // BEGIN Set tab captions
    emit needUpdateCaptions();

    if (document && document->activeView() && document->activeView()->viewContainer()) {
        document->activeView()->viewContainer()->updateCaption();
    }
    // END Set tab captions
}

void KTechlab::slotDocUndoRedoChanged()
{
    Document *document = DocManager::self()->getFocusedDocument();
    if (!document)
        return;

    action("edit_undo")->setEnabled(document->isUndoAvailable());
    action("edit_redo")->setEnabled(document->isRedoAvailable());
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
    KSharedConfigPtr cfgPtr = KSharedConfig::openConfig();
    // saveProperties( KGlobal::config() );
    savePropertiesInConfig(cfgPtr.data());

    if (DocManager::self()->closeAll() && ProjectManager::self()->slotCloseProject()) {
        // Make ourself "deleted"
        m_pSelf = nullptr;
        return true;
    }

    return false;
}

void KTechlab::slotFileQuit()
{
    // close the first window, the list makes the next one the first again.
    // This ensures that queryClose() is called on each window to ask for closing
    // KMainWindow* w;
    if (!memberList().isEmpty()) {
        QList<KMainWindow *> mainWindowList = memberList();
        // for( w=memberList->first(); w!=0; w=memberList->next() )
        for (QList<KMainWindow *>::iterator itWnd = mainWindowList.begin(); itWnd != mainWindowList.end(); ++itWnd) {
            // only close the window if the closeEvent is accepted. If the user presses Cancel on the saveModified() dialog,
            // the window and the application stay open.
            // if( !w->close() ) break;
            if (!(*itWnd)->close())
                break;
        }
    }

    slotChangeStatusbar(i18n("Exiting..."));
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
    uint vaId = vc->createViewArea(view->viewAreaId(), ViewArea::Right, true);
    // 	view->document()->createView( vc, vaId );
    (void)vaId;
}
void KTechlab::slotViewSplitTopBottom()
{
    View *view = DocManager::self()->getFocusedView();
    if (!view)
        return;
    ViewContainer *vc = view->viewContainer();
    uint vaId = vc->createViewArea(view->viewAreaId(), ViewArea::Bottom, true);
    // 	view->document()->createView( vc, vaId );
    (void)vaId;
}
