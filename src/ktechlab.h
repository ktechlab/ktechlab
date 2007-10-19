/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KTECHLAB_H
#define KTECHLAB_H

#include <katemdi.h>

class CircuitDocument;
class TextDocument;
class ComponentSelector;
class Document;
class FlowCodeDocument;
class ItemEditor;
class LanguageManager;
class LogView;
class MechanicsDocument;
class ProjectManager;
class View;
class ViewContainer;

typedef QValueList< QGuardedPtr<ViewContainer> > ViewContainerList;

class KAction;
class RecentFilesAction;
class KTabWidget;
class KToolBar;
class KToggleAction;
class KURL;
class QLabel;

/**
 * This class serves as the main window for KTechlab.  It handles the
 * menus, toolbars, status bars, loading/saving files, config, etc.
 *
 * @short Main window class
 * @author David Saxton
 */
class KTechlab : public KateMDI::MainWindow
{
	Q_OBJECT
	public:
		KTechlab();
		~KTechlab();
	
		/**
		 * Returns a pointer to an action with the given name.
		 */
		KAction * action( const QString & name) const;
		/**
		 * Returns a URL from a Open File dialog (with all ktechlab related file
		 * types allowed).
		 */
		static KURL::List getFileURLs();
		/**
		 * Returns a list of the recently opened/saved files
		 */
		QStringList recentFiles();
		/**
		 * The tab and window captions will get updated when we have re-entered
		 * the Qt event loop.
		 */
		void requestUpdateCaptions();
		/**
		 * Returns the tabwidget that shows the list of view containers.
		 */
		KTabWidget * tabWidget() const { return m_pViewContainerTabWidget; }
		/**
		 * Registers the viewcontainer with the internal list.
		 */
		void addWindow( ViewContainer * vc);
		/**
		 * Removes all gui clients added to the factory other than ourself.
		 */
		void removeGUIClients();
		/**
		 * Work around a crash. Adds the given KXMLGUIClient to a list of those
		 * that ktechlab will not attempt to remove on calling removeGUIClients.
		 */
		void addNoRemoveGUIClient( KXMLGUIClient * client);
		/**
		 * For preventing flickering when we are updating the toolbars - grab
		 * a pixmap of the current toolbars, and overlay it in position.
		 */
		void overlayToolBarScreenshot();
		
		virtual void show();
		
	signals:
		/**
		 * Emitted when the user changes the configuration settings in the config dialog
		 */
		void configurationChanged();
		/**
		 * Emitted when a recent file is added
		 */
		void recentFileAdded( const KURL &url);
		/**
		 * Emitted when ViewContainers should update their captions.
		 */
		void needUpdateCaptions();
	
	public slots:
		/**
		 * The user right clicked on a tab item.
		 */
		void slotTabContext( QWidget* widget,const QPoint & pos);
		void slotDragContextActivated( int id);
		/**
		 * The user clicked on an item in the tab-menu (from right clicking).
		 */
		void slotTabContextActivated( int id);
		void slotChangeStatusbar(const QString& text);
		void load(const KURL& url);
		void slotUpdateConfiguration();
		/**
		 * Adds a url to the list of recently opened files
		 */
		void addRecentFile( const KURL &url);
		/**
		 * A document had its modified state changed; will update actions, 
		 * tab titles, etc as appropriate.
		 */
		void slotDocModifiedChanged();
		/**
		 * A document had its undo/redo state changed; will update actions, 
		 * tab titles, etc as appropriate.
		 */
		void slotDocUndoRedoChanged();

	protected:
		/**
		 * Overridden virtuals for Qt drag 'n drop (XDND)
		 */
		virtual void dragEnterEvent(QDragEnterEvent *event);
		virtual void dropEvent(QDropEvent *event);
		/**
		 * This function is called when it is time for the app to save its
		 * properties for session management purposes.
		 */
		void saveProperties(KConfig *);
		/**
		 * This function is called when this app is restored.  The KConfig
		 * object points to the session management config file that was saved
		 * with @ref saveProperties
		 */
		void readProperties(KConfig *);
		/**
		 * Called before the window is closed, either by the user or indirectly by the session manager.
		 * This function doesn't actually close the main window; it only queries the user and closes the active view.
		 * To quit the appliaction completly, you should use KTechlab::slotFileQuit()
		 */
		virtual bool queryClose();
	
	protected slots:
		void slotViewContainerActivated( QWidget * viewContainer);
		void slotTabDragEvent( const QDragMoveEvent *e, bool &accept);
		void slotTabDragInitiate( QWidget *widget);
		void slotTabReceivedDropEvent( QDropEvent *e);
		void slotTabReceivedDropEvent( QWidget *widget, QDropEvent *e);
		void slotUpdateTabWidget();
		/**
		 * Updates the tab and window captions from what is currently open and
		 * focused.
		 */
		void slotUpdateCaptions();

	private slots:
		/**
		 * Called from a QTimer timeout (which should be after the toolbars have
		 * finished constructing themselves).
		 */
		void hideToolBarOverlay();
		void slotViewContainerDestroyed( QObject * obj);
	
		void slotFileNewAssembly();
		void slotFileNewMicrobe();
		void slotFileNewC();
		void slotFileNewCircuit();
		void slotFileNewFlowCode();
		void slotFileNewMechanics();
		void slotFileNew();
		void slotFileOpen();
		void slotFileSave();
		void slotFileSaveAs();
		void slotFilePrint();
		void slotFileQuit();
	
		// Editing operations
		void slotEditUndo();
		void slotEditRedo();
		void slotEditCut();
		void slotEditCopy();
		void slotEditPaste();
	
		/**
		 * Split the current view into two
		 */
		void slotViewSplitLeftRight();
		/**
		 * Split the current view into two
		 */
		void slotViewSplitTopBottom();
		void slotViewContainerClose();
		void slotViewClose();
	
		void slotOptionsShowStatusbar();
		void slotOptionsConfigureKeys();
		void slotOptionsConfigureToolbars();
		void slotOptionsPreferences();

	private:
		void setupActions();
		void setupToolDocks();
		void setupView();
		void setupTabWidget();
	
		RecentFilesAction * m_recentFiles;
		RecentFilesAction * m_recentProjects;
		KToggleAction * m_statusbarAction;
		KTabWidget * m_pViewContainerTabWidget;
		QString m_lastStatusBarMessage;
		QValueList<KXMLGUIClient*> m_noRemoveGUIClients;
		QLabel * m_pToolBarOverlayLabel;
		bool m_bIsShown; // Set true when show() is called
		ViewContainerList m_viewContainerList;
		QTimer * m_pUpdateCaptionsTimer;
	
		QGuardedPtr<ViewContainer> m_pContextMenuContainer;
		QGuardedPtr<ViewContainer> m_pFocusedContainer;
		QGuardedPtr<ViewContainer> m_pContainerDropSource;
		QGuardedPtr<ViewContainer> m_pContainerDropReceived;
};

#endif // KTECHLAB_H

