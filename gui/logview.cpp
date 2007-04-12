/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "logview.h"

#include <kdebug.h>
#include <kiconloader.h>
#include <katemdi.h>
#include <klocale.h>
#include <qpopupmenu.h>


//BEGIN class LogView
LogView::LogView( KateMDI::ToolView * parent, const char *name )
	: KTextEdit( parent, name )
{
	setReadOnly(true);
	setPaper( Qt::white );
	setTextFormat( LogText );
	setWordWrap( WidgetWidth );
	
	// Connect up signal emitted when the user doubleclicks on a paragraph in the log view
	connect( this, SIGNAL(clicked(int,int)), this, SLOT(slotParaClicked(int,int)) );
}


LogView::~LogView()
{
}


void LogView::clear()
{
	m_messageInfoMap.clear();
	KTextEdit::clear();
}


void LogView::addOutput( QString text, OutputType outputType, MessageInfo messageInfo )
{
	tidyText(text);
	switch(outputType)
	{
		case LogView::ot_important:
			append( QString("<font color=\"#000000\"><b>%1</b></font>").arg(text) );
			break;
			
		case LogView::ot_info:
			append( QString("<font color=\"#000000\"><i>%1</i></font>").arg(text) );
			break;
			
		case LogView::ot_message:
			append( QString("<font color=\"#000000\">%1</font>").arg(text) );
			break;
			
		case LogView::ot_warning:
			append( QString("<font color=\"#666666\">%1</font>").arg(text) );
			break;
			
		case LogView::ot_error:
			append( QString("<font color=\"#800000\">%1</font>").arg(text) );
			break;
	}
	
	m_messageInfoMap[ paragraphs()-1 ] = messageInfo;
}


void LogView::slotParaClicked( int para, int /*pos*/ )
{
	QString t = text(para);
	untidyText(t);
	emit paraClicked( t, m_messageInfoMap[para] );
}


void LogView::tidyText( QString &t )
{
	t.replace( "&", "&amp;" );
	t.replace( "<", "&lt;" );
	t.replace( ">", "&gt;" );
}


void LogView::untidyText( QString &t )
{
	t.replace( "&lt;", "<" );
	t.replace( "&gt;", ">" );
	t.replace( "&amp;", "&" );
}


QPopupMenu * LogView::createPopupMenu( const QPoint & pos )
{
	QPopupMenu * menu = KTextEdit::createPopupMenu( pos );
	
	menu->insertSeparator();
	int id = menu->insertItem( i18n("Clear All"), this, SLOT(clear()) );
	
	// "an empty textedit is always considered to have one paragraph" - qt documentation
	// although this does not always seem to be the case, so I don't know...
	menu->setItemEnabled( id, paragraphs() > 1 );
	
	return menu;
}
//END class LogView



//BEGIN class MessageInfo
MessageInfo::MessageInfo()
{
	m_fileLine = -1;
}


MessageInfo::MessageInfo( QString fileURL, int fileLine )
{
	m_fileURL = fileURL;
	m_fileLine = fileLine;
}
//END class MessageInfo


#include "logview.moc"
