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

#include <QUrl>
#include <QMap>
#include <QList>
#include <QPointer>

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
class ViewArea;
class ViewContainer;

typedef QMap< int, QString > IntStringMap;
typedef QList< QPointer<ViewContainer> > ViewContainerList;

class RecentFilesAction;
class QTabWidget;
class KToggleAction;
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

    friend class KtlTestsAppFixture;

	public:
		KTechlab();
		~KTechlab() override;
		
		/**
		 * @return pointer to the main KTechlab object. This is set to null when
		 * KTechlab is about to be deleted.
		 */
		static KTechlab * self() { return m_pSelf; }
		/**
		 * The standard item font in use (set to a maximum of 12 pixels).
		 */
		QFont itemFont() const { return m_itemFont; }
		/**
		 * Returns a pointer to an action with the given name.
		 */
		QAction* actionByName( const QString& name ) const;
		/**
		 * Returns a URL from a Open File dialog (with all ktechlab related file
		 * types allowed).
		 * @param allowMultiple Whether to allow the user to select more than
		 * one URL.
		 */
		static QList<QUrl> getFileURLs( bool allowMultiple = true );
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
		QTabWidget * tabWidget() const { return m_pViewContainerTabWidget; }
		/**
		 * Registers the viewcontainer with the internal list.
		 */
		void addWindow( ViewContainer * vc );
		/**
		 * Removes all gui clients added to the factory other than ourself.
		 */
		void removeGUIClients();
		/**
		 * Work around a crash. Adds the given KXMLGUIClient to a list of those
		 * that ktechlab will not attempt to remove on calling removeGUIClients.
		 */
		void addNoRemoveGUIClient( KXMLGUIClient * client );
		/**
		 * For preventing flickering when we are updating the toolbars - grab
		 * a pixmap of the current toolbars, and overlay it in position.
		 */
		void overlayToolBarScreenshot();
		/**
		 * Opens a file in the given ViewArea.
		 */
		void openFile( ViewArea * viewArea );

		virtual void show();

	signals:
		/**
		 * Emitted when the user changes the configuration settings in the config dialog
		 */
		void configurationChanged();
		/**
		 * Emitted when a recent file is added
		 */
		void recentFileAdded( const QUrl &url );
		/**
		 * Emitted when ViewContainers should update their captions.
		 */
		void needUpdateCaptions();

	public slots:
		/**
		 * The user right clicked on a tab item.
		 */
		void slotTabContext(const QPoint & pos);
		/**
		 * The user clicked on an item in the tab-menu (from right clicking).
		 */
		void slotTabContextActivated( QAction * action );
		void slotChangeStatusbar(const QString& text);
		/**
		 * Open the document at the given url. If viewArea is non-null, then the
		 * new view will be put into viewArea.
		 */
		void load( const QUrl & url, ViewArea * viewArea = nullptr );
		void slotUpdateConfiguration();
		/**
		 * Adds a url to the list of recently opened files
		 */
		void addRecentFile( const QUrl &url );
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

	protected:
		/**
		 * Overridden virtuals for Qt drag 'n drop (XDND)
		 */
		void dragEnterEvent(QDragEnterEvent *event) override;
		void dropEvent(QDropEvent *event) override;
		/**
		 * This function is called when it is time for the app to save its
		 * properties for session management purposes.
		 */
		void savePropertiesInConfig(KConfig *);
		/**
		 * This function is called when this app is restored.  The KConfig
		 * object points to the session management config file that was saved
		 * with @ref saveProperties
		 */
		void readPropertiesInConfig(KConfig *);
		/**
		 * Called before the window is closed, either by the user or indirectly by the session manager.
		 * This function doesn't actually close the main window; it only queries the user and closes the active view.
		 * To quit the appliaction completly, you should use KTechlab::slotFileQuit()
		 */
		bool queryClose() override;
	
	protected slots:
		void slotViewContainerActivated( int index );
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
		/**
		 * Called when the user clicks on an example (circuit, etc) from the
		 * help menu/
		 */
		void openExample(QAction*);
		void slotViewContainerDestroyed( QObject * obj );

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
        void slotLoadRecent(const QUrl &url);

	private:
		void setupActions();
		void setupExampleActions();
		void setupToolDocks();
		void setupView();
		void setupTabWidget();
        void loadInternal(const QUrl &url, bool isRecent, ViewArea *viewArea = nullptr);

		RecentFilesAction * m_recentFiles;
		RecentFilesAction * m_recentProjects;
		KToggleAction * m_statusbarAction;
		QTabWidget * m_pViewContainerTabWidget;
		QString m_lastStatusBarMessage;
		QList<KXMLGUIClient*> m_noRemoveGUIClients;
		QLabel * m_pToolBarOverlayLabel;
		bool m_bIsShown; // Set true when show() is called
		ViewContainerList m_viewContainerList;
		QTimer * m_pUpdateCaptionsTimer;
		IntStringMap m_exampleFiles;
		QFont m_itemFont;

		static KTechlab * m_pSelf;

		QPointer<ViewContainer> m_pContextMenuContainer;
		QPointer<ViewContainer> m_pFocusedContainer;
		QPointer<ViewContainer> m_pContainerDropSource;
		QPointer<ViewContainer> m_pContainerDropReceived;
};

#endif // KTECHLAB_H

