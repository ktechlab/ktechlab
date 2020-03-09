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

#include <qdebug.h>
#include <kprocess.h>
#include <kshell.h>

#include <qregexp.h>
#include <qtimer.h>

ExternalLanguage::ExternalLanguage( ProcessChain *processChain, const QString &name )
 : Language( processChain, name )
{
	m_languageProcess = nullptr;
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
	
	m_languageProcess = nullptr;
}


void ExternalLanguage::processStdout()
{
    QString allOut = m_languageProcess->readAllStandardOutput();
	QStringList lines = allOut.split('\n', QString::SkipEmptyParts); //QStringList::split( '\n', allOut, false ); // 2018.12.01
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


void ExternalLanguage::processStderr()
{
    QString allStdErr = m_languageProcess->readAllStandardError();
	QStringList lines = allStdErr.split('\n', QString::SkipEmptyParts); // QStringList::split( '\n', allStdErr, false );
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


void ExternalLanguage::processExited( int, QProcess::ExitStatus )
{
	if ( !m_languageProcess ) {
        qDebug() << Q_FUNC_INFO << " m_languageProcess == nullptr, returning";
		return;
    }
	bool allOk = processExited( (m_languageProcess->exitStatus() == QProcess::NormalExit) && (m_errorCount == 0) );
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
	
    m_languageProcess->setOutputChannelMode(KProcess::SeparateChannels);

	m_languageProcess->start( );
    return m_languageProcess->waitForStarted();
}


void ExternalLanguage::resetLanguageProcess()
{
	reset();
	deleteLanguageProcess();
	m_errorCount = 0;
	
	m_languageProcess = new KProcess(this);
	
	connect( m_languageProcess, SIGNAL(readyReadStandardOutput()),
			 this, SLOT(processStdout()) );
	
	connect( m_languageProcess, SIGNAL(readyReadStandardError()),
			 this, SLOT(processStderr()) );
	
	connect( m_languageProcess, SIGNAL(finished(int, QProcess::ExitStatus)),
			 this, SLOT(processExited(int, QProcess::ExitStatus )) );
}


void ExternalLanguage::displayProcessCommand()
{
	QStringList quotedArguments;
	QList<QString> arguments = m_languageProcess->program();
//     QList<QString> arguments;
//     for (QList<QByteArray>::const_iterator itArgs = m_languageProcess->args().begin();
//          itArgs != m_languageProcess->args().end();
//          ++itArgs) {
//         arguments.append(QString(*itArgs));
//     }
	
	if ( arguments.size() == 1 )
		quotedArguments << arguments[0];
		
	else
	{
		QList<QString>::const_iterator end = arguments.end();
	
		for ( QList<QString>::const_iterator it = arguments.begin(); it != end; ++it )
		{
			if ( (*it).isEmpty() || (*it).contains( QRegExp("[\\s]") ) )
				quotedArguments << KShell::quoteArg( *it );
			else
				quotedArguments << *it;
		}
	}
	
// 	outputMessage( "<i>" + quotedArguments.join(" ") + "</i>" );
	outputMessage( quotedArguments.join(" ") );
// 	LanguageManager::self()->logView()->addOutput( quotedArguments.join(" "), LogView::ot_info );
}
