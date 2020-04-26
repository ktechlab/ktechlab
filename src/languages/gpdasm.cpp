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
#include "gpdasm.h"
#include "logview.h"
#include "languagemanager.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <KProcess>

#include <QFile>
#include <QRegExp>
#include <QTextStream>

Gpdasm::Gpdasm( ProcessChain *processChain )
 : ExternalLanguage( processChain, "Gpdasm" )
{
	m_successfulMessage = i18n("*** Disassembly successful ***");
	m_failedMessage = i18n("*** Disassembly failed ***");
}


Gpdasm::~Gpdasm()
{
}


void Gpdasm::processInput( ProcessOptions options )
{
	resetLanguageProcess();
	m_asmOutput = "";
	m_processOptions = options;;
	
	*m_languageProcess << ("gpdasm");
	
	*m_languageProcess << ("--processor");
 	*m_languageProcess << ( options.m_picID );
	*m_languageProcess << ( options.inputFiles().first() );
	
	if ( !start() )
	{
		KMessageBox::sorry( LanguageManager::self()->logView(), i18n("Disassembly failed. Please check you have gputils installed.") );
		processInitFailed();
		return;
	}
}


void Gpdasm::outputtedMessage( const QString &message )
{
	m_asmOutput += message + "\n";
}


bool Gpdasm::processExited( bool successfully )
{
	if (!successfully)
		return false;

	QFile file(m_processOptions.intermediaryOutput());
	if ( file.open(QIODevice::WriteOnly) == false )
		return false;
	
	QTextStream stream(&file);
	stream << m_asmOutput;
	file.close();
	return true;
}


bool Gpdasm::isError( const QString &message ) const
{
	return (message.indexOf( "error", -1, Qt::CaseInsensitive ) != -1);
}


bool Gpdasm::isWarning( const QString &message ) const
{
	return (message.indexOf( "warning", -1, Qt::CaseInsensitive ) != -1);
}


MessageInfo Gpdasm::extractMessageInfo( const QString &text )
{
	if ( text.length()<5 || !text.startsWith("/") )
		return MessageInfo();
	
	const int index = text.indexOf( ".asm", 0, Qt::CaseInsensitive )+4;
	if ( index == -1+4 )
		return MessageInfo();
	const QString fileName = text.left(index);
	
	// Extra line number
	const QString message = text.right(text.length()-index);
	const int linePos = message.indexOf( QRegExp(":[\\d]+") );
	int line = -1;
	if ( linePos != -1 )
	{
		const int linePosEnd = message.indexOf( ':', linePos+1 );
		if ( linePosEnd != -1 )
		{
			const QString number = message.mid( linePos+1, linePosEnd-linePos-1 ).trimmed();
			bool ok;
			line = number.toInt(&ok)-1;
			if (!ok) line = -1;
		}
	}
	
	return MessageInfo( fileName, line );
}



ProcessOptions::ProcessPath::Path Gpdasm::outputPath( ProcessOptions::ProcessPath::Path inputPath ) const
{
	switch (inputPath)
	{
		case ProcessOptions::ProcessPath::Object_Disassembly:
		case ProcessOptions::ProcessPath::Program_Disassembly:
			return ProcessOptions::ProcessPath::None;
			
		case ProcessOptions::ProcessPath::AssemblyAbsolute_PIC:
		case ProcessOptions::ProcessPath::AssemblyAbsolute_Program:
		case ProcessOptions::ProcessPath::AssemblyRelocatable_Library:
		case ProcessOptions::ProcessPath::AssemblyRelocatable_Object:
		case ProcessOptions::ProcessPath::AssemblyRelocatable_PIC:
		case ProcessOptions::ProcessPath::AssemblyRelocatable_Program:
		case ProcessOptions::ProcessPath::C_AssemblyRelocatable:
		case ProcessOptions::ProcessPath::C_Library:
		case ProcessOptions::ProcessPath::C_Object:
		case ProcessOptions::ProcessPath::C_PIC:
		case ProcessOptions::ProcessPath::C_Program:
		case ProcessOptions::ProcessPath::FlowCode_AssemblyAbsolute:
		case ProcessOptions::ProcessPath::FlowCode_Microbe:
		case ProcessOptions::ProcessPath::FlowCode_PIC:
		case ProcessOptions::ProcessPath::FlowCode_Program:
		case ProcessOptions::ProcessPath::Microbe_AssemblyAbsolute:
		case ProcessOptions::ProcessPath::Microbe_PIC:
		case ProcessOptions::ProcessPath::Microbe_Program:
		case ProcessOptions::ProcessPath::Object_Library:
		case ProcessOptions::ProcessPath::Object_PIC:
		case ProcessOptions::ProcessPath::Object_Program:
		case ProcessOptions::ProcessPath::PIC_AssemblyAbsolute:
		case ProcessOptions::ProcessPath::Program_PIC:
		case ProcessOptions::ProcessPath::Invalid:
		case ProcessOptions::ProcessPath::None:
			return ProcessOptions::ProcessPath::Invalid;
	}
	
	return ProcessOptions::ProcessPath::Invalid;
}
