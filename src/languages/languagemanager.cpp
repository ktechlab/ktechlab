/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "docmanager.h"
#include "languagemanager.h"
#include "logview.h"
#include "ktechlab.h"
#include "ktempfile.h"
#include "src/core/ktlconfig.h"
#include "outputmethoddlg.h"
#include "processchain.h"
#include "projectmanager.h"

#include "microbe.h"
#include "gpasm.h"
#include "gpdasm.h"

#include <kdockwidget.h>
#include <kiconloader.h>
#include <klocale.h>
#include <qwhatsthis.h>

#include <cassert>


LanguageManager * LanguageManager::m_pSelf = 0;


LanguageManager * LanguageManager::self( KateMDI::ToolView * parent, KTechlab * ktl)
{
	if(!m_pSelf)
	{
		assert(parent);
		assert(ktl);
		m_pSelf = new LanguageManager( parent, ktl);
	}
	return m_pSelf;
}


LanguageManager::LanguageManager( KateMDI::ToolView * parent, KTechlab * ktl)
	: QObject((QObject*)ktl)
{
	p_ktechlab = ktl;
	m_logView = new LogView( parent, "LanguageManager LogView");
	
	QWhatsThis::add( m_logView, i18n("These messages show the output of language-related functionality such as compiling and assembling.<br><br>For error messages, clicking on the line will automatically open up the file at the position of the error."));
	connect( m_logView, SIGNAL(paraClicked(const QString&, MessageInfo)), this, SLOT(slotParaClicked(const QString&, MessageInfo)));
	reset();
}


LanguageManager::~LanguageManager()
{
}


void LanguageManager::reset()
{
	m_logView->clear();
}


ProcessChain * LanguageManager::compile( ProcessOptions options)
{
	if( KTLConfig::raiseMessagesLog())
		p_ktechlab->showToolView( p_ktechlab->toolView( toolViewIdentifier()));
	
	return new ProcessChain( options, p_ktechlab);
}


ProcessListChain * LanguageManager::compile( ProcessOptionsList pol)
{
	if( KTLConfig::raiseMessagesLog())
		p_ktechlab->showToolView( p_ktechlab->toolView( toolViewIdentifier()));
	
	return new ProcessListChain( pol, p_ktechlab);
}


void LanguageManager::slotError( const QString &error, MessageInfo messageInfo)
{
	m_logView->addOutput( error, LogView::ot_error, messageInfo);
}
void LanguageManager::slotWarning( const QString &error, MessageInfo messageInfo)
{
	m_logView->addOutput( error, LogView::ot_warning, messageInfo);
}
void LanguageManager::slotMessage( const QString &error, MessageInfo messageInfo)
{
	m_logView->addOutput( error, LogView::ot_message, messageInfo);
}

void LanguageManager::slotParaClicked( const QString& message, MessageInfo messageInfo)
{
	Q_UNUSED(message);
	DocManager::self()->gotoTextLine( messageInfo.fileURL(), messageInfo.fileLine());
}

#include "languagemanager.moc"
