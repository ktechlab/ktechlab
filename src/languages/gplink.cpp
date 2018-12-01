/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "gplink.h"
#include "languagemanager.h"
#include "logview.h"
#include "microinfo.h"
#include "microlibrary.h"

#include <klocalizedstring.h>
#include <kmessagebox.h>
#include <kprocess.h>
#include <kdebug.h>

#include <qfile.h>
#include <qdir.h>

#include <ktlconfig.h>

Gplink::Gplink( ProcessChain *processChain )
	: ExternalLanguage( processChain, "Gpasm" )
{
	m_successfulMessage = i18n("*** Linking successful ***");
	m_failedMessage = i18n("*** Linking failed ***");
	
	// search for SDCC
	
	m_sdccLibDir = "";
#define SEARCH_FOR_SDCC(dir) 			\
	{ 					\
    if ( m_sdccLibDir.isEmpty() ) { \
		QFile f(dir); 			\
		qDebug() << Q_FUNC_INFO << " SDCC lib testing " << dir ; \
		if( f.exists() ) { 		\
            qDebug() << Q_FUNC_INFO << " SDCC lib found " << dir ; \
			m_sdccLibDir = dir; 	\
        } \
    } \
	}
	
	// consider adding more paths here, if necessary
	if (!KTLConfig::sDCC_install_prefix().isEmpty()) {
        SEARCH_FOR_SDCC( KTLConfig::sDCC_install_prefix().append("/share/sdcc/lib") );
    }
	SEARCH_FOR_SDCC( "/usr/local/share/sdcc/lib" )
	SEARCH_FOR_SDCC( "/usr/share/sdcc/lib" )
	SEARCH_FOR_SDCC( "/usr/sdcc/lib" )
    SEARCH_FOR_SDCC( "/usr/sdcc/share/lib" )
	SEARCH_FOR_SDCC( "/opt/sdcc/lib" )
    SEARCH_FOR_SDCC( "/opt/sdcc/share/lib" )
    SEARCH_FOR_SDCC( QDir::homePath() + "/share/sdcc/lib" );
#undef SEARCH_FOR_SDCC
	if( m_sdccLibDir == "") {
		qWarning() << Q_FUNC_INFO << "SDCC lib not found. Expect linking errors";
    }
    {
        QFile f( m_sdccLibDir + "/../non-free/lib" );
        if ( !f.exists() ) {
            qWarning() << Q_FUNC_INFO << "SDCC non-free lib not found. Expect linking errors";
        }
    }
}


Gplink::~Gplink()
{
}


void Gplink::processInput( ProcessOptions options )
{
	resetLanguageProcess();
	m_processOptions = options;
	
	*m_languageProcess << ("gplink");
	
	// note: needed for newer GPUtils: relocate to shared memory if necessary
	*m_languageProcess << ("--use-shared");

	if ( !options.m_hexFormat.isEmpty() )
	{
		*m_languageProcess << ("--hex-format");
		*m_languageProcess << (options.m_hexFormat);
	}
	
	if ( options.m_bOutputMapFile )
		*m_languageProcess << ("--map");
	
	if ( !options.m_libraryDir.isEmpty() )
	{
		*m_languageProcess << ("--include");
		*m_languageProcess << ( options.m_libraryDir );
	}
	
	if ( !options.m_linkerScript.isEmpty() )
	{
		*m_languageProcess << ("--script");
		*m_languageProcess << ( options.m_linkerScript );
	}
	
	if ( !options.m_linkOther.isEmpty() )
		*m_languageProcess << (options.m_linkOther);
	
	// Output hex file
	*m_languageProcess << ("--output");
	*m_languageProcess << ( options.intermediaryOutput() );
	
	// Input object file
	const QStringList inputFiles = options.inputFiles();
	QStringList::const_iterator end = inputFiles.end();
	for ( QStringList::const_iterator it = inputFiles.begin(); it != end; ++it )
		*m_languageProcess << ( *it );
	
	// Other libraries
	end = options.m_linkLibraries.end();
	for ( QStringList::const_iterator it = options.m_linkLibraries.begin(); it != end; ++it )
		*m_languageProcess << ( *it );
	
	// if selected to automatically link to SDCC libraries, add some options.
	if( KTLConfig::gplink_link_shared() ) 
	{	
		// set up the include directory
		MicroInfo * info = MicroLibrary::self()->microInfoWithID( options.m_picID );
		if ( ! info )
		{
			// be program won't link anyway, but the user can't say that the program didn't try
			kError() << k_funcinfo << "Couldn't find the requested PIC" << options.m_picID << endl;
			kWarning() << k_funcinfo << "Supposing that the pic is pic12 or pic14" << endl;
			*m_languageProcess << "-I" << m_sdccLibDir + "/pic" ;
		}
		else
		{
            QString picLibSubdir;
			switch ( info->instructionSet()->set() ) {
                // note: is PIC12 supported by SDCC?
                case AsmInfo::PIC12:    picLibSubdir = "pic12";     break;
                case AsmInfo::PIC14:    picLibSubdir = "pic14";     break;
                case AsmInfo::PIC16:    picLibSubdir = "pic16";     break;
                default:
                    qWarning() << Q_FUNC_INFO << "Inexpected instruction set: " << (int) info->instructionSet()->set();
                    break;
            }
            *m_languageProcess << "-I" << m_sdccLibDir + "/" + picLibSubdir ;
            // non-free libraries are needed; beware the Debian/Ubuntu/derivatives' packages
            *m_languageProcess << "-I" << m_sdccLibDir + "/../non-free/lib/" + picLibSubdir ;
			// to pic to link against
			*m_languageProcess << options.m_picID.toLower().replace ( "p","pic" ) + ".lib";
		}
		*m_languageProcess << "libsdcc.lib";
	}
	
	if ( !start() )
	{
		KMessageBox::sorry( LanguageManager::self()->logView(), i18n("Linking failed. Please check you have gputils installed.") );
		processInitFailed();
		return;
	}
}


bool Gplink::isError( const QString &message ) const
{
	return message.contains( "Error", false );
}


bool Gplink::isWarning( const QString &message ) const
{
	return message.contains( "Warning", false );
}


MessageInfo Gplink::extractMessageInfo( const QString &text )
{

	if ( text.length()<5 || !text.startsWith("/") )
		return MessageInfo();
#if 0	
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
#endif
	return MessageInfo();
}



ProcessOptions::ProcessPath::Path Gplink::outputPath( ProcessOptions::ProcessPath::Path inputPath ) const
{
	switch (inputPath)
	{
		case ProcessOptions::ProcessPath::Object_PIC:
			return ProcessOptions::ProcessPath::Program_PIC;
			
		case ProcessOptions::ProcessPath::Object_Program:
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
		case ProcessOptions::ProcessPath::Object_Library:
		case ProcessOptions::ProcessPath::PIC_AssemblyAbsolute:
		case ProcessOptions::ProcessPath::Program_Disassembly:
		case ProcessOptions::ProcessPath::Program_PIC:
		case ProcessOptions::ProcessPath::Invalid:
		case ProcessOptions::ProcessPath::None:
			return ProcessOptions::ProcessPath::Invalid;
	}
	
	return ProcessOptions::ProcessPath::Invalid;
}



