/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "asmparser.h"
// #include "docmanager.h"
#include "gpasm.h"

#include "ktlconfig.h"
#include "messageinfo.h"
#include "qprocesswitharguments.h"

#include <qregexp.h>
#include <QDebug>

Gpasm::Gpasm( ProcessChain *processChain )
 : ExternalLanguage( processChain, "Gpasm" )
{
	m_successfulMessage = tr("*** Assembly successful ***");
	m_failedMessage = tr("*** Assembly failed ***");
}


Gpasm::~Gpasm()
{
}


void Gpasm::processInput( ProcessOptions options )
{
	resetLanguageProcess();
	m_processOptions = options;
	
	AsmParser p( options.inputFiles().first() );
	p.parse();
	
	*m_languageProcess << ("gpasm");
	
	if ( ProcessOptions::ProcessPath::from( options.processPath() ) == ProcessOptions::ProcessPath::AssemblyRelocatable )
		*m_languageProcess << ("--object");
	
// 	*m_languageProcess << ("--debug-info"); // Debug info
	
	// Output filename
	*m_languageProcess << ("--output");
	*m_languageProcess << ( options.intermediaryOutput() );
	
	if ( !options.m_hexFormat.isEmpty() )
	{
		*m_languageProcess << ("--hex-format");
		*m_languageProcess << (options.m_hexFormat);
	}
	
	// Radix
	if ( !p.containsRadix() )
	{
		*m_languageProcess << ("--radix");
		switch( KtlConfig::self()->radix() )
		{
            case KtlConfig::Binary:
				*m_languageProcess << ("BIN");
				break;
			case KtlConfig::Octal:
				*m_languageProcess << ("OCT");
				break;
			case KtlConfig::Hexadecimal:
				*m_languageProcess << ("HEX");
				break;
			case KtlConfig::Decimal:
			default:
				*m_languageProcess << ("DEC");
				break;
		}
		*m_languageProcess << ("DEC"); // choose the default
	}
	
	// Warning Level
	*m_languageProcess << ("--warning");
	switch( KtlConfig::self()->gpasmWarningLevel() )
	{
		case KtlConfig::Warnings:
			*m_languageProcess << ("1");
			break;
		case KtlConfig::Errors:
			*m_languageProcess << ("2");
			break;
		default:
		case KtlConfig::All:
			*m_languageProcess << ("0");
			break;
	}
	
	// Ignore case
	if ( KtlConfig::self()->ignoreCase() )
		*m_languageProcess << ("--ignore-case");
	// Dos formatting
	if ( KtlConfig::self()->dosFormat() )
		*m_languageProcess << ("--dos");
	
	// Force list
	if ( options.b_forceList )
		*m_languageProcess << ("--force-list");
	
	// Other options
	if ( !KtlConfig::self()->miscGpasmOptions().isEmpty() )
		*m_languageProcess << ( KtlConfig::self()->miscGpasmOptions() );
	// Input Asm file
	*m_languageProcess << ( options.inputFiles().first() );
	
	if ( !start() )
	{
		// KMessageBox::sorry( LanguageManager::self()->logView(), tr("Assembly failed. Please check you have gputils installed.") );
        qCritical() << "assembly failed";
		processInitFailed();
		return;
	}
}


bool Gpasm::isError( const QString &message ) const
{
	return message.contains( "Error", false );
}


bool Gpasm::isWarning( const QString &message ) const
{
	return message.contains( "Warning", false );
}


ProcessOptions::ProcessPath::Path Gpasm::outputPath( ProcessOptions::ProcessPath::Path inputPath ) const
{
	switch (inputPath)
	{
		case ProcessOptions::ProcessPath::AssemblyAbsolute_PIC:
			return ProcessOptions::ProcessPath::Program_PIC;
			
		case ProcessOptions::ProcessPath::AssemblyAbsolute_Program:
			return ProcessOptions::ProcessPath::None;
			
		case ProcessOptions::ProcessPath::AssemblyRelocatable_Library:
			return ProcessOptions::ProcessPath::Object_Library;
			
		case ProcessOptions::ProcessPath::AssemblyRelocatable_Object:
			return ProcessOptions::ProcessPath::None;
			
		case ProcessOptions::ProcessPath::AssemblyRelocatable_PIC:
			return ProcessOptions::ProcessPath::Object_PIC;
			
		case ProcessOptions::ProcessPath::AssemblyRelocatable_Program:
			return ProcessOptions::ProcessPath::Object_Program;
			
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
		case ProcessOptions::ProcessPath::Object_Library:
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

