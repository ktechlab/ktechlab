/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"

#ifndef NO_GPSIM

#include "asmparser.h"
#include "debugmanager.h"
#include "gpsimprocessor.h"
#include "language.h"
#include "microlibrary.h"
#include "processchain.h"
#include "simulator.h"

#include <cassert>

#include <qdebug.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <QList>
#include <QTemporaryFile>
// #include <qmessagebox.h>

#include "gpsim/cod.h"
#include "gpsim/interface.h"
#include "gpsim/gpsim_classes.h"
#include "gpsim/pic-processor.h"
#include "gpsim/registers.h"
#include "gpsim/14bit-registers.h"
#include "gpsim/symbol.h"
#include "gpsim/sim_context.h"

bool bDoneGpsimInit = false;
bool bUseGUI = true;
// extern "C" void initialize_gpsim();
// void initialize_gpsim(void);
extern void initialize_commands();
extern void initialize_readline();
extern void gui_main(void);
extern void cli_main();
void gpsim_version() {}
void quit_gui() {}


//BEGIN class GpsimProcessor
/**
Work around a bug in gpsim: the directory in a filename is recorded twice, e.g.
"/home/david/afile.asm" is recorded as "/home/david//home/david/afile.asm". This
function will remove the duplicated directory path (by searching for a "//").
*/
QString sanitizeGpsimFile( QString file )
{
	int pos = file.indexOf("//");
	if ( pos != -1 )
	{
		file.remove( 0, pos + 1 );
	}
	return file;
}


GpsimProcessor::GpsimProcessor( QString symbolFile, QObject *parent )
	: QObject(parent),
	m_symbolFile(symbolFile)
{
	if (!bDoneGpsimInit)
	{
		initialize_gpsim_core();
		initialization_is_complete();

		bDoneGpsimInit = true;
	}

	m_bCanExecuteNextCycle = true;
	m_bIsRunning = false;
	m_pPicProcessor = 0l;
	m_codLoadStatus = CodUnknown;
	m_pRegisterMemory = 0l;
	m_debugMode = GpsimDebugger::AsmDebugger;
	m_pDebugger[0] = m_pDebugger[1] = 0l;

	Processor * tempProcessor = 0l;
	const char * fileName = symbolFile.toAscii();

#ifdef GPSIM_0_21_4
	switch ( (cod_errors)load_symbol_file( &tempProcessor, fileName ) )
	{
		case COD_SUCCESS:
			m_codLoadStatus = CodSuccess;
			break;
		case COD_FILE_NOT_FOUND:
			m_codLoadStatus = CodFileNotFound;
			break;
		case COD_UNRECOGNIZED_PROCESSOR:
			m_codLoadStatus = CodUnrecognizedProcessor;
			break;
		case COD_FILE_NAME_TOO_LONG:
			m_codLoadStatus = CodFileNameTooLong;
			break;
		case COD_LST_NOT_FOUND:
			m_codLoadStatus = CodLstNotFound;
			break;
		case COD_BAD_FILE:
			m_codLoadStatus = CodBadFile;
			break;
		default:
			m_codLoadStatus = CodUnknown;
	}
#else // GPSIM_0_21_11+
	FILE * pFile = fopen( fileName, "r" );
	if ( !pFile )
		m_codLoadStatus = CodFileUnreadable;
	else
		m_codLoadStatus = ( ProgramFileTypeList::GetList().LoadProgramFile( & tempProcessor, fileName, pFile ) ) ? CodSuccess : CodFailure;
#endif

	m_pPicProcessor = dynamic_cast<pic_processor*>(tempProcessor);

	if ( codLoadStatus() == CodSuccess )
	{
		m_pRegisterMemory = new RegisterSet( m_pPicProcessor );
		m_pDebugger[0] = new GpsimDebugger( GpsimDebugger::AsmDebugger, this );
		m_pDebugger[1] = new GpsimDebugger( GpsimDebugger::HLLDebugger, this );
		Simulator::self()->attachGpsimProcessor(this);
		DebugManager::self()->registerGpsim(this);
	}

        m_stepsPerMicrosecond = 1;
}


GpsimProcessor::~GpsimProcessor()
{
	Simulator::self()->detachGpsimProcessor(this);
	delete m_pRegisterMemory;

	if ( m_pDebugger[0] )
		m_pDebugger[0]->deleteLater();
	if ( m_pDebugger[1] )
		m_pDebugger[1]->deleteLater();
}


void GpsimProcessor::displayCodLoadStatus( )
{
	switch (m_codLoadStatus)
	{
		case CodSuccess:
			break;
		case CodFileNotFound:
			// QMessageBox::critical( 0l, tr("The cod file \"%1\" was not found.").arg(m_symbolFile), tr("File Not Found") );
			qCritical() << tr("The cod file \"%1\" was not found.").arg(m_symbolFile);
			break;
		case CodUnrecognizedProcessor:
			// QMessageBox::critical( 0l, tr("The processor for cod file \"%1\" is unrecognized.").arg(m_symbolFile), tr("Unrecognized Processor") );
			qCritical() << tr("The processor for cod file \"%1\" is unrecognized.").arg(m_symbolFile);
			break;
		case CodFileNameTooLong:
			// QMessageBox::critical( 0l, tr("The file name \"%1\" is too long.").arg(m_symbolFile), tr("Filename Too Long") );
			qCritical() << tr("The file name \"%1\" is too long.").arg(m_symbolFile);
			break;
		case CodLstNotFound:
			// QMessageBox::critical( 0l, tr("The lst file associated with the cod file \"%1\" was not found.").arg(m_symbolFile), tr("LST File Not Found") );
			qCritical() << tr("The lst file associated with the cod file \"%1\" was not found.").arg(m_symbolFile);
			break;
		case CodBadFile:
			// QMessageBox::critical( 0l, tr("The cod file \"%1\" is bad.").arg(m_symbolFile), tr("Bad File") );
			qCritical() << tr("The cod file \"%1\" is bad.").arg(m_symbolFile);
			break;
		case CodFileUnreadable:
			// QMessageBox::critical( 0l, tr("The cod file \"%1\" could not be read from.").arg(m_symbolFile), tr("Unreadable File") );
			qCritical() << tr("The cod file \"%1\" could not be read from.").arg(m_symbolFile);
			break;
		case CodFailure:
		case CodUnknown:
			// QMessageBox::critical( 0l, tr("An error occured with the cod file \"%1\".").arg(m_symbolFile), tr("Error") );
			qCritical() << tr("An error occured with the cod file \"%1\".").arg(m_symbolFile);
			break;
	}
}


unsigned GpsimProcessor::programMemorySize() const
{
	return m_pPicProcessor->program_memory_size();
}


QStringList GpsimProcessor::sourceFileList()
{
	QStringList files;

	// Work around nasty bug in gpsim 0.21.4 where nsrc_files value might be used uninitiazed
	int max = m_pPicProcessor->files.nsrc_files();
#ifdef GPSIM_0_21_4
	if ( max > 10 )
		max = 10;
#endif

	for ( int i = 0; i < max; ++i )
	{
		if ( !m_pPicProcessor->files[i] )
			continue;

		files << sanitizeGpsimFile( m_pPicProcessor->files[i]->name().c_str() );
	}

	return files;
}


void GpsimProcessor::emitLineReached()
{
	m_pDebugger[0]->emitLineReached();
	m_pDebugger[1]->emitLineReached();
}


void GpsimProcessor::setRunning( bool run )
{
	if ( m_bIsRunning == run )
		return;

	m_bIsRunning = run;
	emit runningStatusChanged(run);
}


void GpsimProcessor::executeNext()
{
	if ( !m_bIsRunning )
		return;

	if ( !m_bCanExecuteNextCycle )
	{
		m_bCanExecuteNextCycle = true;
		return;
	}

	unsigned long long beforeExecuteCount = get_cycles().get();

	if(get_bp().have_interrupt())
	{
		m_pPicProcessor->interrupt();
	}
	else
	{
		m_pPicProcessor->step_one(false); // Don't know what the false is for; gpsim ignores its value anyway

		// Some instructions take more than one cycle to execute, so ignore next cycle if this was the case
		if ( (get_cycles().get() - beforeExecuteCount) > 1 )
			m_bCanExecuteNextCycle = false;
	}

	currentDebugger()->checkForBreak();

	// Let's also update the values of RegisterInfo every 25 milliseconds
	if ( (beforeExecuteCount % 10000) == 0 )
		registerMemory()->update();
}


void GpsimProcessor::reset()
{
	bool wasRunning = isRunning();
	m_pPicProcessor->reset(SIM_RESET);
	setRunning(false);
	if (!wasRunning)
	{
		// If we weren't running before, then the next signal won't have been emitted
		emitLineReached();
	}
}


MicroInfo * GpsimProcessor::microInfo( ) const
{
	if ( !m_pPicProcessor ){
		qWarning() << " m_pPicProcessor == NULL" << endl;
		return 0l;
	}

	return MicroLibrary::self()->microInfoWithID( m_pPicProcessor->name().c_str() );
}


int GpsimProcessor::operandRegister( unsigned address )
{
	instruction * ins = m_pPicProcessor->program_memory[ address ];
	if ( Register_op * reg = dynamic_cast<Register_op*>(ins) )
		return reg->register_address;
	return -1;
}


int GpsimProcessor::operandLiteral( unsigned address )
{
	instruction * ins = m_pPicProcessor->program_memory[ address ];
	if ( Literal_op * lit = dynamic_cast<Literal_op*>(ins) )
		return lit->L;
	return -1;
}


GpsimProcessor::ProgramFileValidity GpsimProcessor::isValidProgramFile( const QString & programFile )
{
	if ( !QFile::exists(programFile) )
		return DoesntExist;

	QString extension = programFile.right(
        programFile.length() - programFile.lastIndexOf('.') - 1
        ).toLower();

	if ( extension == "flowcode" ||
			extension == "asm" ||
			extension == "cod" ||
			extension == "basic" || extension == "microbe" ||
	   		extension == "c" )
		return Valid;

	if ( extension == "hex" && QFile::exists( QString(programFile).replace(".hex",".cod") ) )
		return Valid;

	return IncorrectType;
}


QString GpsimProcessor::generateSymbolFile( const QString &fileName, QObject *receiver, const char *successMember, const char * failMember )
{
	if ( !isValidProgramFile(fileName) )
		return QString::null;

	QString extension = fileName.right(
        fileName.length() - fileName.lastIndexOf('.') - 1
        ).toLower();

	if ( extension == "cod" )
	{
		QTimer::singleShot( 0, receiver, successMember );
		return fileName;
	}
	if ( extension == "hex" )
	{
		QTimer::singleShot( 0, receiver, successMember );
		// We've already checked for the existance of the ".cod" file in GpsimProcessor::isValidProgramFile
		return QString(fileName).replace(".hex",".cod");
	}

	else if ( extension == "basic" || extension == "microbe" )
	{
		compileMicrobe( fileName, receiver, successMember, failMember );
		return QString(fileName).replace( "."+extension, ".cod" );
	}
	else if ( extension == "flowcode" )
	{
		const QString hexFile = QTemporaryFile( "tmpXXXXXX.hex" ).fileName();

		ProcessOptions o;
		o.b_addToProject = false;
		o.setTargetFile( hexFile );
		o.setInputFiles( QStringList(fileName) );
		o.setMethod( ProcessOptions::Method::Forget );
		o.setProcessPath( ProcessOptions::ProcessPath::FlowCode_Program );

		ProcessChain * pc = new ProcessChain(o);
		if (receiver)
		{
			if (successMember)
				connect( pc, SIGNAL(successful()), receiver, successMember );
			if (failMember)
				connect( pc, SIGNAL(failed()), receiver, failMember );
		}

		return QString(hexFile).replace( ".hex", ".cod" );
	}
	else if ( extension == "asm" )
	{
		ProcessOptions o;
		o.b_addToProject = false;
		o.setTargetFile( QString(fileName).replace(".asm",".hex"));
		o.setInputFiles(QStringList(fileName));
		o.setMethod( ProcessOptions::Method::Forget );
		o.setProcessPath( ProcessOptions::ProcessPath::path( ProcessOptions::guessMediaType(fileName), ProcessOptions::ProcessPath::Program ) );

		ProcessChain *pc = new ProcessChain(o);
		if (receiver)
		{
			if (successMember)
				connect( pc, SIGNAL(successful()), receiver, successMember );
			if (failMember)
				connect( pc, SIGNAL(failed()), receiver, failMember );
		}

		return QString(fileName).replace(".asm",".cod");
	}
	else if ( extension == "c" )
	{
		ProcessOptions o;
		o.b_addToProject = false;
		o.setTargetFile( QString(fileName).replace(".c",".hex"));
		o.setInputFiles(QStringList(fileName));
		o.setMethod( ProcessOptions::Method::Forget );
		o.setProcessPath( ProcessOptions::ProcessPath::C_Program );

		ProcessChain *pc = new ProcessChain(o);
		if (receiver)
		{
			if (successMember)
				connect( pc, SIGNAL(successful()), receiver, successMember );
			if (failMember)
				connect( pc, SIGNAL(failed()), receiver, failMember );
		}

		return QString(fileName).replace(".c",".cod");
	}

	if ( failMember )
		QTimer::singleShot( 0, receiver, failMember );
	return QString::null;
}


void GpsimProcessor::compileMicrobe( const QString &filename, QObject *receiver, const char * successMember, const char * failMember )
{
	ProcessOptions o;
	o.b_addToProject = false;
	o.setTargetFile( QString(filename).replace(".microbe",".hex") );
	o.setInputFiles(filename);
	o.setMethod( ProcessOptions::Method::Forget );
	o.setProcessPath( ProcessOptions::ProcessPath::Microbe_Program );
	ProcessChain * pc = new ProcessChain(o);
	if (receiver)
	{
		if (successMember)
			connect( pc, SIGNAL(successful()), receiver, successMember );
		if (failMember)
			connect( pc, SIGNAL(failed()), receiver, failMember );
	}
}

void GpsimProcessor::setStepsPerMicrosecond(const unsigned int steps){
    m_stepsPerMicrosecond = steps;
}

unsigned int GpsimProcessor::stepsPerMicrosecond(){
    return m_stepsPerMicrosecond;
}


//END class GpsimProcessor



//BEGIN class GpsimDebugger
GpsimDebugger::GpsimDebugger( Type type, GpsimProcessor * gpsim )
	: QObject()
{
	m_pGpsim = gpsim;
	m_type = type;
	m_pBreakFromOldLine = 0l;
	m_addressToLineMap = 0l;
	m_stackLevelLowerBreak = -1;
	m_addressSize = 0;

	connect( m_pGpsim, SIGNAL(runningStatusChanged(bool )), this, SLOT(gpsimRunningStatusChanged(bool )) );

	if ( type == HLLDebugger )
	{
		const QStringList sourceFileList = m_pGpsim->sourceFileList();
		QStringList::const_iterator sflEnd = sourceFileList.end();
		for ( QStringList::const_iterator it = sourceFileList.begin(); it != sflEnd; ++it )
		{
			AsmParser p(*it);
			p.parse(this);
		}
	}

	initAddressToLineMap();
}


GpsimDebugger::~GpsimDebugger()
{
	Q3ValueList<DebugLine*> debugLinesToDelete;

	for ( unsigned i = 0; i < m_addressSize; ++i )
	{
		DebugLine * dl = m_addressToLineMap[i];
		if ( !dl || dl->markedAsDeleted() )
			continue;

		dl->markAsDeleted();
		debugLinesToDelete += dl;
	}

	const Q3ValueList<DebugLine*>::iterator end = debugLinesToDelete.end();
	for ( Q3ValueList<DebugLine*>::iterator it = debugLinesToDelete.begin(); it != end; ++it )
		delete *it;

	delete [] m_addressToLineMap;
}


void GpsimDebugger::gpsimRunningStatusChanged( bool isRunning )
{
	if (!isRunning)
	{
		m_stackLevelLowerBreak = -1;
		m_pBreakFromOldLine = 0l;
		emitLineReached();
	}
}


void GpsimDebugger::associateLine( const QString & sourceFile, int sourceLine, const QString & assemblyFile, int assemblyLine )
{
	if ( assemblyLine < 0 || sourceLine < 0 )
	{
		qWarning() << "Invalid lines: assemblyLine="<<assemblyLine<<" sourceLine="<<sourceLine<<endl;
		return;
	}

	SourceLine hllSource = SourceLine( sourceFile, sourceLine );
	SourceLine asmSource = SourceLine( assemblyFile, assemblyLine );

	if ( m_sourceLineMap.contains(asmSource) )
	{
		qWarning() << "Already have an association for assembly (\""<<assemblyFile<<"\","<<assemblyLine<<")"<<endl;
		return;
	}

	m_sourceLineMap[asmSource] = hllSource;
}


void GpsimDebugger::initAddressToLineMap()
{
	m_addressSize = m_pGpsim->programMemorySize();

	delete [] m_addressToLineMap;
	m_addressToLineMap = new DebugLine*[m_addressSize];
	memset( m_addressToLineMap, 0, m_addressSize * sizeof(DebugLine*) );

	if ( m_type == AsmDebugger )
	{
		for ( unsigned i = 0; i < m_addressSize; ++i )
		{
			int line = m_pGpsim->picProcessor()->pma->get_src_line(i) - 1;
			int fileID = m_pGpsim->picProcessor()->pma->get_file_id(i);
			FileContext * fileContext = m_pGpsim->picProcessor()->files[fileID];

			if (fileContext)
				m_addressToLineMap[i] = new DebugLine( sanitizeGpsimFile( fileContext->name().c_str() ), line );
		}
	}
	else
	{
		SourceLineMap::const_iterator slmEnd = m_sourceLineMap.end();
		for ( SourceLineMap::const_iterator it = m_sourceLineMap.begin(); it != slmEnd; ++it )
		{
			SourceLineMap::const_iterator next = it;
			++next;

			int asmToLine = ((next == slmEnd) || (next.key().fileName() != it.key().fileName())) ? -1 : next.key().line() - 1;

			QString asmFile = it.key().fileName();
			int asmFromLine = it.key().line();
			SourceLine sourceLine = it.value();


			std::string stdAsmFile( asmFile.toAscii() );
			int fileID = m_pGpsim->picProcessor()->files.Find( stdAsmFile );
			if ( fileID == -1 )
			{
				qWarning() << "Could not find FileContext (asmFile=\""<<asmFile<<"\")"<<endl;
				continue;
			}

			if ( asmToLine == -1 )
				asmToLine = m_pGpsim->picProcessor()->files[fileID]->max_line() - 2;

			if ( (asmFromLine < 0) || (asmToLine < asmFromLine) )
			{
				qWarning() << "Invalid lines: asmFromLine="<<asmFromLine<<" asmToLine="<<asmToLine<<endl;
				continue;
			}

			DebugLine * debugLine = new DebugLine( sourceLine.fileName(), sourceLine.line() );
			bool used = false;

			for ( int i = asmFromLine; i <= asmToLine; ++i )
			{
#ifdef GPSIM_0_21_4
				int address = m_pGpsim->picProcessor()->pma->find_address_from_line( fileID, i+1 );
#else // GPSIM_0_21_11
				int address = m_pGpsim->picProcessor()->pma->find_address_from_line( m_pGpsim->picProcessor()->files[fileID], i+1 );
#endif
				if ( address != -1 )
				{
					used = true;
					m_addressToLineMap[address] = debugLine;
				}
			}

			if (!used)
				delete debugLine;
		}
	}
}


void GpsimDebugger::setBreakpoints( const QString & path, const IntList & lines )
{
	for ( unsigned i = 0; i < m_addressSize; i++ )
	{
		DebugLine * dl = m_addressToLineMap[i];
		if ( !dl || dl->fileName() != path )
			continue;

		dl->setBreakpoint( lines.contains( dl->line() ) );
	}
}


void GpsimDebugger::setBreakpoint( const QString & path, int line, bool isBreakpoint )
{
	for ( unsigned i = 0; i < m_addressSize; i++ )
	{
		if ( !m_addressToLineMap[i] )
			continue;

		if ( (m_addressToLineMap[i]->fileName() == path) &&
					( line == m_addressToLineMap[i]->line() ) )
			m_addressToLineMap[i]->setBreakpoint(isBreakpoint);
	}
}


DebugLine * GpsimDebugger::currentDebugLine()
{
	return m_addressToLineMap[ m_pGpsim->picProcessor()->pc->get_value() ];
}


SourceLine GpsimDebugger::currentLine()
{
	DebugLine * dl = currentDebugLine();
	return dl ? *dl : SourceLine();
}


void GpsimDebugger::emitLineReached()
{
	SourceLine currentAt = currentLine();

	if ( currentAt == m_previousAtLineEmit )
		return;

	m_previousAtLineEmit = currentAt;
	m_pGpsim->registerMemory()->update();
	emit lineReached(currentAt);
}


void GpsimDebugger::checkForBreak()
{
	DebugLine * currentLine = m_addressToLineMap[ m_pGpsim->picProcessor()->pc->get_value() ];
	int currentStackLevel = int( m_pGpsim->picProcessor()->stack->pointer & m_pGpsim->picProcessor()->stack->stack_mask );

	bool ontoNextLine = m_pBreakFromOldLine != currentLine;
	bool lineBreakpoint = currentLine ? currentLine->isBreakpoint() : false;
	bool stackBreakpoint = m_stackLevelLowerBreak >= currentStackLevel;

	if ( ontoNextLine && (lineBreakpoint || stackBreakpoint) )
		m_pGpsim->setRunning(false);
}


int GpsimDebugger::programAddress( const QString & path, int line )
{
	for ( unsigned i = 0; i < m_addressSize; ++i )
	{
		DebugLine * dl = m_addressToLineMap[i];
		if ( !dl || (dl->line() != line) || (dl->fileName() != path) )
			continue;

		return i;
	}

	return -1;
}


void GpsimDebugger::stepInto()
{
	// I'm not aware of the stack being able to increase in size by more than
	// one at a time, so "1" should suffice here...but to be on the safe side,
	// make it a nice large number
	stackStep( 1 << 16 );
}
void GpsimDebugger::stepOver()
{
	stackStep(0);
}
void GpsimDebugger::stepOut()
{
	stackStep(-1);
}
void GpsimDebugger::stackStep( int dl )
{
	if ( m_pGpsim->isRunning() )
		return;

	int initialStack = (m_pGpsim->picProcessor()->stack->pointer & m_pGpsim->picProcessor()->stack->stack_mask) + dl;
	DebugLine * initialLine = currentDebugLine();

	if ( initialStack < 0 )
		initialStack = 0;

	// Reset any previous stackStep, and step
	m_pBreakFromOldLine = 0l;
	m_stackLevelLowerBreak = -1;
	m_pGpsim->picProcessor()->step_one(false);

	int currentStack = m_pGpsim->picProcessor()->stack->pointer & m_pGpsim->picProcessor()->stack->stack_mask;
	DebugLine * currentLine = currentDebugLine();

	if ( (initialStack >= currentStack) && (initialLine != currentLine) )
		emitLineReached();

	else
	{
		// Looks like we stepped into something or haven't gone onto the next
		// instruction, wait until we step back out....
		m_stackLevelLowerBreak = initialStack;
		m_pBreakFromOldLine = initialLine;
		m_pGpsim->setRunning(true);
	}
}
//END class Debugger



//BEGIN class RegisterSet
RegisterSet::RegisterSet( pic_processor * picProcessor )
{
	unsigned numRegisters = picProcessor->rma.get_size();
	qDebug() << "numRegisters="<<numRegisters<<endl;
	m_registers.resize( numRegisters );
	for ( unsigned i = 0; i < numRegisters; ++i )
	{
		RegisterInfo * info = new RegisterInfo( & picProcessor->rma[i] );
		m_registers[i] = info;
		m_nameToRegisterMap[ info->name() ] = info;
	}

	RegisterInfo * info = new RegisterInfo( picProcessor->W );
	m_registers.append( info );
	m_nameToRegisterMap[ info->name() ] = info;
}


RegisterSet::~RegisterSet()
{
	for (int i = 0; i < m_registers.size(); ++i )
		delete m_registers[i];
}


RegisterInfo * RegisterSet::fromAddress( unsigned address )
{
	return ((int)address < m_registers.size()) ? m_registers[address] : 0l;
}


RegisterInfo * RegisterSet::fromName( const QString & name )
{
	// First try the name as case sensitive, then as case insensitive.
	if ( m_nameToRegisterMap.contains( name ) )
		return m_nameToRegisterMap[ name ];

	QString nameLower = name.toLower();

	RegisterInfoMap::iterator end = m_nameToRegisterMap.end();
	for ( RegisterInfoMap::iterator it = m_nameToRegisterMap.begin(); it != end; ++ it )
	{
		if ( it.key().toLower() == nameLower )
			return it.value();
	}

	return 0l;
}


void RegisterSet::update()
{
	for (int i = 0; i < m_registers.size(); ++i )
		m_registers[i]->update();
}
//END class RegisterSet



//BEGIN class RegisterInfo
RegisterInfo::RegisterInfo( Register * reg )
{
	assert(reg);
	m_pRegister = reg;
	m_type = Invalid;
	m_prevEmitValue = 0;

	switch ( m_pRegister->isa() )
	{
		case Register::GENERIC_REGISTER:
			m_type = Generic;
			break;
		case Register::FILE_REGISTER:
			m_type = File;
			break;
		case Register::SFR_REGISTER:
			m_type = SFR;
			break;
		case Register::BP_REGISTER:
			m_type = Breakpoint;
			break;
		case Register::INVALID_REGISTER:
			m_type = Invalid;
			break;
	}

	m_name = m_pRegister->baseName().c_str();
}


unsigned RegisterInfo::value() const
{
	return m_pRegister->value.data;
}


void RegisterInfo::update()
{
	unsigned newValue = value();
	if ( newValue != m_prevEmitValue )
	{
		m_prevEmitValue = newValue;
		emit valueChanged(newValue);
	}
}


QString RegisterInfo::toString( RegisterType type )
{
	switch ( type )
	{
		case Generic:
			return tr("Generic");

		case File:
			return tr("File");

		case SFR:
			return tr("SFR");

		case Breakpoint:
			return tr("Breakpoint");

		case Invalid:
			return tr("Invalid");
	}

	return tr("Unknown");
}
//END class RegisterInfo



//BEGIN class DebugLine
DebugLine::DebugLine( const QString & fileName, int line )
	: SourceLine( fileName, line )
{
	m_bIsBreakpoint = false;
	m_bMarkedAsDeleted = false;
}


DebugLine::DebugLine()
	: SourceLine()
{
	m_bIsBreakpoint = false;
	m_bMarkedAsDeleted = false;
}
//END class DebugLine


#include "gpsimprocessor.moc"

#endif
