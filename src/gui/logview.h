/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef LOGVIEW_H
#define LOGVIEW_H

class KTechlab;
// class Q3PopupMenu;

#include <ktextedit.h>
#include <qmap.h>

namespace KateMDI { class ToolView; }

class MessageInfo
{
	public:
		MessageInfo();
		MessageInfo( QString fileURL, int fileLine );
		
		QString fileURL() const { return m_fileURL; }
		int fileLine() const { return m_fileLine; }
			
	protected:
		QString m_fileURL;
		int m_fileLine;
};
typedef QMap<int,MessageInfo> MessageInfoMap;


/**
Base class for logviews (eg GpasmInterface) which output information, warnings, errors to a viewable log
@short Dockable logview
@author David Saxton
*/
class LogView : public KTextEdit
{
	Q_OBJECT
	public:
		LogView( KateMDI::ToolView * parent, const char *name = 0 );
		~LogView() override;
	
		enum OutputType
		{
			ot_important,	// Bold
			ot_info,		// Italic
			ot_message,		// Plain
			ot_warning,		// Grey
			ot_error		// Red
		};
		
	signals:
		/**
		 * Emitted when the user clicks on a paragraph in the log view
		 */
		void paraClicked( const QString &text, MessageInfo messageInfo );
	
	public slots:
		virtual void clear();
		void addOutput( QString text, OutputType outputType, MessageInfo messageInfo = MessageInfo() );
	
	protected:
		virtual QMenu * createPopupMenu( const QPoint & pos );
		/**
		 * Replaces "&" with &amp;, "<" with &lt;, etc
		 */
		void tidyText( QString &t );
		/**
		 * Replaces "&lt;" with "<", "&amp;" with "&", etc
		 */
		void untidyText( QString &t );
	
		MessageInfoMap m_messageInfoMap;

        void mouseDoubleClickEvent ( QMouseEvent * e ) override;
	
	private slots:
		void slotParaClicked( int para, int pos );
};

#endif
