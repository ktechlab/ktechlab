/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "asminfo.h"
#include "languagemanager.h"
#include "logview.h"
#include "microinfo.h"
#include "microlibrary.h"
#include "sdcc.h"
#include "src/core/ktlconfig.h"

#include <klocale.h>
#include <kmessagebox.h>
#include <k3process.h>

SDCC::SDCC( ProcessChain * processChain )
	: ExternalLanguage( processChain, "SDCC" )
{
	m_successfulMessage = i18n("*** Compilation successful ***");
	m_failedMessage = i18n("*** Compilation failed ***");
}

SDCC::~SDCC()
{
}

void SDCC::processInput( ProcessOptions options )
{
	resetLanguageProcess();
	
	MicroInfo * info = MicroLibrary::self()->microInfoWithID( options.m_picID );
	if (!info)
	{
		outputError( i18n("Could not find PIC with ID \"%1\".", options.m_picID) );
		return;
	}
	
	m_processOptions = options;
	
	*m_languageProcess << ("sdcc");
	
	
	//BEGIN Pass custom sdcc options
#define ARG(text,option) if ( KTLConfig::text() ) *m_languageProcess << ( QString("--%1").arg(option) );
	// General
	ARG( sDCC_nostdlib,			"nostdlib" )
	ARG( sDCC_nostdinc,			"nostdinc" )
	ARG( sDCC_less_pedantic,		"less-pedantic" )
	ARG( sDCC_std_c89,			"std-c89" )
	ARG( sDCC_std_c99,			"std-c99" )
	
	// Code generation
	ARG( sDCC_stack_auto,			"stack-auto" )
	ARG( sDCC_int_long_reent,		"int-long-reent" )
	ARG( sDCC_float_reent,			"float-reent" )
	ARG( sDCC_fommit_frame_pointer,	"fommit-frame-pointer" )
	ARG( sDCC_no_xinit_opt,			"no-xinit-opt" )
	ARG( sDCC_all_callee_saves,		"all-callee-saves" )
	
	// Optimization
	ARG( sDCC_nooverlay,			"nooverlay" )
	ARG( sDCC_nogcse,			"nogcse" )
	ARG( sDCC_nolabelopt,			"nolabelopt" )
	ARG( sDCC_noinvariant,			"noinvariant" )
	ARG( sDCC_noinduction,			"noinduction" )
	ARG( sDCC_no_peep,			"no-peep" )
	ARG( sDCC_noloopreverse,		"noloopreverse" )
	ARG( sDCC_opt_code_size,		"opt-code-size" )
	ARG( sDCC_opt_code_speed,		"opt-code-speed" )
	ARG( sDCC_peep_asm,			"peep-asm" )
	ARG( sDCC_nojtbound,			"nojtbound" )
	
	// PIC16 Specific
	if ( info->instructionSet()->set() == AsmInfo::PIC16 )
	{
		ARG( sDCC_nodefaultlibs,		"nodefaultlibs" )
		ARG( sDCC_pno_banksel,			"pno-banksel" )
		ARG( sDCC_pstack_model_large,	"pstack-model=large" )
		ARG( sDCC_debug_xtra,			"debug-xtra" )
		ARG( sDCC_denable_peeps,		"denable-peeps" )
		ARG( sDCC_calltree,			"calltree" )
		ARG( sDCC_fstack,			"fstack" )
		ARG( sDCC_optimize_goto,		"optimize-goto" )
		ARG( sDCC_optimize_cmp,			"optimize-cmp" )
		ARG( sDCC_optimize_df,			"optimize-df" )
	}
#undef ARG

	if ( !KTLConfig::miscSDCCOptions().isEmpty() )
		*m_languageProcess << ( KTLConfig::miscSDCCOptions() );
	//END Pass custom sdcc options
	
	
	*m_languageProcess << ("--debug"); // Enable debugging symbol output
	*m_languageProcess << ("-S"); // Compile only; do not assemble or link
	
	QString asmSwitch;
	switch ( info->instructionSet()->set() )
	{
		case AsmInfo::PIC12:
			// Last time I checked, SDCC doesn't support Pic12, and probably never will, but whatever...
			asmSwitch = "-mpic12";
			break;
		case AsmInfo::PIC14:
			asmSwitch = "-mpic14";
			break;
		case AsmInfo::PIC16:
			asmSwitch = "-mpic16";
			break;
	}
	
	*m_languageProcess << (asmSwitch);
	
	*m_languageProcess << ( "-"+options.m_picID.lower() );
	
	*m_languageProcess << ( options.inputFiles().first() );
	
	*m_languageProcess << ("-o");
	*m_languageProcess << ( options.intermediaryOutput() );
	
	if ( !start() )
	{
		KMessageBox::sorry( LanguageManager::self()->logView(), i18n("Compilation failed. Please check you have sdcc installed.") );
		processInitFailed();
		return;
	}
}


bool SDCC::isError( const QString &message ) const
{
	if ( message.startsWith("Error:") )
		return true;

	return false;
}

bool SDCC::isStderrOutputFatal( const QString & message ) const
{
	if ( message.startsWith("Processor:") )
		return false;
	
	return true;
}

bool SDCC::isWarning( const QString &message ) const
{
	if ( message.startsWith("Warning:") )
		return true;
	return false;
}

ProcessOptions::ProcessPath::Path SDCC::outputPath( ProcessOptions::ProcessPath::Path inputPath ) const
{
	switch (inputPath)
	{
		case ProcessOptions::ProcessPath::C_AssemblyRelocatable:
			return ProcessOptions::ProcessPath::None;
			
		case ProcessOptions::ProcessPath::C_Library:
			return ProcessOptions::ProcessPath::AssemblyRelocatable_Library;
			
		case ProcessOptions::ProcessPath::C_Object:
			return ProcessOptions::ProcessPath::AssemblyRelocatable_Object;
			
		case ProcessOptions::ProcessPath::C_PIC:
			return ProcessOptions::ProcessPath::AssemblyAbsolute_PIC;
			
		case ProcessOptions::ProcessPath::C_Program:
			return ProcessOptions::ProcessPath::AssemblyRelocatable_Program;
			
		case ProcessOptions::ProcessPath::AssemblyAbsolute_PIC:
		case ProcessOptions::ProcessPath::AssemblyAbsolute_Program:
		case ProcessOptions::ProcessPath::AssemblyRelocatable_Library:
		case ProcessOptions::ProcessPath::AssemblyRelocatable_Object:
		case ProcessOptions::ProcessPath::AssemblyRelocatable_PIC:
		case ProcessOptions::ProcessPath::AssemblyRelocatable_Program:
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
