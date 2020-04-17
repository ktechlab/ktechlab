/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RECENTFILESACTION_H
#define RECENTFILESACTION_H

//#include <kactionclasses.h>
#include <kselectaction.h>

//class K3PopupMenu;
class QUrl;
class QMenu;

/**
Taken mainly from kdelibs/kdeui/kactionclasses.[cpp/h], author Michael Koch.
Adapted to work around strange bug occuring.
*/
class RecentFilesAction : public KSelectAction
{
	Q_OBJECT
	public:
		RecentFilesAction( const QString & configGroupName, const QString & text, const QObject * receiver, const char* slot, QObject* parent, const char * name );
		
		~RecentFilesAction() override;
		
		/**
		 *  Loads the recent files entries from a given KConfig object.
		 *  You can provide the name of the group used to load the entries.
		 *  If the groupname is empty, entries are load from a group called 'RecentFiles'
		 *
		 *  This method does not effect the active group of KConfig.
		 */
		void loadEntries();
		/**
		 *  Saves the current recent files entries to a given KConfig object.
		 *  You can provide the name of the group used to load the entries.
		 *  If the groupname is empty, entries are saved to a group called 'RecentFiles'
		 *
		 *  This method does not effect the active group of KConfig.
		 */
		void saveEntries();
		/**
		 *  Add URL to recent files list.
		 *
		 *  @param url The URL of the file
		 */
		void addUrl( const QUrl& url );
		
	signals:
		/**
		 *  This signal gets emited when the user selects an URL.
		 *
		 *  @param url The URL thats the user selected.
		 */
		void urlSelected( const QUrl& url );
		
	protected slots:
		void itemSelected( const QString& string );
		void menuAboutToShow();
		void menuItemActivated( QAction *action );
		void slotClicked();
		virtual void slotActivated(int);
		virtual void slotActivated(const QString& );
		virtual void slotActivated();

	protected:
		unsigned m_maxItems;
		QMenu * m_popup;
		QString m_configGroupName;
};

#endif
