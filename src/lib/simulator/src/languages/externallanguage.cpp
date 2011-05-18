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

#include "qprocesswitharguments.h"

#include <qregexp.h>
#include <qtimer.h>
#include <QDebug>

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


void ExternalLanguage::receivedStdout( QProcess *, char * buffer, int buflen )
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


void ExternalLanguage::receivedStderr( QProcess *, char * buffer, int buflen )
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


void ExternalLanguage::processExited( QProcess * )
{
	if ( !m_languageProcess )
		return;
	bool allOk = processExited( (m_languageProcess->exitStatus() == QProcess::NormalExit) && m_errorCount == 0 );
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
	
	m_languageProcess->start(); // KProcess::NotifyOnExit, KProcess::All ); // FIXME
	return true;
}


void ExternalLanguage::resetLanguageProcess()
{
	reset();
	deleteLanguageProcess();
	m_errorCount = 0;
	
	m_languageProcess = new QProcessWithArguments(this);
	
	connect( m_languageProcess, SIGNAL(receivedStdout( QProcess*, char*, int )),
			 this, SLOT(receivedStdout( QProcess*, char*, int )) );
	
	connect( m_languageProcess, SIGNAL(receivedStderr( QProcess*, char*, int )),
			 this, SLOT(receivedStderr( QProcess*, char*, int )) );
	
	connect( m_languageProcess, SIGNAL(processExited( QProcess* )),
			 this, SLOT(processExited( QProcess* )) );
}


void ExternalLanguage::displayProcessCommand()
{
	QStringList quotedArguments;
	QStringList arguments = m_languageProcess->args();
	
	if ( arguments.size() == 1 )
		quotedArguments << arguments[0];
		
	else
	{
        QChar q('\'');
        foreach(QString arg, arguments){
            if(arg.isEmpty() || arg.contains( QRegExp("[\\s]")) )
                    // see KProcess::quote(QString),
                    // http://api.kde.org/3.5-api/kdelibs-apidocs/kdecore/html/classKProcess.html#1ce049f3abcc805d6e5d44988451d32a
                quotedArguments << arg.replace(q,"'\\''").prepend(q).append(q);
            else
                quotedArguments << arg;
        }
	}
	
// 	outputMessage( "<i>" + quotedArguments.join(" ") + "</i>" );
	// outputMessage( quotedArguments.join(" ") );
// 	LanguageManager::self()->logView()->addOutput( quotedArguments.join(" "), LogView::ot_info );
    qDebug() << "Process command is: \n" << quotedArguments.join(" ");
}


#include "externallanguage.moc"
