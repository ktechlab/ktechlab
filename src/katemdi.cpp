/* This file is part of the KDE libraries
   Copyright (C) 2005 Christoph Cullmann <cullmann@kde.org>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

   GUIClient partly based on ktoolbarhandler.cpp: Copyright (C) 2002 Simon Hausmann <hausmann@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "katemdi.h"

#include <kdebug.h>
#include <kglobalsettings.h>
#include <kapplication.h>
#include <klocale.h>
#include <kconfig.h>
#include <kiconloader.h>
//#include <kpopupmenu.h>
#include <kactioncollection.h>
#include <kxmlguifactory.h>
#include <kconfiggroup.h>
#include <kmenu.h>

#include <Qt/q3intdict.h>
#include <Qt/qevent.h>


typedef QList<int> IntList;

namespace KateMDI {

//BEGIN SPLITTER

Splitter::Splitter(Qt::Orientation o, QWidget* parent, const char* name)
  : QSplitter(o, parent, name)
{
}

Splitter::~Splitter()
{
}

bool Splitter::isLastChild(QWidget* w) const
{
  return indexOf(w) == (count() - 1);
  //return ( idAfter( w ) == 0 );
}

int Splitter::idAfter ( QWidget * w ) const
{
  //return QSplitter::idAfter (w);
    return indexOf(w);
}

//END SPLITTER


//BEGIN TOGGLETOOLVIEWACTION

ToggleToolViewAction::ToggleToolViewAction ( const QString& text, const KShortcut& cut, ToolView *tv,
                                             QObject* parent, const char* name )
 : KToggleAction(text /* ,cut */, parent /*,name*/)
 , m_tv(tv)
{
    setName(name);
    KToggleAction::setShortcut(cut.primary());

  connect(this,SIGNAL(toggled(bool)),this,SLOT(slotToggled(bool)));
  connect(m_tv,SIGNAL(visibleChanged(bool)),this,SLOT(visibleChanged(bool)));

  setChecked(m_tv->visible());
}

ToggleToolViewAction::~ToggleToolViewAction()
{
  //unplugAll(); // TODO is this needed?
}

void ToggleToolViewAction::visibleChanged(bool)
{
  if (isChecked() != m_tv->visible())
    setChecked (m_tv->visible());
}

void ToggleToolViewAction::slotToggled(bool t)
{
  if (t)
  {
    m_tv->mainWindow()->showToolView (m_tv);
    m_tv->setFocus ();
  }
  else
  {
    m_tv->mainWindow()->hideToolView (m_tv);
    m_tv->mainWindow()->centralWidget()->setFocus ();
  }
}

//END TOGGLETOOLVIEWACTION


//BEGIN GUICLIENT

GUIClient::GUIClient ( MainWindow *mw )
 : QObject ( mw )
 , KXMLGUIClient ( mw )
 , m_mw (mw)
{
  connect( m_mw->guiFactory(), SIGNAL( clientAdded( KXMLGUIClient * ) ),
           this, SLOT( clientAdded( KXMLGUIClient * ) ) );

  //if (actionCollection()->kaccel()==0)  // TODO what does this do?
  {
    actionCollection()->associateWidget(m_mw);
  }


  // read shortcuts
  //actionCollection()->readShortcutSettings( "Shortcuts", kapp->config() );
  KConfigGroup grShortcuts = KGlobal::config()->group("Shortcuts");
  actionCollection()->readSettings(&grShortcuts);
}

GUIClient::~GUIClient()
{
}

void GUIClient::registerToolView (ToolView *tv)
{
  QString aname = QString("kate_mdi_toolview_") + tv->id;

  // try to read the action shortcut
  KShortcut sc;

//   KConfig *cfg = kapp->config();
//   QString _grp = cfg->group();
//   cfg->setGroup("Shortcuts");
//   sc = KShortcut( cfg->readEntry( aname, "" ) );
//   cfg->setGroup( _grp );

  KConfigGroup grSh = KGlobal::config()->group("Shortcuts");
  sc = KShortcut( grSh.readEntry(aname, "") );
}

void GUIClient::clientAdded( KXMLGUIClient *client )
{
  if ( client == this )
    updateActions();
}

void GUIClient::updateActions()
{
  if ( !factory() )
    return;
}

//END GUICLIENT


//BEGIN TOOLVIEW

ToolView::ToolView (MainWindow *mainwin, Sidebar *sidebar, QWidget *parent)
 : Q3VBox (parent)
 , m_mainWin (mainwin)
 , m_sidebar (sidebar)
 , m_visible (false)
 , persistent (false)
{
}

ToolView::~ToolView ()
{
  m_mainWin->toolViewDeleted (this);
}

void ToolView::setVisible (bool vis)
{
  if (m_visible == vis)
    return;

  m_visible = vis;
  emit visibleChanged (m_visible);
}

bool ToolView::visible () const
{
  return m_visible;
}

void ToolView::childEvent ( QChildEvent *ev )
{
  // set the widget to be focus proxy if possible
    QWidget *childWidget =  qobject_cast< QWidget* >( ev->child() );
  if (ev->inserted() && ev->child() && childWidget) {
    //setFocusProxy ((QWidget *)(ev->child()->qt_cast("QWidget")));
    setFocusProxy( childWidget );
  }

  Q3VBox::childEvent (ev);
}

//END TOOLVIEW


//BEGIN SIDEBAR

Sidebar::Sidebar (KMultiTabBar::KMultiTabBarPosition pos, MainWindow *mainwin, QWidget *parent)
  : KMultiTabBar (
      //(pos == KMultiTabBar::Top || pos == KMultiTabBar::Bottom) ? KMultiTabBar::Horizontal : KMultiTabBar::Vertical, parent)
      pos, parent)
  , m_mainWin (mainwin)
  , m_splitter (0)
  , m_ownSplit (0)
  , m_lastSize (0)
{
  setSidebarPosition( pos );
  setFocusPolicy( Qt::NoFocus );
  hide ();
}

Sidebar::~Sidebar ()
{
}

void Sidebar::setSidebarPosition( KMultiTabBarPosition pos )
{
	m_pos = pos;
	setPosition(pos);
}

void Sidebar::setSidebarStyle( KMultiTabBarStyle style )
{
	m_sidebarTabStyle = style;
	setStyle(style);
}

void Sidebar::setSplitter (Splitter *sp)
{
  m_splitter = sp;
  m_ownSplit = new Splitter ((sidebarPosition() == KMultiTabBar::Top || sidebarPosition() == KMultiTabBar::Bottom) ? Qt::Horizontal : Qt::Vertical, m_splitter);
  m_ownSplit->setOpaqueResize( KGlobalSettings::opaqueResize() );
  m_ownSplit->setChildrenCollapsible( false );
  m_splitter->setResizeMode( m_ownSplit, QSplitter::KeepSize );
  m_ownSplit->hide ();
}

ToolView *Sidebar::addWidget (const QPixmap &icon, const QString &text, ToolView *widget)
{
  static int id = 0;

  if (widget)
  {
    if (widget->sidebar() == this)
      return widget;

    widget->sidebar()->removeWidget (widget);
  }

  int newId = ++id;

  appendTab (icon, newId, text);

  if (!widget)
  {
    widget = new ToolView (m_mainWin, this, m_ownSplit);
    widget->hide ();
    widget->icon = icon;
    widget->text = text;
  }
  else
  {
    widget->hide ();
    widget->reparent (m_ownSplit, 0, QPoint());
    widget->m_sidebar = this;
  }

  // save it's pos ;)
  widget->persistent = false;

  m_idToWidget.insert (newId, widget);
  m_widgetToId.insert (widget, newId);
  m_toolviews.push_back (widget);

  show ();

  connect(tab(newId),SIGNAL(clicked(int)),this,SLOT(tabClicked(int)));
  tab(newId)->installEventFilter(this);

  return widget;
}

void Sidebar::updateMinimumSize()
{
// 	kDebug() << "layout()->margin()="<<layout()->margin()<<endl;
	
	QSize minSize;
	
	QList<ToolView*>::iterator end = m_toolviews.end();
	for ( QList<ToolView*>::iterator it = m_toolviews.begin(); it != end; ++it )
	{
		QSize s = (*it)->childrenRect().size();
		minSize = minSize.expandedTo( s );
// 		kDebug() << "s="<<s<<"(*it)->minimumSize()="<<(*it)->minimumSize()<<endl;
// 		kDebug() << "(*it)->layout()->margin()="<<(*it)->margin()<<endl;
	}
	
	minSize.setWidth( minSize.width() - 30 );
	minSize.setHeight( minSize.height() - 30 );
	
	for ( QList<ToolView*>::iterator it = m_toolviews.begin(); it != end; ++it )
	{
		(*it)->setMinimumSize( minSize );
	}
}

bool Sidebar::removeWidget (ToolView *widget)
{
  if (!m_widgetToId.contains(widget))
    return false;

  removeTab(m_widgetToId[widget]);

  m_idToWidget.remove (m_widgetToId[widget]);
  m_widgetToId.remove (widget);
  m_toolviews.remove (widget);

  bool anyVis = false;
  Q3IntDictIterator<ToolView> it( m_idToWidget );
  for ( ; it.current(); ++it )
  {
    if (!anyVis)
      anyVis =  it.current()->isVisible();
  }

  if (m_idToWidget.isEmpty())
  {
    m_ownSplit->hide ();
    hide ();
  }
  else if (!anyVis)
    m_ownSplit->hide ();

  return true;
}

bool Sidebar::showWidget (ToolView *widget)
{
  if (!m_widgetToId.contains(widget))
    return false;

  // hide other non-persistent views
  Q3IntDictIterator<ToolView> it( m_idToWidget );
  for ( ; it.current(); ++it )
    if ((it.current() != widget) && !it.current()->persistent)
    {
      it.current()->hide();
      setTab (it.currentKey(), false);
      it.current()->setVisible(false);
    }

  setTab (m_widgetToId[widget], true);

  m_ownSplit->show ();
  widget->show ();

  widget->setVisible (true);

  return true;
}

bool Sidebar::hideWidget (ToolView *widget)
{
  if (!m_widgetToId.contains(widget))
    return false;

  bool anyVis = false;

   updateLastSize ();

  for ( Q3IntDictIterator<ToolView> it( m_idToWidget ); it.current(); ++it )
  {
    if (it.current() == widget)
    {
      it.current()->hide();
      continue;
    }

    if (!anyVis)
      anyVis =  it.current()->isVisible();
  }

  // lower tab
  setTab (m_widgetToId[widget], false);

  if (!anyVis)
    m_ownSplit->hide ();

  widget->setVisible (false);

  return true;
}

void Sidebar::tabClicked(int i)
{
  ToolView *w = m_idToWidget[i];

  if (!w)
    return;

  if (isTabRaised(i))
  {
    showWidget (w);
    w->setFocus ();
  }
  else
  {
    hideWidget (w);
    m_mainWin->centralWidget()->setFocus ();
  }
}

bool Sidebar::eventFilter(QObject *obj, QEvent *ev)
{
  if (ev->type()==QEvent::ContextMenu)
  {
    QContextMenuEvent *e = (QContextMenuEvent *) ev;
    KMultiTabBarTab *bt = dynamic_cast<KMultiTabBarTab*>(obj);
    if (bt)
    {
      kDebug()<<"Request for popup"<<endl;

      m_popupButton = bt->id();

      ToolView *w = m_idToWidget[m_popupButton];

      if (w)
      {
        KMenu *p = new KMenu (this);

        p->addTitle(SmallIcon("view_remove"), i18n("Behavior"), 0 /*0 */);

        p->insertItem(w->persistent ? SmallIconSet("window_nofullscreen") : SmallIconSet("window_fullscreen"), w->persistent ? i18n("Make Non-Persistent") : i18n("Make Persistent"), 10);

        p->addTitle(SmallIcon("move"), i18n("Move To"), 0 /* 51 ? */);

		if (sidebarPosition() != 0)
          p->insertItem(SmallIconSet("back"), i18n("Left Sidebar"),0);

		if (sidebarPosition() != 1)
          p->insertItem(SmallIconSet("forward"), i18n("Right Sidebar"),1);

		if (sidebarPosition() != 2)
          p->insertItem(SmallIconSet("up"), i18n("Top Sidebar"),2);

		if (sidebarPosition() != 3)
          p->insertItem(SmallIconSet("down"), i18n("Bottom Sidebar"),3);

        connect(p, SIGNAL(activated(int)),
              this, SLOT(buttonPopupActivate(int)));

        p->exec(e->globalPos());
        delete p;

        return true;
      }
    }
  }

  return false;
}

void Sidebar::buttonPopupActivate (int id)
{
  ToolView *w = m_idToWidget[m_popupButton];

  if (!w)
    return;

  // move ids
  if (id < 4)
  {
    // move + show ;)
    m_mainWin->moveToolView (w, (KMultiTabBar::KMultiTabBarPosition) id);
    m_mainWin->showToolView (w);
  }

  // toggle persistent
  if (id == 10)
    w->persistent = !w->persistent;
}

void Sidebar::updateLastSize ()
{
   QList<int> s = m_splitter->sizes ();

  int i = 0;
  if ((sidebarPosition() == KMultiTabBar::Right || sidebarPosition() == KMultiTabBar::Bottom))
    i = 2;

  // little threshold
  if (s[i] > 2)
    m_lastSize = s[i];
}

class TmpToolViewSorter
{
  public:
    ToolView *tv;
    unsigned int pos;
};

void Sidebar::restoreSession (KConfigGroup *configGr)
{
  // get the last correct placed toolview
  unsigned int firstWrong = 0;
  for ( ; firstWrong < m_toolviews.size(); ++firstWrong )
  {
    ToolView *tv = m_toolviews[firstWrong];

    unsigned int pos = configGr->readEntry (QString ("Kate-MDI-ToolView-%1-Sidebar-Position").arg(tv->id), firstWrong);

    if (pos != firstWrong)
      break;
  }

  // we need to reshuffle, ahhh :(
  if (firstWrong < m_toolviews.size())
  {
    // first: collect the items to reshuffle
    QList<TmpToolViewSorter> toSort;
    for (unsigned int i=firstWrong; i < m_toolviews.size(); ++i)
    {
      TmpToolViewSorter s;
      s.tv = m_toolviews[i];
      s.pos = configGr->readEntry (QString ("Kate-MDI-ToolView-%1-Sidebar-Position").arg(m_toolviews[i]->id), i);
      toSort.push_back (s);
    }

    // now: sort the stuff we need to reshuffle
    for (unsigned int m=0; m < toSort.size(); ++m)
      for (unsigned int n=m+1; n < toSort.size(); ++n)
        if (toSort[n].pos < toSort[m].pos)
        {
          TmpToolViewSorter tmp = toSort[n];
          toSort[n] = toSort[m];
          toSort[m] = tmp;
        }

    // then: remove this items from the button bar
    // do this backwards, to minimize the relayout efforts
    for (int i=m_toolviews.size()-1; i >= (int)firstWrong; --i)
    {
      removeTab (m_widgetToId[m_toolviews[i]]);
    }

    // insert the reshuffled things in order :)
    for (unsigned int i=0; i < toSort.size(); ++i)
    {
      ToolView *tv = toSort[i].tv;

      m_toolviews[firstWrong+i] = tv;

      // readd the button
      int newId = m_widgetToId[tv];
      appendTab (tv->icon, newId, tv->text);
      connect(tab(newId),SIGNAL(clicked(int)),this,SLOT(tabClicked(int)));
      tab(newId)->installEventFilter(this);

      // reshuffle in splitter
      m_ownSplit->moveToLast (tv);
    }
  }

  // update last size if needed
  updateLastSize ();

  // restore the own splitter sizes
  QList<int> s = configGr->readEntry (QString ("Kate-MDI-Sidebar-%1-Splitter").arg(sidebarPosition()), IntList());
  m_ownSplit->setSizes (s);

  // show only correct toolviews, remember persistent values ;)
  bool anyVis = false;
  for ( unsigned int i=0; i < m_toolviews.size(); ++i )
  {
    ToolView *tv = m_toolviews[i];

    tv->persistent = configGr->readEntry (QString ("Kate-MDI-ToolView-%1-Persistent").arg(tv->id), false);
    tv->setVisible (configGr->readEntry (QString ("Kate-MDI-ToolView-%1-Visible").arg(tv->id), false));

    if (!anyVis)
      anyVis = tv->visible();

    setTab (m_widgetToId[tv],tv->visible());

    if (tv->visible())
      tv->show();
    else
      tv->hide ();
  }

  if (anyVis)
    m_ownSplit->show();
  else
    m_ownSplit->hide();
}

void Sidebar::saveSession (KConfigGroup *config)
{
  // store the own splitter sizes
  QList<int> s = m_ownSplit->sizes();
  config->writeEntry (QString ("Kate-MDI-Sidebar-%1-Splitter").arg(sidebarPosition()), s);

  // store the data about all toolviews in this sidebar ;)
  for ( unsigned int i=0; i < m_toolviews.size(); ++i )
  {
    ToolView *tv = m_toolviews[i];

	config->writeEntry (QString ("Kate-MDI-ToolView-%1-Position").arg(tv->id), (int)tv->sidebar()->sidebarPosition());
    config->writeEntry (QString ("Kate-MDI-ToolView-%1-Sidebar-Position").arg(tv->id), i);
    config->writeEntry (QString ("Kate-MDI-ToolView-%1-Visible").arg(tv->id), tv->visible());
    config->writeEntry (QString ("Kate-MDI-ToolView-%1-Persistent").arg(tv->id), tv->persistent);
  }
}

//END SIDEBAR


//BEGIN MAIN WINDOW

MainWindow::MainWindow (QWidget* parentWidget, const char* name)
 : KParts::MainWindow( parentWidget, name)
 , m_restoreConfig (0)
 , m_guiClient (new GUIClient (this))
{
  // init the internal widgets
  Q3HBox *hb = new Q3HBox (this);
  setCentralWidget(hb);

  m_sidebars[KMultiTabBar::Left] = new Sidebar (KMultiTabBar::Left, this, hb);

  m_hSplitter = new Splitter (Qt::Horizontal, hb);
  m_hSplitter->setOpaqueResize( KGlobalSettings::opaqueResize() );

  m_sidebars[KMultiTabBar::Left]->setSplitter (m_hSplitter);

  Q3VBox *vb = new Q3VBox (m_hSplitter);
  m_hSplitter->setCollapsible(vb, false);

  m_sidebars[KMultiTabBar::Top] = new Sidebar (KMultiTabBar::Top, this, vb);

  m_vSplitter = new Splitter (Qt::Vertical, vb);
  m_vSplitter->setOpaqueResize( KGlobalSettings::opaqueResize() );

  m_sidebars[KMultiTabBar::Top]->setSplitter (m_vSplitter);

  m_centralWidget = new Q3VBox (m_vSplitter);
  m_vSplitter->setCollapsible(m_centralWidget, false);

  m_sidebars[KMultiTabBar::Bottom] = new Sidebar (KMultiTabBar::Bottom, this, vb);
  m_sidebars[KMultiTabBar::Bottom]->setSplitter (m_vSplitter);

  m_sidebars[KMultiTabBar::Right] = new Sidebar (KMultiTabBar::Right, this, hb);
  m_sidebars[KMultiTabBar::Right]->setSplitter (m_hSplitter);
}

MainWindow::~MainWindow ()
{
  // cu toolviews
  while (!m_toolviews.isEmpty())
    delete m_toolviews[0];

  // seems like we really should delete this by hand ;)
  delete m_centralWidget;

  for (unsigned int i=0; i < 4; ++i)
    delete m_sidebars[i];
}

QWidget *MainWindow::centralWidget () const
{
  return m_centralWidget;
}

ToolView *MainWindow::createToolView (const QString &identifier, KMultiTabBar::KMultiTabBarPosition pos, const QPixmap &icon, const QString &text)
{
  if (m_idToWidget[identifier])
    return 0;

  // try the restore config to figure out real pos
  if (m_restoreConfig && m_restoreConfig->hasGroup (m_restoreGroup))
  {
    KConfigGroup grRest = m_restoreConfig->group (m_restoreGroup);
    pos = (KMultiTabBar::KMultiTabBarPosition) grRest.readEntry (QString ("Kate-MDI-ToolView-%1-Position").arg(identifier), (int) pos);
  }

  ToolView *v  = m_sidebars[pos]->addWidget (icon, text, 0);
  v->id = identifier;

  m_idToWidget.insert (identifier, v);
  m_toolviews.push_back (v);

  // register for menu stuff
  m_guiClient->registerToolView (v);

  return v;
}

ToolView *MainWindow::toolView (const QString &identifier) const
{
  return m_idToWidget[identifier];
}

void MainWindow::toolViewDeleted (ToolView *widget)
{
  if (!widget)
    return;

  if (widget->mainWindow() != this)
    return;

  // unregister from menu stuff

  widget->sidebar()->removeWidget (widget);

  m_idToWidget.remove (widget->id);
  m_toolviews.remove (widget);
}

void MainWindow::setToolViewStyle (KMultiTabBar::KMultiTabBarStyle style)
{
	m_sidebars[0]->setSidebarStyle(style);
	m_sidebars[1]->setSidebarStyle(style);
	m_sidebars[2]->setSidebarStyle(style);
	m_sidebars[3]->setSidebarStyle(style);
}

KMultiTabBar::KMultiTabBarStyle MainWindow::toolViewStyle () const
{
  // all sidebars have the same style, so just take Top
  return m_sidebars[KMultiTabBar::Top]->sidebarTabStyle();
}

bool MainWindow::moveToolView (ToolView *widget, KMultiTabBar::KMultiTabBarPosition pos)
{
  if (!widget || widget->mainWindow() != this)
    return false;

  // try the restore config to figure out real pos
  if (m_restoreConfig && m_restoreConfig->hasGroup (m_restoreGroup))
  {
    KConfigGroup grRest = m_restoreConfig->group (m_restoreGroup);
    pos = (KMultiTabBar::KMultiTabBarPosition) grRest.readEntry (QString ("Kate-MDI-ToolView-%1-Position").arg(widget->id), (int) pos);
  }

  m_sidebars[pos]->addWidget (widget->icon, widget->text, widget);

  return true;
}

bool MainWindow::showToolView (ToolView *widget)
{
  if (!widget || widget->mainWindow() != this)
    return false;

  // skip this if happens during restoring, or we will just see flicker
  if (m_restoreConfig && m_restoreConfig->hasGroup (m_restoreGroup))
    return true;

  return widget->sidebar()->showWidget (widget);
}

bool MainWindow::hideToolView (ToolView *widget)
{
  if (!widget || widget->mainWindow() != this)
    return false;

  // skip this if happens during restoring, or we will just see flicker
  if (m_restoreConfig && m_restoreConfig->hasGroup (m_restoreGroup))
    return true;

  return widget->sidebar()->hideWidget (widget);
}

void MainWindow::startRestore (KConfig *config, const QString &group)
{
  // first save this stuff
  m_restoreConfig = config;
  m_restoreGroup = group;

  if (!m_restoreConfig || !m_restoreConfig->hasGroup (m_restoreGroup))
  {
	  //BEGIN Added stuff specifically for ktechlab
	  QList<int> hs;
	  hs << 220 << 100 << 230;
	  QList<int> vs;
	  vs << 0 << 100 << 150;

	  m_sidebars[0]->setLastSize (hs[0]);
	  m_sidebars[1]->setLastSize (hs[2]);
	  m_sidebars[2]->setLastSize (vs[0]);
	  m_sidebars[3]->setLastSize (vs[2]);

	  m_hSplitter->setSizes(hs);
	  m_vSplitter->setSizes(vs);
	  //END Added stuff specifically for ktechlab
	  
    return;
  }

  // apply size once, to get sizes ready ;)
  KConfigGroup grRestWnd = m_restoreConfig->group (m_restoreGroup);
  restoreWindowSize (grRestWnd);

  //m_restoreConfig->group (m_restoreGroup);

  // get main splitter sizes ;)
  QList<int> hs = grRestWnd.readEntry ("Kate-MDI-H-Splitter", IntList());
  QList<int> vs = grRestWnd.readEntry ("Kate-MDI-V-Splitter", IntList());

  m_sidebars[0]->setLastSize (hs[0]);
  m_sidebars[1]->setLastSize (hs[2]);
  m_sidebars[2]->setLastSize (vs[0]);
  m_sidebars[3]->setLastSize (vs[2]);

  m_hSplitter->setSizes(hs);
  m_vSplitter->setSizes(vs);

  setToolViewStyle( (KMultiTabBar::KMultiTabBarStyle)grRestWnd.readEntry ("Kate-MDI-Sidebar-Style", (int)toolViewStyle()) );
}

void MainWindow::finishRestore ()
{
  if (!m_restoreConfig)
    return;

  if (m_restoreConfig->hasGroup (m_restoreGroup))
  {
    // apply all settings, like toolbar pos and more ;)
    applyMainWindowSettings(m_restoreConfig->group(m_restoreGroup));

    // reshuffle toolviews only if needed
    KConfigGroup grRest = m_restoreConfig->group (m_restoreGroup);
    for ( unsigned int i=0; i < m_toolviews.size(); ++i )
    {
		KMultiTabBar::KMultiTabBarPosition newPos = (KMultiTabBar::KMultiTabBarPosition)grRest.readEntry(
            QString ("Kate-MDI-ToolView-%1-Position").arg(m_toolviews[i]->id), (int) m_toolviews[i]->sidebar()->sidebarPosition());

		if (m_toolviews[i]->sidebar()->sidebarPosition() != newPos)
      {
        moveToolView (m_toolviews[i], newPos);
      }
    }

    // restore the sidebars
    for (unsigned int i=0; i < 4; ++i)
      m_sidebars[i]->restoreSession (&grRest);
  }

  // clear this stuff, we are done ;)
  m_restoreConfig = 0;
  m_restoreGroup = "";
}

void MainWindow::saveSession (KConfigGroup *grConf)
{
  if (!grConf)
    return;

  saveMainWindowSettings (*grConf);

  // save main splitter sizes ;)
  QList<int> hs = m_hSplitter->sizes();
  QList<int> vs = m_vSplitter->sizes();

  if (hs[0] <= 2 && !m_sidebars[0]->splitterVisible ())
    hs[0] = m_sidebars[0]->lastSize();
  if (hs[2] <= 2 && !m_sidebars[1]->splitterVisible ())
    hs[2] = m_sidebars[1]->lastSize();
  if (vs[0] <= 2 && !m_sidebars[2]->splitterVisible ())
    vs[0] = m_sidebars[2]->lastSize();
  if (vs[2] <= 2 && !m_sidebars[3]->splitterVisible ())
    vs[2] = m_sidebars[3]->lastSize();

  grConf->writeEntry ("Kate-MDI-H-Splitter", hs);
  grConf->writeEntry ("Kate-MDI-V-Splitter", vs);

  // save sidebar style
  grConf->writeEntry ("Kate-MDI-Sidebar-Style", (int)toolViewStyle());

  // save the sidebars
  for (unsigned int i=0; i < 4; ++i)
    m_sidebars[i]->saveSession (grConf);
}

void KateMDI::MainWindow::updateSidebarMinimumSizes( )
{
// 	for (unsigned int i=0; i < 4; ++i)
// 		m_sidebars[i]->updateMinimumSize();
	m_sidebars[KMultiTabBar::Right]->updateMinimumSize();
}

//END MAIN WINDOW

}

#include "katemdi.moc"

