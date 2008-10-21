/***************************************************************************
 *   Copyright (C) 2005 by David Saxton <david@bluehaze.org>               *
 *   Copyright (C) 2008 Julian Bäume <julian@svg4all.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VIEWCONTAINER_H
#define VIEWCONTAINER_H

#include <QDrag>
#include <QMap>
#include <QList>
#include <QSplitter>

class KTechlab;
class View;
class ViewArea;
class ViewContainer;

class KConfig;
class QHBoxLayout;
class QLayout;

typedef QMap< uint, ViewArea* > ViewAreaMap;
typedef QList<int> IntList;

/**
Before a ViewArea has been given a view, this is shown.
\author David Saxton
*/
class EmptyViewArea : public QWidget
{
    Q_OBJECT
    public:
        EmptyViewArea( ViewArea * parent );
        ~EmptyViewArea();

    protected slots:
        void openDocument();

    protected:
        ViewArea * m_pViewArea;
};


/**
Contains either exactly one View, or two ViewAreas, seperated by a QSplitter.
If it contains one view, then the value returned in id() is that of the view.
If it contains two ViewAreas, then the value returned by id() is -1.
@author David Saxton
*/
class ViewArea : public QSplitter
{
Q_OBJECT
public:
    enum Position
    {
        Right,
        Bottom
    };

    ViewArea( QWidget *parent, ViewContainer *viewContainer, int id, bool showOpenButton, const char * name = 0 );
    ~ViewArea();

    ViewContainer *viewContainer() const { return p_viewContainer; }
    int id() const { return m_id; }
    /**
    * Splits this ViewArea into two, and returns a pointer to the new ViewArea
    * @param showOpenButton Whether to present the user with the EmptyViewArea
    * widget (i.e. the new ViewArea is not destined to be immediately filled
    * with a view).
    */
    ViewArea *createViewArea( Position position, uint id, bool showOpenButton );
    /**
    * Adds the given View to the main part of the layout
    */
    void setView( View *view );
    /**
    * Saves the state of this ViewArea and any contained ViewAreas
    */
    void saveState( KConfig *config );
    /**
    * Restores the state of this ViewArea and any contained ViewAreas
    * @param groupName e.g. "ViewContainer 1"
    */
    void restoreState( KConfig *config, int id, const QString &groupName );
    /**
    * Returns true if this ViewArea can save useful information as to its state
    * (i.e. it's children can save useful information about their state, or has
    * a view with a url in it)
    */
    bool canSaveUsefulStateInfo() const;

    static QString fileKey( int id );
    static QString containsKey( int id );
    static QString orientationKey( int id );

    View * view() const { return p_view; }

protected slots:
    void viewAreaDestroyed( QObject *obj );
    void viewDestroyed();

protected:
    int m_id;
    EmptyViewArea * m_pEmptyViewArea;
    QPointer<View> p_view;
    ViewArea *p_viewArea1;
    ViewArea *p_viewArea2;
    ViewContainer *p_viewContainer;
};

/**
@author David Saxton
*/
class ViewContainer : public QWidget
{
Q_OBJECT
public:
    /**
    * Constructs a new ViewContainer, along with a default ViewArea ready for
    * parenting a View, with an id of 0. parent is only used if ktechlab is
    * null; otherwise the parent widget is ktechlab's tabWidget()
    */
    ViewContainer( const QString & caption, QWidget * parent = 0 );
    ~ViewContainer();

    /**
    * Returns the view in the ViewArea with the given id
    */
    View *view( uint id ) const;
    /**
    * Returns the ViewArea with the given id
    */
    ViewArea *viewArea( uint id ) const;
    /**
    * The active view area is the one that is focused.
    */
    void setActiveViewArea( uint id );
    /**
    * Returns the id of the active ViewArea
    */
    uint activeViewArea() const { return m_activeViewArea; }
    /**
    * Returns a pointer to the view of the active view area
    */
    View *activeView() const { return view( activeViewArea() ); }
    /**
    * Attempts to close the given viewarea, returning true if successful (i.e
    * if the user did not object to the close request )
    */
    bool closeViewArea( uint id );
    /**
    * Creates a view area (parent QWidget, splitter et al) ready for inclusion
    * of a view.
    * @param relativeViewArea the viewarea to position the new viewarea next to, if -1 then is taken to be the active view area
    * @param position Top, Right, Bottom or Left of given relativeViewArea
    * @returns id of the the view area, or -1 if unsucessful
    * @param showOpenButton Whether to present the user with the EmptyViewArea
    * widget (i.e. the new ViewArea is not destined to be immediately filled
    * with a view).
    */
    int createViewArea( int relativeViewArea, ViewArea::Position position, bool showOpenButton );
    /**
    * Attempts to close each view area, returning false if any fail to be
    * closed
    */
    bool closeViewContainer();
    /**
    * @returns number of views in this view container
    */
    uint viewCount() const { return m_viewAreaMap.size(); }
    /**
    * Sets the pointer to the view area with the given id
    */
    void setViewAreaId( ViewArea *viewArea, uint id );
    /**
    * Removes a ViewArea from internal lists
    */
    void setViewAreaRemoved( uint id );
    /**
    * Sets the id to be "used"
    */
    void setIdUsed( int id );
    /**
    * Writes the state of the View Container (layout of views and view URLs)
    * to the given KConfig. Doesn't change the group - so preset it if
    * needed!
    */
    void saveState( KConfig *config );
    /**
    * Reads in the saved config state (as written by saveState), and restores
    * the ViewContainer with all appropriate views open
    * @param groupName e.g. "ViewContainer 1"
    */
    void restoreState( KConfig *config, const QString &groupName );
    /**
    * Returns a unique id (negative) for a ViewArea that is now a Parent of other ViewAreas
    */
    int uniqueParentId();
    /**
    * Returns a unique id (positive) for a new ViewArea
    */
    int uniqueNewId();
    /**
    * Returns true if this ViewArea can save useful information as to its state
    * (i.e. it's children can save useful information about their state, or has
    * a view with a url in it)
    */
    bool canSaveUsefulStateInfo() const;

public slots:
    /**
    * Sets the tab caption et al from the contents of this ViewContainer
    * @deprecated use updateWindowTitle instead
    */
    void updateCaption();
    /**
    * Sets the tab windowTitle (former known as caption) et al from the contents of this ViewContainer
    */
    void updateWindowTitle();

protected slots:
    void baseViewAreaDestroyed( QObject *obj );

protected:
    void restoreViewArea( KConfig *config, int id, ViewArea *viewArea );
    void findActiveViewArea();
    int m_activeViewArea;
    ViewArea *m_baseViewArea;
    ViewAreaMap m_viewAreaMap;
    IntList m_usedIDs;
    bool b_deleted;
};

#endif
