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
#include "ktechlab.h"
#include "language.h"
#include "logview.h"
#include "outputmethoddlg.h"
#include "processchain.h"
#include "projectmanager.h"
#include "languagemanager.h"

#include <kdebug.h>
//#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <kprocess.h>

#include <qregexp.h>
#include <qtimer.h>

#include <ktlconfig.h>

//BEGIN class Language
Language::Language( ProcessChain *processChain, const QString &name )
	: QObject( KTechlab::self() /*, name */ )
{
    setObjectName(name.latin1());
	p_processChain = processChain;
}


Language::~Language()
{
}


void Language::outputMessage( const QString &message )
{
	LanguageManager::self()->slotMessage( message, extractMessageInfo(message) );
}


void Language::outputWarning( const QString &message )
{
	LanguageManager::self()->slotWarning( message, extractMessageInfo(message) );
}


void Language::outputError( const QString &message )
{
	LanguageManager::self()->slotError( message, extractMessageInfo(message) );
	m_errorCount++;
}


void Language::finish( bool successful )
{
	if (successful)
	{
		outputMessage(m_successfulMessage + "\n");
		KTechlab::self()->slotChangeStatusbar(m_successfulMessage);
		
		ProcessOptions::ProcessPath::Path newPath = outputPath( m_processOptions.processPath() );
		
		if ( newPath == ProcessOptions::ProcessPath::None )
			emit processSucceeded(this);
		
		else if (p_processChain)
		{
			m_processOptions.setInputFiles( QStringList( m_processOptions.intermediaryOutput() ) );
			m_processOptions.setIntermediaryOutput( m_processOptions.targetFile() );
			m_processOptions.setProcessPath(newPath);
// 			p_processChain->compile(m_processOptions);
			p_processChain->setProcessOptions(m_processOptions);
			p_processChain->compile();
		}
	}
	else
	{
		outputError(m_failedMessage + "\n");
		KTechlab::self()->slotChangeStatusbar(m_failedMessage);
		emit processFailed(this);
		return;
	}
}


void Language::reset()
{
	m_errorCount = 0;
}


MessageInfo Language::extractMessageInfo( const QString &text )
{
	if ( !text.startsWith("/") )
		return MessageInfo();
	
	const int index = text.find( ":", 0, false );
	if ( index == -1 )
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
//END class Language



//BEGIN class ProcessOptionsSpecial
ProcessOptionsSpecial::ProcessOptionsSpecial()
{
	m_bOutputMapFile = true;
	b_forceList = true;
	b_addToProject = ProjectManager::self()->currentProject();
	
	p_flowCodeDocument = 0l;
	
	switch ( KTLConfig::hexFormat() )
	{
		case KTLConfig::EnumHexFormat::inhx8m:
			m_hexFormat = "inhx8m";
			break;
			
		case KTLConfig::EnumHexFormat::inhx8s:
			m_hexFormat = "inhx8s";
			break;
			
		case KTLConfig::EnumHexFormat::inhx16:
			m_hexFormat = "inhx16";
			break;
			
		case KTLConfig::EnumHexFormat::inhx32:
		default:
			m_hexFormat = "inhx32";
			break;
	}
}
//END class ProcessOptionsSpecial


//BEGIN class ProcessOptions
ProcessOptions::ProcessOptions()
{
	m_pHelper = new ProcessOptionsHelper;
	
	b_targetFileSet = false;
	m_pTextOutputTarget = 0l;
}


ProcessOptions::ProcessOptions( OutputMethodInfo info )
{
	m_pHelper = new ProcessOptionsHelper;
	
	b_addToProject = info.addToProject();
	m_picID = info.picID();
	b_targetFileSet = false;
	
	setTargetFile( info.outputFile().path() );
	
	switch ( info.method() )
	{
		case OutputMethodInfo::Method::Direct:
			m_method = Method::LoadAsNew;
			break;
			
		case OutputMethodInfo::Method::SaveAndForget:
			m_method = Method::Forget;
			break;
			
		case OutputMethodInfo::Method::SaveAndLoad:
			m_method = Method::Load;
			break;
	}
}


void ProcessOptions::setTextOutputTarget( TextDocument * target, QObject * receiver, const char * slot )
{
	m_pTextOutputTarget = target;
	QObject::connect( m_pHelper, SIGNAL(textOutputtedTo( TextDocument* )), receiver, slot );
}


void ProcessOptions::setTextOutputtedTo( TextDocument * outputtedTo )
{
	m_pTextOutputTarget = outputtedTo;
	emit m_pHelper->textOutputtedTo( m_pTextOutputTarget );
}


void ProcessOptions::setTargetFile( const QString &file )
{
	if (b_targetFileSet)
	{
		kWarning() << "Trying to reset target file!"<<endl;
		return;
	}
	m_targetFile = file;
	m_intermediaryFile = file;
	b_targetFileSet = true;
}


ProcessOptions::ProcessPath::MediaType ProcessOptions::guessMediaType( const QString & url )
{
	QString extension = url.right( url.length() - url.findRev('.') - 1 );
	extension = extension.lower();
	
	if ( extension == "asm" )
	{
		// We'll have to look at the file contents to determine its type...
		AsmParser p( url );
		p.parse();
		switch ( p.type() )
		{
			case AsmParser::Relocatable:
				return ProcessPath::AssemblyRelocatable;
				
			case AsmParser::Absolute:
				return ProcessPath::AssemblyAbsolute;
		}
	}
	
	if ( extension == "c" )
		return ProcessPath::C;
	
	if ( extension == "flowcode" )
		return ProcessPath::FlowCode;
	
	if ( extension == "a" || extension == "lib" )
		return ProcessPath::Library;
	
	if ( extension == "microbe" || extension == "basic" )
		return ProcessPath::Microbe;
	
	if ( extension == "o" )
		return ProcessPath::Object;
	
	if ( extension == "hex" )
		return ProcessPath::Program;
	
	return ProcessPath::Unknown;
}


ProcessOptions::ProcessPath::Path ProcessOptions::ProcessPath::path( MediaType from, MediaType to )
{
	switch (from)
	{
		case AssemblyAbsolute:
			switch (to)
			{
				case AssemblyAbsolute:
					return None;
				case Pic:
					return AssemblyAbsolute_PIC;
				case Program:
					return AssemblyAbsolute_Program;
					
				case AssemblyRelocatable:
				case C:
				case Disassembly:
				case FlowCode:
				case Library:
				case Microbe:
				case Object:
				case Unknown:
					return Invalid;
			}
			
		case AssemblyRelocatable:
			switch (to)
			{
				case Library:
					return AssemblyRelocatable_Library;
				case Object:
					return AssemblyRelocatable_Object;
				case Pic:
					return AssemblyRelocatable_PIC;
				case Program:
					return AssemblyRelocatable_Program;
					
				case AssemblyAbsolute:
				case AssemblyRelocatable:
				case C:
				case Disassembly:
				case FlowCode:
				case Microbe:
				case Unknown:
					return Invalid;
			}
			
		case C:
			switch (to)
			{
				case AssemblyRelocatable:
					return C_AssemblyRelocatable;
				case Library:
					return C_Library;
				case Object:
					return C_Object;
				case Pic:
					return C_PIC;
				case Program:
					return C_Program;
					
				case AssemblyAbsolute:
				case C:
				case Disassembly:
				case FlowCode:
				case Microbe:
				case Unknown:
					return Invalid;
			}
			
		case Disassembly:
			return Invalid;
			
		case FlowCode:
			switch (to)
			{
				case AssemblyAbsolute:
					return FlowCode_AssemblyAbsolute;
				case Microbe:
					return FlowCode_Microbe;
				case Pic:
					return FlowCode_PIC;
				case Program:
					return FlowCode_Program;
					
				case AssemblyRelocatable:
				case C:
				case Disassembly:
				case FlowCode:
				case Library:
				case Object:
				case Unknown:
					return Invalid;
			}
			
		case Library:
			return Invalid;
			
		case Microbe:
			switch (to)
			{	
				case AssemblyAbsolute:
					return Microbe_AssemblyAbsolute;
				case Pic:
					return Microbe_PIC;
				case Program:
					return Microbe_Program;
					
				case AssemblyRelocatable:
				case C:
				case Disassembly:
				case FlowCode:
				case Library:
				case Microbe:
				case Object:
				case Unknown:
					return Invalid;
			}
			
		case Object:
			switch (to)
			{
				case Disassembly:
					return Object_Disassembly;
				case Library:
					return Object_Library;
				case Pic:
					return Object_PIC;
				case Program:
					return Object_Program;
					
				case AssemblyAbsolute:
				case AssemblyRelocatable:
				case C:
				case FlowCode:
				case Microbe:
				case Object:
				case Unknown:
					return Invalid;
			}
			
		case Pic:
			return Invalid;
			
		case Program:
			switch (to)
			{
				case Disassembly:
					return Program_Disassembly;
				case Pic:
					return Program_PIC;
					
				case AssemblyAbsolute:
				case AssemblyRelocatable:
				case C:
				case FlowCode:
				case Library:
				case Microbe:
				case Object:
				case Program:
				case Unknown:
					return Invalid;
			}
				
		case Unknown:
			return Invalid;
	}
	
	return Invalid;
}


ProcessOptions::ProcessPath::MediaType ProcessOptions::ProcessPath::from( Path path )
{
	switch (path)
	{
		case ProcessPath::AssemblyAbsolute_PIC:
		case ProcessPath::AssemblyAbsolute_Program:
			return AssemblyAbsolute;
			
		case ProcessPath::AssemblyRelocatable_Library:
		case ProcessPath::AssemblyRelocatable_Object:
		case ProcessPath::AssemblyRelocatable_PIC:
		case ProcessPath::AssemblyRelocatable_Program:
			return AssemblyRelocatable;
			
		case ProcessPath::C_AssemblyRelocatable:
		case ProcessPath::C_Library:
		case ProcessPath::C_Object:
		case ProcessPath::C_PIC:
		case ProcessPath::C_Program:
			return C;
			
		case ProcessPath::FlowCode_AssemblyAbsolute:
		case ProcessPath::FlowCode_Microbe:
		case ProcessPath::FlowCode_PIC:
		case ProcessPath::FlowCode_Program:
			return FlowCode;
			
		case ProcessPath::Microbe_AssemblyAbsolute:
		case ProcessPath::Microbe_PIC:
		case ProcessPath::Microbe_Program:
			return Microbe;
			
		case ProcessPath::Object_Disassembly:
		case ProcessPath::Object_Library:
		case ProcessPath::Object_PIC:
		case ProcessPath::Object_Program:
			return Object;
			
		case ProcessPath::PIC_AssemblyAbsolute:
			return Pic;
			
		case ProcessPath::Program_Disassembly:
		case ProcessPath::Program_PIC:
			return Program;
			
		case ProcessPath::Invalid:
		case ProcessPath::None:
			return Unknown;
	}
	
	return Unknown;
}


ProcessOptions::ProcessPath::MediaType ProcessOptions::ProcessPath::to( Path path )
{
	switch (path)
	{
		case ProcessPath::FlowCode_AssemblyAbsolute:
		case ProcessPath::Microbe_AssemblyAbsolute:
		case ProcessPath::PIC_AssemblyAbsolute:
			return AssemblyAbsolute;
			
		case ProcessPath::C_AssemblyRelocatable:
			return AssemblyRelocatable;
			
		case ProcessPath::Object_Disassembly:
		case ProcessPath::Program_Disassembly:
			return Disassembly;
			
		case ProcessPath::AssemblyRelocatable_Library:
		case ProcessPath::C_Library:
		case ProcessPath::Object_Library:
			return Library;
			
		case ProcessPath::FlowCode_Microbe:
			return Microbe;
			
		case ProcessPath::AssemblyRelocatable_Object:
		case ProcessPath::C_Object:
			return Object;
			
		case ProcessPath::AssemblyAbsolute_PIC:
		case ProcessPath::AssemblyRelocatable_PIC:
		case ProcessPath::C_PIC:
		case ProcessPath::FlowCode_PIC:
		case ProcessPath::Microbe_PIC:
		case ProcessPath::Object_PIC:
		case ProcessPath::Program_PIC:
			return Pic;
			
		case ProcessPath::AssemblyAbsolute_Program:
		case ProcessPath::AssemblyRelocatable_Program:
		case ProcessPath::C_Program:
		case ProcessPath::FlowCode_Program:
		case ProcessPath::Microbe_Program:
		case ProcessPath::Object_Program:
			return Program;
			
		case ProcessPath::Invalid:
		case ProcessPath::None:
			return Unknown;
	}
	
	return Unknown;
}
//END class ProcessOptions


#include "language.moc"
