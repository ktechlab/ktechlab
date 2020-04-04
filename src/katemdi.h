/* This file is part of the KDE libraries
   Copyright (C) 2005 Christoph Cullmann <cullmann@kde.org>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

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

#ifndef __KATE_MDI_H__
#define __KATE_MDI_H__

#include <kparts/mainwindow.h>

#include <kmultitabbar.h>

#include <KDELibs4Support/kshortcut.h>

// #include <q3dict.h>
// #include <q3intdict.h>
// #include <q3vbox.h>
#include <qsplitter.h>
#include <QMap>

class QAction;

namespace KateMDI {

class MainWindow;
class ToolView;

/** This class is needed because QSplitter cant return an index for a widget. */
class Splitter : public QSplitter
{
  Q_OBJECT

  public:
    Splitter(Qt::Orientation o, QWidget* parent = nullptr, const char* name = nullptr);
    ~Splitter() override;

    /** Since there is supposed to be only 2 childs of a katesplitter,
     * any child other than the last is the first.
     * This method uses QSplitter::idAfter(widget) which
     * returns 0 if there is no widget after this one.
     * This results in an error if widget is not a child
     * in this splitter */
    //bool isLastChild(QWidget* w) const;

    //int idAfter ( QWidget * w ) const;
};

class GUIClient : public QObject, public KXMLGUIClient
{
  Q_OBJECT

  public:
    GUIClient ( MainWindow *mw );
    ~GUIClient() override;

    void registerToolView (ToolView *tv);

  private slots:
    void clientAdded( KXMLGUIClient *client );
    void updateActions();

  private:
    MainWindow *m_mw;
};

class ToolView : public QWidget
{
  Q_OBJECT

  friend class Sidebar;
  friend class MainWindow;
  friend class GUIClient;
  friend class ToggleToolViewAction;

  protected:
    /**
     * ToolView
     * Objects of this clas represent a toolview in the mainwindow
     * you should only add one widget as child to this toolview, it will
     * be automatically set to be the focus proxy of the toolview
     * @param mainwin main window for this toolview
     * @param sidebar sidebar of this toolview
     * @param parent parent widget, e.g. the splitter of one of the sidebars
     */
    ToolView (MainWindow *mainwin, class Sidebar *sidebar, QWidget *parent);

  public:
    /**
     * destuct me, this is allowed for all, will care itself that the toolview is removed
     * from the mainwindow and sidebar
     */
    ~ToolView () override;

  signals:
    /**
     * toolview hidden or shown
     * @param visible is this toolview made visible?
     */
    void visibleChanged (bool visible);

  /**
   * some internal methodes needed by the main window and the sidebars
   */
  protected:
    MainWindow *mainWindow () { return m_mainWin; }

    Sidebar *sidebar () { return m_sidebar; }

    void setVisibleToolView (bool vis);

  public:
    bool visible () const;

  protected:
    void childEvent ( QChildEvent *ev ) override;

  private:
    MainWindow *m_mainWin;
    Sidebar *m_sidebar;

    /**
     * unique id
     */
    QString id;

    /**
     * is visible in sidebar
     */
    bool m_visible;

    /**
     * is this view persistent?
     */
    bool persistent;

    QPixmap icon;
    QString text;
};

class Sidebar : public KMultiTabBar
{
  Q_OBJECT

  public:
    Sidebar (KMultiTabBar::KMultiTabBarPosition pos, MainWindow *mainwin, QWidget *parent);
    ~Sidebar () override;

    void setSplitter (Splitter *sp);

	//HACK use these functions intead of their respective functions in
	//KMultiTabBar so that we know what they were set to.
	void setSidebarPosition( KMultiTabBarPosition pos );
	KMultiTabBar::KMultiTabBarPosition sidebarPosition() const { return m_pos; }
	void setSidebarStyle( KMultiTabBarStyle style );
	KMultiTabBar::KMultiTabBarStyle sidebarTabStyle() const { return m_sidebarTabStyle; }

  public:
    ToolView *addWidget (const QPixmap &icon, const QString &text, ToolView *widget);
    bool removeWidget (ToolView *widget);

    bool showWidget (ToolView *widget);
    bool hideWidget (ToolView *widget);

    void setLastSize (int s) { m_lastSize = s; }
    int lastSize () const { return m_lastSize; }
    void updateLastSize ();

    bool splitterVisible () const { return m_ownSplit->isVisible(); }

	void restoreSession ();
	void updateMinimumSize();

     /**
     * restore the current session config from given object, use current group
     * @param config config object to use
     */
    void restoreSession (KConfigGroup *config);

     /**
     * save the current session config to given object, use current group
     * @param config config object to use
     */
    void saveSession (KConfigGroup* config);

  private slots:
    void tabClicked(int);

  protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;

  private slots:
    void buttonPopupActivate (QAction* action);

  private:

    MainWindow *m_mainWin;

	KMultiTabBar::KMultiTabBarStyle m_sidebarTabStyle;
    KMultiTabBar::KMultiTabBarPosition m_pos;
    Splitter *m_splitter;
    KMultiTabBar *m_tabBar;
    Splitter *m_ownSplit;

    //Q3IntDict<ToolView> m_idToWidget;
    QMap<int, ToolView*> m_idToWidget;
    QMap<ToolView*, int> m_widgetToId;

    /**
     * list of all toolviews around in this sidebar
     */
    QList<ToolView*> m_toolviews;

    int m_lastSize;

    int m_popupButton;
};

class MainWindow : public KParts::MainWindow
{
  Q_OBJECT

  friend class ToolView;

  //
  // Constructor area
  //
  public:
    /**
     * Constructor
     */
    MainWindow (QWidget* parentWidget = nullptr, const char* name = nullptr);

    /**
     * Destructor
     */
    ~MainWindow () override;

  //
  // public interfaces
  //
  public:
    /**
     * central widget ;)
     * use this as parent for your content
     * this widget will get focus if a toolview is hidden
     * @return central widget
     */
    QWidget *centralWidget () const;

    /**
     * add a given widget to the given sidebar if possible, name is very important
     * @param identifier unique identifier for this toolview
     * @param pos position for the toolview, if we are in session restore, this is only a preference
     * @param icon icon to use for the toolview
     * @param text text to use in addition to icon
     * @return created toolview on success or 0
     */
    ToolView *createToolView (const QString &identifier, KMultiTabBar::KMultiTabBarPosition pos, const QPixmap &icon, const QString &text);

    /**
     * give you handle to toolview for the given name, 0 if no toolview around
     * @param identifier toolview name
     * @return toolview if existing, else 0
     */
    ToolView *toolView (const QString &identifier) const;

    /**
     * set the toolview's tabbar style.
     * @param style the tabbar style.
     */
    void setToolViewStyle (KMultiTabBar::KMultiTabBarStyle style);

    /**
     * get the toolview's tabbar style. Call this before @p startRestore(),
     * otherwise you overwrite the usersettings.
     * @return toolview's tabbar style
     */
    KMultiTabBar::KMultiTabBarStyle toolViewStyle () const;

  protected:
    /**
     * called by toolview destructor
     * @param widget toolview which is destroyed
     */
    void toolViewDeleted (ToolView *widget);

  /**
   * modifiers for existing toolviews
   */
  public:
    /**
     * move a toolview around
     * @param widget toolview to move
     * @param pos position to move too, during session restore, only preference
     * @return success
     */
    bool moveToolView (ToolView *widget, KMultiTabBar::KMultiTabBarPosition pos);

    /**
     * show given toolview, discarded while session restore
     * @param widget toolview to show
     * @return success
     */
    bool showToolView (ToolView *widget);

    /**
     * hide given toolview, discarded while session restore
     * @param widget toolview to hide
     * @return success
     */
    bool hideToolView (ToolView *widget);

  /**
   * session saving and restore stuff
   */
  public:
    /**
     * start the restore
     * @param config config object to use
     * @param group config group to use
     */
    void startRestore (KConfig *config, const QString &group);

    /**
     * finish the restore
     */
    void finishRestore ();

    /**
     * save the current session config to given object and group
     * @param config config object to use
     */
    void saveSession (KConfigGroup *config);

	void updateSidebarMinimumSizes();

  /**
   * internal data ;)
   */
  private:
    /**
     * map identifiers to widgets
     */
    //Q3Dict<ToolView> m_idToWidget;
    QMap<QString, ToolView*> m_idToWidget;

    /**
     * list of all toolviews around
     */
    QList<ToolView*> m_toolviews;

    /**
     * widget, which is the central part of the
     * main window ;)
     */
    QWidget *m_centralWidget;

    /**
     * horizontal splitter
     */
    Splitter *m_hSplitter;

    /**
     * vertical splitter
     */
    Splitter *m_vSplitter;

    /**
     * sidebars for the four sides
     */
    Sidebar *m_sidebars[4];

    /**
     * config object for session restore, only valid between
     * start and finish restore calls
     */
    KConfig *m_restoreConfig;

    /**
     * restore group
     */
    QString m_restoreGroup;

    /**
     * out guiclient
     */
    GUIClient *m_guiClient;
};

}

#endif
