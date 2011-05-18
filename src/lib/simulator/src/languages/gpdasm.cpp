/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "gpdasm.h"

#include "messageinfo.h"
#include "qprocesswitharguments.h"


#include <qfile.h>
#include <qregexp.h>
//Added by qt3to4:
#include <Q3TextStream>
#include <QDebug>

Gpdasm::Gpdasm( ProcessChain *processChain )
 : ExternalLanguage( processChain, "Gpdasm" )
{
	m_successfulMessage = tr("*** Disassembly successful ***");
	m_failedMessage = tr("*** Disassembly failed ***");
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
		// QMessageBox::critical( LanguageManager::self()->logView(), tr("Disassembly failed. Please check you have gputils installed.") );
        qDebug() << "disassembly failed";
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
	
	Q3TextStream stream(&file);
	stream << m_asmOutput;
	file.close();
	return true;
}


bool Gpdasm::isError( const QString &message ) const
{
	return (message.find( "error", -1, false ) != -1);
}


bool Gpdasm::isWarning( const QString &message ) const
{
	return (message.find( "warning", -1, false ) != -1);
}


MessageInfo Gpdasm::extractMessageInfo( const QString &text )
{
	if ( text.length()<5 || !text.startsWith("/") )
		return MessageInfo();
	
	const int index = text.find( ".asm", 0, false )+4;
	if ( index == -1+4 )
		return MessageInfo();
	const QString fileName = text.left(index);
	
	// Extra line number
	const QString message = text.right(text.length()-index);
	const int linePos = message.find( QRegExp(":[\\d]+") );
	int line = -1;
	if ( linePos != -1 )
	{
		const int linePosEnd = message.find( ':', linePos+1 );
		if ( linePosEnd != -1 )
		{
			const QString number = message.mid( linePos+1, linePosEnd-linePos-1 ).stripWhiteSpace();
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
