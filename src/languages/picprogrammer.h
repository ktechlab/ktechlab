/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PICPROGRAMMER_H
#define PICPROGRAMMER_H

#include "externallanguage.h"

#include <qmap.h>

class KConfig;
class KProcess;


class ProgrammerConfig
{
	public:
		ProgrammerConfig();
		
		/**
		 * Clears the type and all commands.
		 */
		void reset();
		
		QString initCommand;
		QString readCommand;
		QString writeCommand;
		QString verifyCommand;
		QString blankCheckCommand;
		QString eraseCommand;
		
		QString description;
		QString executable; // The name of the program executable
};

typedef QMap< QString, ProgrammerConfig > ProgrammerConfigMap;



/**
This class provides access to the PIC Programmer configurations. Several are
predefinied; the rest can be read from and written to, and removed. Names are
case insensitive.

Each programmer configuration is in the form of the ProgrammerConfig struct.

@author David Saxton
*/
class PicProgrammerSettings
{
	public:
		PicProgrammerSettings();
		
		/**
		 * Reads in custom ProgrammerConfigs from config. Any previously loaded
		 * configurations stored in this class will removed first.
		 */
		void load( KConfig * config );
		/**
		 * Saves the custom ProgrammConfigs to config.
		 */
		void save( KConfig * config );
		/**
		 * @return the ProgrammConfig for the programmer with the given name. If
		 * no such ProgrammerConfigs with the given name exist, then one will be
		 * created. The name is case insensitive (although the full case of the
		 * name will be stored if a new ProgrammerConfig is created).
		 */
		ProgrammerConfig config( const QString & name );
		/**
		 * Removes the config (if it is custom) with the give name.
		 */
		void removeConfig( const QString & name );
		/**
		 * Sets the ProgrammerConfig with the given name (or creates one if no
		 * such config exists). The name is case insensitive.
		 */
		void saveConfig( const QString & name, const ProgrammerConfig & config );
		/**
		 * @param makeLowercase whether the names should be converted to
		 * lowercase before returning.
		 * @return a list of names of the custom and predefined configs.
		 */
		QStringList configNames( bool makeLowercase ) const;
		/**
		 * @return whether the given config is predefined.
		 */
		bool isPredefined( const QString & name ) const;
		
	protected:
		/**
		 * Called when a PicProgrammerSettings object is first created. Does
		 * initialization of the predefined configs.
		 */
		void initStaticConfigs();
		
		ProgrammerConfigMap m_customConfigs;
		
		static bool m_bDoneStaticConfigsInit;
		static ProgrammerConfigMap m_staticConfigs;
};


/**
@author David Saxton
*/
class PicProgrammer : public ExternalLanguage
{
	public:
		PicProgrammer( ProcessChain *processChain );
		~PicProgrammer() override;
	
		void processInput( ProcessOptions options ) override;
		ProcessOptions::ProcessPath::Path outputPath( ProcessOptions::ProcessPath::Path inputPath ) const override;

	protected:
		bool isError( const QString &message ) const override;
		bool isWarning( const QString &message ) const override;
};

#endif
