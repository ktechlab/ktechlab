/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CONTEXTHELP_H
#define CONTEXTHELP_H

#include <qwidget.h>

class Item;
class ContextHelp;
class QLabel;
class QTextBrowser;
namespace KateMDI { class ToolView; }

/**
Sidebar that provides context-sensitive help for whatever the user is currently
helping (e.g. pinouts, command references, etc). Not to be confused with
ItemEditor, which which allows editing of data specific to the selected CNItem
in a ICNDocument.

@author David Saxton
*/
class ContextHelp : public QWidget
{
	Q_OBJECT
	public:
		static ContextHelp * self( KateMDI::ToolView * parent = 0 );
		static QString toolViewIdentifier() { return "ContextHelp"; }
		
		~ContextHelp();
		/**
		 * Replace special tags with appropriate html formatting code.
		 */
		void parseInfo( QString &info );
	
	public slots:
		void slotClear();
		void slotMultipleSelected();
		void slotUpdate( Item *item );
		void setContextHelp(const QString& name, const QString &help);
	
	private:
		ContextHelp( KateMDI::ToolView * parent );
	
		QLabel *m_nameLbl;
		QTextBrowser *m_info;
		static ContextHelp * m_pSelf;
};

#endif

