/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"
#ifndef NO_GPSIM

#include "debugmanager.h"
#include "gpsimprocessor.h"

#include <QStringList>


//BEGIN class DebugManager
DebugManager * DebugManager::m_pSelf = 0l;
// static KStaticDeleter<DebugManager> staticDebugManagerDeleter;

DebugManager * DebugManager::self()
{
	if (!m_pSelf)
		// staticDebugManagerDeleter.setObject( m_pSelf, new DebugManager );
        m_pSelf = new DebugManager;
	return m_pSelf;
}


DebugManager::DebugManager()
	: QObject()
{
}


DebugManager::~DebugManager()
{
}


void DebugManager::registerGpsim( GpsimProcessor * gpsim )
{
	if (!gpsim)
		return;
	
	m_processors << gpsim;
	
	const QStringList files = gpsim->sourceFileList();
	QStringList::const_iterator end = files.end();
    /*
	for ( QStringList::const_iterator it = files.begin(); it != end; ++it )
	{
		if ( TextDocument * doc = dynamic_cast<TextDocument*>(DocManager::self()->findDocument(*it)) )
		{
			if ( !doc->debuggerIsRunning() )
				doc->setDebugger( gpsim->currentDebugger(), false );
		}
	}
	*/
}

/*
void DebugManager::urlOpened( TextDocument * td )
{
	if ( td->debuggerIsRunning() )
		return;
	
	m_processors.remove( (GpsimProcessor*)0l );
	GpsimProcessorList::iterator end = m_processors.end();
	for ( GpsimProcessorList::iterator it = m_processors.begin(); it != end; ++it )
	{
		if ( !(*it)->sourceFileList().contains( td->url().path() ) )
			continue;
		
		(*it)->setDebugMode( (td->guessedCodeType() == TextDocument::ct_asm) ? GpsimDebugger::AsmDebugger : GpsimDebugger::HLLDebugger );
		
		td->setDebugger( (*it)->currentDebugger(), false );
		return;
	}
}
*/
//END class DebugManager


#include "debugmanager.moc"

#endif
