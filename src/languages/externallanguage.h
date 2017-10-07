/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef EXTERNALLANGUAGE_H
#define EXTERNALLANGUAGE_H

#include "language.h"

#include <qprocess.h>

class KProcess;

/**
Base class for Language support that relies on an external program; so this
class provides functionality for dealing with external processes.

@author Daniel Clarke
@author David Saxton
*/
class ExternalLanguage : public Language
{
Q_OBJECT
public:
	ExternalLanguage( ProcessChain *processChain, const QString &name );
	~ExternalLanguage();
	
protected slots:
	void processStdout();
	void processStderr();
	void processExited( int, QProcess::ExitStatus );
	
protected:
	/**
	 * Call this to start the language process. ExternalLanguage will ensure
	 * that communication et all is properly set up.
	 * @return true on success, false on error
	 */
	bool start();
	/**
	 * @returns whether the string outputted to stdout is an error or not
	 */
	virtual bool isError( const QString &message ) const = 0;
	/**
	 * @returns whether the string outputted to stderr is fatal (stopped compilation)
	 */
	virtual bool isStderrOutputFatal( const QString & message ) const { Q_UNUSED(message); return true; }
	/**
	 * @returns whether the string outputted to stdout is a warning or not
	 */
	virtual bool isWarning( const QString &message ) const = 0;
	/**
	 * Called when the process outputs a (non warning/error) message
	 */
	virtual void outputtedMessage( const QString &/*message*/ ) {};
	/**
	 * Called when the process outputs a warning
	 */
	virtual void outputtedWarning( const QString &/*message*/ ) {};
	/**
	 * Called when the process outputs an error
	 */
	virtual void outputtedError( const QString &/*message*/ ) {};
	/**
	 * Called when the process exits (called before any signals are emitted,
	 * etc). If you reinherit this function, you should return whether 
	 * everything is OK.
	 */
	virtual bool processExited( bool successfully ) { return successfully; }
	/**
	 * Call this function if your process could not be started - the process
	 * will be deleted, and a failure signal emitted.
	 */
	void processInitFailed();
	/**
	 * Disconnects and deletes the language's process.
	 */
	void deleteLanguageProcess();
	/**
	 * Creates process and makes connections, ready for the derived class to
	 * add arguments and start the process.
	 */
	void resetLanguageProcess();
	/**
	 * Prints out the command used for running the process, with any arguments
	 * that contain spaces put into quotation marks.
	 */
	void displayProcessCommand();
	
	KProcess * m_languageProcess;
};

#endif
