/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "contexthelp.h"
#include "docmanager.h"
#include "logview.h"
#include "microbe.h"
#include "languagemanager.h"

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

#include <Qt/qfile.h>
#include <k3process.h>

Microbe::Microbe( ProcessChain *processChain )
 : ExternalLanguage( processChain, "Microbe" )
{
	m_failedMessage = i18n("*** Compilation failed ***");
	m_successfulMessage = i18n("*** Compilation successful ***");
	
#if 0
	// Setup error messages list
	QFile file( locate("appdata",i1 8n("error_messages_en_gb")) );
	if ( file.open( IO_ReadOnly ) ) 
	{
        QTextStream stream( &file );
        QString line;
        while ( !stream.atEnd() )
		{
			line = stream.readLine(); // line of text excluding '\n'
			if ( !line.isEmpty() )
			{
				bool ok;
				const int pos = line.left( line.find("#") ).toInt(&ok);
				if (ok) {
					m_errorMessages[pos] = line.right(line.length()-line.find("#"));
				} else {
					kError() << k_funcinfo << "Error parsing Microbe error-message file"<<endl;
				}
			}
        }
		file.close();
	}
#endif
}

Microbe::~Microbe()
{
}


void Microbe::processInput( ProcessOptions options )
{
	resetLanguageProcess();
	m_processOptions = options;
	
	*m_languageProcess << ("microbe");
	
	// Input Asm file
	*m_languageProcess << ( options.inputFiles().first() );
	
	// Output filename
	*m_languageProcess << ( options.intermediaryOutput() );
	
	*m_languageProcess << ("--show-source");
	
	if ( !start() )
	{
		KMessageBox::sorry( LanguageManager::self()->logView(), i18n("Assembly failed. Please check you have KTechlab installed properly (\"microbe\" could not be started).") );
		processInitFailed();
		return;
	}
}


bool Microbe::isError( const QString &message ) const
{
	 return message.contains( "Error", false );
}

bool Microbe::isWarning( const QString &message ) const
{
	return message.contains( "Warning", false );
}


ProcessOptions::ProcessPath::Path Microbe::outputPath( ProcessOptions::ProcessPath::Path inputPath ) const
{
	switch (inputPath)
	{
		case ProcessOptions::ProcessPath::Microbe_AssemblyAbsolute:
			return ProcessOptions::ProcessPath::None;
			
		case ProcessOptions::ProcessPath::Microbe_PIC:
			return ProcessOptions::ProcessPath::AssemblyAbsolute_PIC;
			
		case ProcessOptions::ProcessPath::Microbe_Program:
			return ProcessOptions::ProcessPath::AssemblyAbsolute_Program;
			
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
