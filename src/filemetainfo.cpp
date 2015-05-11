/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "filemetainfo.h"
#include "textdocument.h"
#include "textview.h"

#include <kconfig.h>


//BEGIN class MetaInfo
MetaInfo::MetaInfo()
{
	m_cursorLine = 0;
	m_cursorColumn = 0;
}


bool MetaInfo::hasDefaultData() const
{
	return bookmarks().isEmpty() &&
			breakpoints().isEmpty()  &&
			(m_outputMethodInfo.method() == OutputMethodInfo::Method::Direct ) &&
			(m_cursorLine == 0) &&
			(m_cursorColumn == 0);
}


void MetaInfo::save( KConfigGroup* conf )
{
	conf->writeEntry( "Bookmarks", bookmarks() );
	conf->writeEntry( "Breakpoints", breakpoints() );
	conf->writeEntry( "OutputMethod", toID(outputMethodInfo().method()) );
	conf->writePathEntry( "OutputPath", outputMethodInfo().outputFile().prettyUrl() );
	conf->writeEntry( "OutputPicID", outputMethodInfo().picID() );
	conf->writeEntry( "CursorLine", cursorLine() );
	conf->writeEntry( "CursorColumn", cursorColumn() );
}


void MetaInfo::load( KConfigGroup* conf )
{
	setBookmarks( conf->readEntry("Bookmarks", IntList()) );
	setBreakpoints( conf->readEntry("Breakpoints", IntList()) );
	m_outputMethodInfo.setMethod( toMethod( conf->readEntry("OutputMethod") ) );
	m_outputMethodInfo.setOutputFile( conf->readPathEntry("OutputPath", QString()) );
	m_outputMethodInfo.setPicID( conf->readEntry("OutputPicID") );
	setCursorLine( conf->readEntry( "CursorLine", 0 ) );
	setCursorColumn( conf->readEntry( "CursorColumn", 0 ) );
}


OutputMethodInfo::Method::Type MetaInfo::toMethod( const QString & id )
{
	if ( id == "SaveAndLoad" )
		return OutputMethodInfo::Method::SaveAndLoad;
	
	else if ( id == "SaveAndForget" )
		return OutputMethodInfo::Method::SaveAndForget;
	
	return OutputMethodInfo::Method::Direct;
}


QString MetaInfo::toID( OutputMethodInfo::Method::Type method )
{
	switch (method)
	{
		case OutputMethodInfo::Method::SaveAndLoad:
			return "SaveAndLoad";
			
		case OutputMethodInfo::Method::SaveAndForget:
			return "SaveAndForget";
			
		case OutputMethodInfo::Method::Direct:
		default:
			return "Direct";
	}
}
//END class MetaInfo


//BEGIN class FileMetaInfo
FileMetaInfo::FileMetaInfo()
	: QObject()
{
	//m_metaInfoConfig = new KConfig( "metainfo", false, false, "appdata" );
    m_metaInfoConfig = new KConfig( "metainfo", "appdata" );
	loadAllMetaInfo();
}


FileMetaInfo::~FileMetaInfo()
{
	saveAllMetaInfo();
	delete m_metaInfoConfig;
}


void FileMetaInfo::grabMetaInfo( const KUrl & url, TextDocument * textDocument )
{
	if (!textDocument)
		return;
	
	m_metaInfoMap[url].setBookmarks( textDocument->bookmarkList() );
	m_metaInfoMap[url].setBreakpoints( textDocument->breakpointList() );
}


void FileMetaInfo::initializeFromMetaInfo( const KUrl & url, TextDocument * textDocument )
{
	if (!textDocument)
		return;
	
	textDocument->setBookmarks(m_metaInfoMap[url].bookmarks());
	textDocument->setBreakpoints(m_metaInfoMap[url].breakpoints());
}


void FileMetaInfo::grabMetaInfo( const KUrl & url, TextView * textView )
{
	if (!textView)
		return;
	
	m_metaInfoMap[url].setCursorLine( textView->currentLine() );
	m_metaInfoMap[url].setCursorColumn( textView->currentColumn() );
}


void FileMetaInfo::initializeFromMetaInfo( const KUrl & url, TextView * textView )
{
	if (!textView)
		return;
	
	textView->setCursorPosition( m_metaInfoMap[url].cursorLine(), m_metaInfoMap[url].cursorColumn() );
}


void FileMetaInfo::grabMetaInfo( const KUrl & url, OutputMethodDlg * dlg )
{
	if (!dlg)
		return;
	
	m_metaInfoMap[url].setOutputMethodInfo( dlg->info() );
}


void FileMetaInfo::initializeFromMetaInfo( const KUrl & url, OutputMethodDlg * dlg )
{
	if ( !dlg || url.isEmpty() || !m_metaInfoMap.contains(url) )
		return;
	
	OutputMethodInfo::Method::Type method = m_metaInfoMap[url].outputMethodInfo().method();
	dlg->setMethod(method);
	
	if ( method != OutputMethodInfo::Method::Direct )
		dlg->setOutputFile( m_metaInfoMap[url].outputMethodInfo().outputFile() );
	
	dlg->setPicID( m_metaInfoMap[url].outputMethodInfo().picID() );
}


void FileMetaInfo::saveAllMetaInfo()
{
	const MetaInfoMap::iterator end = m_metaInfoMap.end();
	for ( MetaInfoMap::iterator it = m_metaInfoMap.begin(); it != end; ++it )
	{
		if ( it.data().hasDefaultData() )
			m_metaInfoConfig->deleteGroup(it.key().prettyUrl());
		
		else
		{
			KConfigGroup grUrl = m_metaInfoConfig->group( it.key().prettyUrl() );
			it.data().save( &grUrl );
		}
	}
}


void FileMetaInfo::loadAllMetaInfo()
{
	QStringList urlList = m_metaInfoConfig->groupList();
	const QStringList::iterator end = urlList.end();
	for ( QStringList::iterator it = urlList.begin(); it != end; ++it )
	{
		KConfigGroup grUrl = m_metaInfoConfig->group(*it);
		m_metaInfoMap[*it].load(&grUrl);
	}
}
//END class FileMetaInfo

#include "filemetainfo.moc"
