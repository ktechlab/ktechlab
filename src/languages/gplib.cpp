/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "gplib.h"
#include "languagemanager.h"
#include "logview.h"

#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <k3process.h>

Gplib::Gplib( ProcessChain *processChain )
	: ExternalLanguage( processChain, "Gpasm" )
{
	m_successfulMessage = i18n("*** Archiving successful ***");
	m_failedMessage = i18n("*** Archiving failed ***");
}


Gplib::~Gplib()
{
}


void Gplib::processInput( ProcessOptions options )
{
	resetLanguageProcess();
	m_processOptions = options;
	
	*m_languageProcess << ("gplib");
	*m_languageProcess << ("--create");
	
	*m_languageProcess << ( options.intermediaryOutput() );
	
	const QStringList inputFiles = options.inputFiles();
	QStringList::const_iterator end = inputFiles.end();
	for ( QStringList::const_iterator it = inputFiles.begin(); it != end; ++it )
		*m_languageProcess << ( *it );

	if ( !start() )
	{
		KMessageBox::sorry( LanguageManager::self()->logView(), i18n("Linking failed. Please check you have gputils installed.") );
		processInitFailed();
		return;
	}
}


bool Gplib::isError( const QString &message ) const
{
	return message.contains( "Error", false );
}


bool Gplib::isWarning( const QString &message ) const
{
	return message.contains( "Warning", false );
}


MessageInfo Gplib::extractMessageInfo( const QString &text )
{

	if ( text.length()<5 || !text.startsWith("/") )
		return MessageInfo();
#if 0	
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
#endif
	return MessageInfo();
}




ProcessOptions::ProcessPath::Path Gplib::outputPath( ProcessOptions::ProcessPath::Path inputPath ) const
{
	switch (inputPath)
	{
		case ProcessOptions::ProcessPath::Object_Library:
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
		case ProcessOptions::ProcessPath::Object_Disassembly:
		case ProcessOptions::ProcessPath::Object_PIC:
		case ProcessOptions::ProcessPath::Object_Program:
		case ProcessOptions::ProcessPath::PIC_AssemblyAbsolute:
		case ProcessOptions::ProcessPath::Program_Disassembly:
		case ProcessOptions::ProcessPath::Program_PIC:
		case ProcessOptions::ProcessPath::Invalid:
		case ProcessOptions::ProcessPath::None:
			return ProcessOptions::ProcessPath::Invalid;
	}
	
	return ProcessOptions::ProcessPath::Invalid;
}
