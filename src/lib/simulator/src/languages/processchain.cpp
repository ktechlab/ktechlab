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
#include "gplib.h"
// #include "src/core/ktlconfig.h"
#include "language.h"
// #include "languagemanager.h"
// #include "logview.h"
// #include "ktechlab.h"
#include "outputmethodinfo.h"
#include "processchain.h"
// #include "projectmanager.h"
// #include "textdocument.h"

#include "flowcode.h"
#include "gpasm.h"
#include "gpdasm.h"
#include "gplink.h"
#include "microbe.h"
#include "picprogrammer.h"
#include "sdcc.h"

#include <qdebug.h>
// #include <ktempfile.h>
#include <qfile.h>
#include <qtimer.h>
//Added by qt3to4:
#include <Q3TextStream>
#include <QTemporaryFile>


//BEGIN class ProcessChain
ProcessChain::ProcessChain( ProcessOptions options, const char *name )
{
	m_pFlowCode = 0l;
	m_pGpasm = 0l;
	m_pGpdasm = 0l;
	m_pGplib = 0l;
	m_pGplink = 0l;
	m_pMicrobe = 0l;
	m_pPicProgrammer = 0l;
	m_pSDCC = 0l;
	m_processOptions = options;
	
	QString target;
	if ( ProcessOptions::ProcessPath::to( options.processPath() ) == ProcessOptions::ProcessPath::Pic )
		target = options.m_picID;
	else
		target = options.targetFile();
	
	// LanguageManager::self()->logView()->addOutput( i18n("Building: %1").arg( target ), LogView::ot_important );
	QTimer::singleShot( 0, this, SLOT(compile()) );
}


ProcessChain::~ProcessChain()
{
	delete m_pFlowCode;
	delete m_pGpasm;
	delete m_pGpdasm;
	delete m_pGplib;
	delete m_pGplink;
	delete m_pMicrobe;
	delete m_pPicProgrammer;
	delete m_pSDCC;
}


// void ProcessChain::compile( ProcessOptions * options )
void ProcessChain::compile()
{
	// If the micro id in the options is empty, then attempt to get it from any
	// open project (it might not be necessarily...but won't hurt if it isn't).
    /*
	if ( m_processOptions.m_picID.isEmpty() )
	{
		if ( ProjectInfo * projectInfo = ProjectManager::self()->currentProject() )
		{
			ProjectItem * projectItem = projectInfo->findItem( m_processOptions.inputFiles().first() );
			if (projectItem)
				m_processOptions.m_picID = projectItem->microID();
		}
	}
	*/
	
	switch ( m_processOptions.processPath() )
	{
#define DIRECT_PROCESS( path, processor )       \
        case ProcessOptions::ProcessPath::path: { processor()->processInput(m_processOptions); break; }
#define INDIRECT_PROCESS( path, processor, extension )  \
        case ProcessOptions::ProcessPath::path:         \
            {                                           \
                QTemporaryFile f( QString("indirectXXXXXX").append(extension) ); f.close();    \
                m_processOptions.setIntermediaryOutput( f.name() );             \
                processor()->processInput(m_processOptions);                    \
                break;                                                          \
            }

		INDIRECT_PROCESS(	AssemblyAbsolute_PIC,			gpasm,		".hex" )
		DIRECT_PROCESS(		AssemblyAbsolute_Program,		gpasm )
		INDIRECT_PROCESS(	AssemblyRelocatable_Library,	gpasm,		".o" )
		DIRECT_PROCESS(		AssemblyRelocatable_Object,		gpasm )
		INDIRECT_PROCESS(	AssemblyRelocatable_PIC,		gpasm,		".o" )
		INDIRECT_PROCESS(	AssemblyRelocatable_Program,	gpasm,		".o" )
		DIRECT_PROCESS(		C_AssemblyRelocatable,			sdcc )
		INDIRECT_PROCESS(	C_Library,						sdcc,		".asm" )
		INDIRECT_PROCESS(	C_Object,						sdcc,		".asm" )
		INDIRECT_PROCESS(	C_PIC,							sdcc,		".asm" )
		INDIRECT_PROCESS(	C_Program,						sdcc,		".asm" )
// FIXME all flowcode is now disabled
//		INDIRECT_PROCESS(	FlowCode_AssemblyAbsolute,		flowCode,	".microbe" )
//		DIRECT_PROCESS(		FlowCode_Microbe,				flowCode )
//		INDIRECT_PROCESS(	FlowCode_PIC,					flowCode,	".microbe" )
//		INDIRECT_PROCESS(	FlowCode_Program,				flowCode,	".microbe" )
		DIRECT_PROCESS(		Microbe_AssemblyAbsolute,		microbe )
		INDIRECT_PROCESS(	Microbe_PIC,					microbe,	".asm" )
		INDIRECT_PROCESS(	Microbe_Program,				microbe,	".asm" )
		DIRECT_PROCESS(		Object_Disassembly,				gpdasm )
		DIRECT_PROCESS(		Object_Library,					gplib )
		INDIRECT_PROCESS(	Object_PIC,						gplink,		".lib" )
		DIRECT_PROCESS(		Object_Program,					gplink )
		DIRECT_PROCESS(		PIC_AssemblyAbsolute,			picProgrammer )
		DIRECT_PROCESS(		Program_Disassembly,			gpdasm )
		DIRECT_PROCESS(		Program_PIC,					picProgrammer )
#undef DIRECT_PROCESS
#undef INDIRECT_PROCESS
			
		case ProcessOptions::ProcessPath::Invalid:
			qWarning() << "Process path is invalid" << endl;
			
		case ProcessOptions::ProcessPath::None:
			qWarning() << "Nothing to do" << endl;
			break;
	}
}


void ProcessChain::slotFinishedCompile(Language *language)
{
	ProcessOptions options = language->processOptions();

    /*
	if ( options.b_addToProject && ProjectManager::self()->currentProject() )
		ProjectManager::self()->currentProject()->addFile( KURL(options.targetFile()) );
	*/
	ProcessOptions::ProcessPath::MediaType typeTo = ProcessOptions::ProcessPath::to( m_processOptions.processPath() );

    /*
	TextDocument * editor = 0l;
	if ( KTLConfig::reuseSameViewForOutput() )
	{
		editor = options.textOutputTarget();
		if ( editor && (!editor->url().isEmpty() || editor->isModified()) )
			editor = 0l;
	}
	switch (typeTo)
	{
		case ProcessOptions::ProcessPath::AssemblyAbsolute:
		case ProcessOptions::ProcessPath::AssemblyRelocatable:
		case ProcessOptions::ProcessPath::C:
		case ProcessOptions::ProcessPath::Disassembly:
		case ProcessOptions::ProcessPath::Library:
		case ProcessOptions::ProcessPath::Microbe:
		case ProcessOptions::ProcessPath::Object:
		case ProcessOptions::ProcessPath::Program:
		{
			switch ( options.method() )
			{
				case ProcessOptions::Method::LoadAsNew:
				{
					if ( !editor )
						editor = DocManager::self()->createTextDocument();
					
					if ( !editor )
						break;
				
					QString text;
					QFile f( options.targetFile() );
					if ( !f.open( QIODevice::ReadOnly ) )
					{
						editor->deleteLater();
						editor = 0l;
						break;
					}
				
					Q3TextStream stream(&f);
				
					while ( !stream.atEnd() )
						text += stream.readLine()+'\n';
				
					f.close();
	
					editor->setText( text, true );
					break;
				}
			
				case ProcessOptions::Method::Load:
				{
					editor = dynamic_cast<TextDocument*>( DocManager::self()->openURL(options.targetFile()) );
					break;
				}
			
				case ProcessOptions::Method::Forget:
					break;
			}
		}
			
		case ProcessOptions::ProcessPath::FlowCode:
		case ProcessOptions::ProcessPath::Pic:
		case ProcessOptions::ProcessPath::Unknown:
			break;
	}
	
	
	if (editor)
	{
		switch (typeTo)
		{
			case ProcessOptions::ProcessPath::AssemblyAbsolute:
			case ProcessOptions::ProcessPath::AssemblyRelocatable:
			{
				if ( KTLConfig::autoFormatMBOutput() )
					editor->formatAssembly();
				editor->slotInitLanguage( TextDocument::ct_asm );
				break;
			}
		
			case ProcessOptions::ProcessPath::C:
				editor->slotInitLanguage( TextDocument::ct_c );
				break;
			
			case ProcessOptions::ProcessPath::Disassembly:
				break;
			
			case ProcessOptions::ProcessPath::Library:
			case ProcessOptions::ProcessPath::Object:
			case ProcessOptions::ProcessPath::Program:
				editor->slotInitLanguage( TextDocument::ct_hex );
				break;
			
			case ProcessOptions::ProcessPath::Microbe:
				editor->slotInitLanguage( TextDocument::ct_microbe );
				break;
			
			case ProcessOptions::ProcessPath::FlowCode:
			case ProcessOptions::ProcessPath::Pic:
			case ProcessOptions::ProcessPath::Unknown:
				break;
		}
		
		DocManager::self()->giveDocumentFocus( editor );
	}
	
	options.setTextOutputtedTo( editor );

    */

    emit successful(options);
	emit successful();
}

#define LanguageFunction(a,b,c) \
a * ProcessChain::b( ) \
{ \
	if ( !c ) \
	{ \
		c = new a( this ); \
		connect( c, SIGNAL(processSucceeded(Language* )), this, SLOT(slotFinishedCompile(Language* )) ); \
		connect( c, SIGNAL(processFailed(Language* )), this, SIGNAL(failed()) ); \
	} \
	return c; \
}

// FIXME no flowcode for now
// LanguageFunction( FlowCode, flowCode, m_pFlowCode )
LanguageFunction( Gpasm, gpasm, m_pGpasm )
LanguageFunction( Gpdasm, gpdasm, m_pGpdasm )
LanguageFunction( Gplib, gplib, m_pGplib )
LanguageFunction( Gplink, gplink, m_pGplink )
LanguageFunction( Microbe, microbe, m_pMicrobe )
LanguageFunction( PicProgrammer, picProgrammer, m_pPicProgrammer )
LanguageFunction( SDCC, sdcc, m_pSDCC )
//END class ProcessChain



//BEGIN class ProcessListChain
ProcessListChain::ProcessListChain( ProcessOptionsList pol, const char * name )
{
	m_processOptionsList = pol;
	
	// Start us off...
	slotProcessChainSuccessful();
}


void ProcessListChain::slotProcessChainSuccessful()
{
	if ( m_processOptionsList.isEmpty() )
	{
		emit successful();
		return;
	}
	
	ProcessOptionsList::iterator it = m_processOptionsList.begin();
	ProcessOptions po = *it;
	m_processOptionsList.remove(it);

    /*
	ProcessChain * pc = LanguageManager::self()->compile(po);
	
	connect( pc, SIGNAL(successful()), this, SLOT(slotProcessChainSuccessful()) );
	connect( pc, SIGNAL(failed()), this, SLOT(slotProcessChainFailed()) );
    */
}


void ProcessListChain::slotProcessChainFailed()
{
	emit failed();
}
//END class ProcessListChain


#include "processchain.moc"
