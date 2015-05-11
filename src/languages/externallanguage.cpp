/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "externallanguage.h"
#include "languagemanager.h"
#include "logview.h"

#include <kdebug.h>
#include <k3process.h>

#include <Qt/qregexp.h>
#include <Qt/qtimer.h>

ExternalLanguage::ExternalLanguage( ProcessChain *processChain, const QString &name )
 : Language( processChain, name )
{
	m_languageProcess = 0l;
}


ExternalLanguage::~ExternalLanguage()
{
	deleteLanguageProcess();
}


void ExternalLanguage::deleteLanguageProcess()
{
	if (!m_languageProcess)
		return;
	
	// I'm not too sure if this combination of killing the process is the best way....
// 	m_languageProcess->tryTerminate();
// 	QTimer::singleShot( 5000, m_languageProcess, SLOT( kill() ) );
//  	delete m_languageProcess;
	m_languageProcess->kill();
	m_languageProcess->deleteLater();
	
	m_languageProcess = 0L;
}


void ExternalLanguage::receivedStdout( K3Process *, char * buffer, int buflen )
{
	QStringList lines = QStringList::split( '\n', QString::fromLocal8Bit( buffer, buflen ), false );
	QStringList::iterator end = lines.end();
	
	for ( QStringList::iterator it = lines.begin(); it != end; ++it )
	{
		if ( isError( *it ) )
		{
			outputError( *it );
			outputtedError( *it );
		}
		else if ( isWarning( *it ) )
		{
			outputWarning( *it );
			outputtedWarning( *it );
		}
		else
		{
			outputMessage( *it );
			outputtedMessage( *it );
		}
	}
}


void ExternalLanguage::receivedStderr( K3Process *, char * buffer, int buflen )
{
	QStringList lines = QStringList::split( '\n', QString::fromLocal8Bit( buffer, buflen ), false );
	QStringList::iterator end = lines.end();
	
	for ( QStringList::iterator it = lines.begin(); it != end; ++it )
	{
		if ( isStderrOutputFatal( *it ) )
		{
			outputError( *it );
			outputtedError( *it );
		}
		else
		{
			outputWarning( *it );
			outputtedWarning( *it );
		}
	}
}


void ExternalLanguage::processExited( K3Process * )
{
	if ( !m_languageProcess )
		return;
	bool allOk = processExited( (m_languageProcess->normalExit()) && (m_errorCount == 0) );
	finish(allOk);
	deleteLanguageProcess();
}


void ExternalLanguage::processInitFailed()
{
	finish(false);
	deleteLanguageProcess();
}


bool ExternalLanguage::start()
{
	displayProcessCommand();
	
	return m_languageProcess->start( K3Process::NotifyOnExit, K3Process::All );
}


void ExternalLanguage::resetLanguageProcess()
{
	reset();
	deleteLanguageProcess();
	m_errorCount = 0;
	
	m_languageProcess = new K3Process(this);
	
	connect( m_languageProcess, SIGNAL(receivedStdout( K3Process*, char*, int )),
			 this, SLOT(receivedStdout( K3Process*, char*, int )) );
	
	connect( m_languageProcess, SIGNAL(receivedStderr( K3Process*, char*, int )),
			 this, SLOT(receivedStderr( K3Process*, char*, int )) );
	
	connect( m_languageProcess, SIGNAL(processExited( K3Process* )),
			 this, SLOT(processExited( K3Process* )) );
}


void ExternalLanguage::displayProcessCommand()
{
	QStringList quotedArguments;
	//QList<QString> arguments = m_languageProcess->args();
    QList<QString> arguments;
    for (QList<QByteArray>::const_iterator itArgs = m_languageProcess->args().begin();
         itArgs != m_languageProcess->args().end();
         ++itArgs) {
        arguments.append(QString(*itArgs));
    }
	
	if ( arguments.size() == 1 )
		quotedArguments << arguments[0];
		
	else
	{
		QList<QString>::const_iterator end = arguments.end();
	
		for ( QList<QString>::const_iterator it = arguments.begin(); it != end; ++it )
		{
			if ( (*it).isEmpty() || (*it).contains( QRegExp("[\\s]") ) )
				quotedArguments << K3Process::quote( *it );
			else
				quotedArguments << *it;
		}
	}
	
// 	outputMessage( "<i>" + quotedArguments.join(" ") + "</i>" );
	outputMessage( quotedArguments.join(" ") );
// 	LanguageManager::self()->logView()->addOutput( quotedArguments.join(" "), LogView::ot_info );
}


#include "externallanguage.moc"
