/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "languagemanager.h"
#include "picprogrammer.h"
// #include "src/core/ktlconfig.h"
#include "qprocesswitharguments.h"

// #include <kapplication.h>
// #include <kconfig.h>
#include <qdebug.h>
// #include <klocale.h>
// #include <kmessagebox.h>

#include <qapplication.h>
#include <qfile.h>
// #include <kprocess.h>
#include <qregexp.h>
#include <q3textstream.h>
#include <qdatetime.h>

#include <stdio.h>


//BEGIN class ProgrammerConfig
ProgrammerConfig::ProgrammerConfig()
{
}


void ProgrammerConfig::reset()
{
	initCommand = QString::null;
	readCommand = QString::null;
	writeCommand = QString::null;
	verifyCommand = QString::null;
	blankCheckCommand = QString::null;
	eraseCommand = QString::null;
}
//END class ProgrammerConfig



//BEGIN class PicProgrammerSettings
bool PicProgrammerSettings::m_bDoneStaticConfigsInit = false;
ProgrammerConfigMap PicProgrammerSettings::m_staticConfigs = ProgrammerConfigMap();


PicProgrammerSettings::PicProgrammerSettings()
{
	if ( !m_bDoneStaticConfigsInit )
		initStaticConfigs();
}


void PicProgrammerSettings::initStaticConfigs()
{
	m_bDoneStaticConfigsInit = true;
	ProgrammerConfig config;
	
	config.description = QString(tr("Supported programmers: %1")).arg("JuPic, PICStart Plus, Warp-13");
	config.description += tr("<br>Interface: Serial Port");
	config.initCommand = "";
	config.readCommand = "picp %port %device -rp %file";
	config.writeCommand = "picp %port %device -wp %file";
	config.verifyCommand = "";
	config.blankCheckCommand = "picp %port %device -b";
	config.eraseCommand = "picp %port %device -e";
// 	config.executable = "picp";
	m_staticConfigs[ "PICP" ] = config;
	
	
	config.description = QString(tr("Supported programmers: %1")).arg("Epic Plus");
	config.description += tr("<br>Interface: Parallel Port");
	config.initCommand = "odyssey init";
	config.readCommand = "odyssey %device read %file";
	config.writeCommand = "odyssey %device write %file";
	config.verifyCommand = "odyssey %device verify %file";
	config.blankCheckCommand = "odyssey %device blankcheck";
	config.eraseCommand = "odyssey %device erase";
// 	config.executable = "odyssey";
	m_staticConfigs[ "Odyssey" ] = config;
	
	
	config.description = QString(tr("Supported programmers: %1")).arg("JDM PIC-Programmer 2, PIC-PG2C");
	config.description += tr("<br>Interface: Serial Port");
	config.initCommand = "";
	config.readCommand = "picprog --output %file --pic %port";
	config.writeCommand = "picprog --burn --input %file --pic %port --device %device";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "picprog --erase --pic %device";
	m_staticConfigs[ "PICProg" ] = config;
	
	
	config.description = QString(tr("Supported programmers: %1")).arg("Epic Plus");
	config.description += tr("<br>Interface: Parallel Port");
	config.initCommand = "";
	config.readCommand = "dump84 --dump-all --output=%file";
	config.writeCommand = "prog84 --intel16=%file";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "prog84 --clear";
	m_staticConfigs[ "prog84" ] = config;
	
	
	config.description = QString(tr("Supported programmers: %1")).arg("Kit 149, Kit 150");
	config.description += tr("<br>Interface: USB Port");
	config.initCommand = "";
	config.readCommand = "pp -d %device -r %file";
	config.writeCommand = "pp -d %device -w %file";
	config.verifyCommand = "pp -d %device -v %file";
	config.blankCheckCommand = "";
	config.eraseCommand = "pp -d %device -e";
	m_staticConfigs[ "PP" ] = config;
	
	
	config.description = QString(tr("Supported programmers: %1")).arg("Wisp628");
	config.description += tr("<br>Interface: Serial Port");
	config.initCommand = "";
	config.readCommand = "xwisp ID %device PORT %device DUMP";
	config.writeCommand = "xwisp ID %device PORT %device WRITE %file";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "xwisp ID %device PORT %device ERASE";
	m_staticConfigs[ "XWisp" ] = config;
	
	
#if 0
	config.description = QString(tr("Supported programmers: %1")).arg("Epic Plus, JDM PIC-Programmer 2, PICCOLO, PICCOLO Grande, Trivial HVP Programmer");
	config.description += QString(tr("<br>Interface: Serial Port and Parallel Port");
	config.initCommand = "";
	config.readCommand = "";
	config.writeCommand = "";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "";
	config.executable = "pkp";
	m_staticConfigs[ "PiKdev" ] = config;
	config.executable = "";
	
	
	config.description = QString(tr("Supported programmers: %1")).arg("Trivial LVP programmer, Trivial HVP Programmer");
	config.description += QString(tr("<br>Interface: Parallel Port");
	config.initCommand = "";
	config.readCommand = "";
	config.writeCommand = "";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "";
	m_staticConfigs[ "PicPrg2" ] = config;
	
	
	config.description = QString(tr("Supported programmers: %1")).arg("El Cheapo");
	config.description += QString(tr("<br>Interface: Parallel Port");
	config.initCommand = "";
	config.readCommand = "";
	config.writeCommand = "";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "";
	m_staticConfigs[ "PP06" ] = config;
	
	
	config.description = QString(tr("Supported programmers: %1")).arg("NOPPP");
	config.description += QString(tr("<br>Interface: Parallel Port");
	config.initCommand = "";
	config.readCommand = "";
	config.writeCommand = "";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "";
	m_staticConfigs[ "NOPPP" ] = config;
	
	
	config.description = QString(tr("Supported programmers: %1")).arg("SNOPPP");
	config.description += QString(tr("<br>Interface: Parallel Port");
	config.initCommand = "";
	config.readCommand = "";
	config.writeCommand = "";
	config.verifyCommand = "";
	config.blankCheckCommand = "";
	config.eraseCommand = "";
	m_staticConfigs[ "SNOPPP" ] = config;
#endif
}


void PicProgrammerSettings::load() {}
#if 0
KConfig * config )
{
	QStringList oldCustomProgrammers = config->groupList().grep("CustomProgrammer_");
	QStringList::iterator ocpEnd = oldCustomProgrammers.end();
	for ( QStringList::iterator it = oldCustomProgrammers.begin(); it != ocpEnd; ++it )
	{
		// The CustomProgrammer_ string we searched for might appear half way through... (don't want)
		if ( (*it).startsWith("CustomProgrammer_") )
		{
			config->setGroup(*it);
			
			ProgrammerConfig pc;
			pc.initCommand = config->readEntry( "InitCommand" );
			pc.readCommand = config->readEntry( "ReadCommand" );
			pc.writeCommand = config->readEntry( "WriteCommand" );
			pc.verifyCommand = config->readEntry( "VerifyCommand" );
			pc.blankCheckCommand = config->readEntry( "BlankCheckCommand" );
			pc.eraseCommand = config->readEntry( "EraseCommand" );
			
			QString name = config->readEntry( "Name" );
			m_customConfigs[name] = pc;
		}
	}
}
#endif

void PicProgrammerSettings::save() {}
#if 0
KConfig * config )
{
	QStringList oldCustomProgrammers = config->groupList().grep("CustomProgrammer_");
	QStringList::iterator ocpEnd = oldCustomProgrammers.end();
	for ( QStringList::iterator it = oldCustomProgrammers.begin(); it != ocpEnd; ++it )
	{
		// The CustomProgrammer_ string we searched for might appear half way through... (don't want)
		if ( (*it).startsWith("CustomProgrammer_") )
			config->deleteGroup(*it);
	}
	
	int at = 0;
	ProgrammerConfigMap::iterator end = m_customConfigs.end();
	for ( ProgrammerConfigMap::iterator it = m_customConfigs.begin(); it != end; ++it )
	{
		config->setGroup( QString("CustomProgrammer_%1")).arg(at++) );
		
		config->writeEntry( "Name", it.key() );
		config->writeEntry( "InitCommand", it.data().initCommand );
		config->writeEntry( "ReadCommand", it.data().readCommand );
		config->writeEntry( "WriteCommand", it.data().writeCommand );
		config->writeEntry( "VerifyCommand", it.data().verifyCommand );
		config->writeEntry( "BlankCheckCommand", it.data().blankCheckCommand );
		config->writeEntry( "EraseCommand", it.data().eraseCommand );
	}
}
#endif

ProgrammerConfig PicProgrammerSettings::config( const QString & name )
{
	if ( name.isEmpty() )
		return ProgrammerConfig();
	
	QString l = name.lower();
		
	ProgrammerConfigMap::const_iterator end = m_customConfigs.end();
	for ( ProgrammerConfigMap::const_iterator it = m_customConfigs.begin(); it != end; ++it )
	{
		if ( it.key().lower() == l )
			return *it;
	}
	
	end = m_staticConfigs.end();
	for ( ProgrammerConfigMap::const_iterator it = m_staticConfigs.begin(); it != end; ++it )
	{
		if ( it.key().lower() == l )
			return *it;
	}
	
	return m_customConfigs[ name ];
}


void PicProgrammerSettings::removeConfig( const QString & name )
{
	if ( isPredefined( name ) )
	{
		qWarning() << "Cannot remove a predefined PIC programmer configuration." << endl;
		return;
	}
	
	QString l = name.lower();
	
	ProgrammerConfigMap::iterator end = m_customConfigs.end();
	for ( ProgrammerConfigMap::iterator it = m_customConfigs.begin(); it != end; ++it )
	{
		if ( it.key().lower() == l )
		{
			m_customConfigs.remove( it );
			return;
		}
	}
}


void PicProgrammerSettings::saveConfig( const QString & name, const ProgrammerConfig & config )
{
	if ( isPredefined( name ) )
	{
		qWarning() << "Cannot save to a predefined PIC programmer configuration." << endl;
		return;
	}
	
	QString l = name.lower();
	
	ProgrammerConfigMap::iterator end = m_customConfigs.end();
	for ( ProgrammerConfigMap::iterator it = m_customConfigs.begin(); it != end; ++it )
	{
		if ( it.key().lower() == l )
		{
			*it = config;
			return;
		}
	}
	
	m_customConfigs[ name ] = config;
}


QStringList PicProgrammerSettings::configNames( bool makeLowercase ) const
{
	if ( !makeLowercase )
		return m_customConfigs.keys() + m_staticConfigs.keys();
	
	QStringList names;
	
	ProgrammerConfigMap::const_iterator end = m_customConfigs.end();
	for ( ProgrammerConfigMap::const_iterator it = m_customConfigs.begin(); it != end; ++it )
		names << it.key().lower();
	
	end = m_staticConfigs.end();
	for ( ProgrammerConfigMap::const_iterator it = m_staticConfigs.begin(); it != end; ++it )
		names << it.key().lower();
	
	return names;
}


bool PicProgrammerSettings::isPredefined( const QString & name ) const
{
	QString l = name.lower();
	
	ProgrammerConfigMap::const_iterator end = m_staticConfigs.end();
	for ( ProgrammerConfigMap::const_iterator it = m_staticConfigs.begin(); it != end; ++it )
	{
		if ( it.key().lower() == l )
			return true;
	}
	
	return false;
}
//END class PicProgrammerSettings



//BEGIN class PicProgrammer
PicProgrammer::PicProgrammer( ProcessChain *processChain )
	: ExternalLanguage( processChain, "PicProgrammer" )
{
	m_successfulMessage = tr("*** Programming successful ***");
	m_failedMessage = tr("*** Programming failed ***");
}


PicProgrammer::~PicProgrammer()
{
}


void PicProgrammer::processInput( ProcessOptions options )
{
	resetLanguageProcess();
	m_processOptions = options;
	
	PicProgrammerSettings settings;
	settings.load(); // kapp->config() ); // does nothing
	
	QString program = options.m_program;
	if ( !settings.configNames( true ).contains( program.lower() ) )
	{
		qCritical() << "Invalid program" << endl;
		finish( false );
		return;
	}
	
	ProgrammerConfig config = settings.config( program );
	
	QString command = config.writeCommand;
	command.replace( "%port", options.m_port );
	command.replace( "%device", QString( options.m_picID ).remove("P") );
	command.replace( "%file", ( options.inputFiles().first() ) ); // FIXME KProcess::quote removed
	
	// m_languageProcess->setUseShell( true );
	*m_languageProcess << command;
	
	if ( !start() )
	{
// 		KMessageBox::sorry( LanguageManager::self()->logView(), tr("Could not program PIC.") );
		processInitFailed();
		return;
	}
}


bool PicProgrammer::isError( const QString &message ) const
{
	return message.contains( "Error", false );
}


bool PicProgrammer::isWarning( const QString &message ) const
{
	return message.contains( "Warning", false );
}


ProcessOptions::ProcessPath::Path PicProgrammer::outputPath( ProcessOptions::ProcessPath::Path inputPath ) const
{
	switch (inputPath)
	{
		case ProcessOptions::ProcessPath::PIC_AssemblyAbsolute:
		case ProcessOptions::ProcessPath::Program_PIC:
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
		case ProcessOptions::ProcessPath::Object_PIC:
		case ProcessOptions::ProcessPath::Object_Program:
		case ProcessOptions::ProcessPath::Program_Disassembly:
		case ProcessOptions::ProcessPath::Invalid:
		case ProcessOptions::ProcessPath::None:
			return ProcessOptions::ProcessPath::Invalid;
	}
	
	return ProcessOptions::ProcessPath::Invalid;
}
//END class PicProgrammer

#include "picprogrammer.moc"
