/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <qobject.h>
#include <qstringlist.h>

class FlowCodeDocument;
class KTechlab;
class LogView;
class MessageInfo;
class MicroSettings;
class OutputMethodInfo;
class ProcessChain;
class ProcessOptions;
class TextDocument;
class QProcess;

typedef QList<ProcessOptions> ProcessOptionsList;

class ProcessOptionsSpecial
{
	public:
		ProcessOptionsSpecial();
		
		bool b_addToProject;
		bool b_forceList;
		QString m_picID;
		FlowCodeDocument * p_flowCodeDocument;
		
		// Linking
		QString m_hexFormat;
		bool m_bOutputMapFile;
		QString m_libraryDir;
		QString m_linkerScript;
		QStringList m_linkLibraries;
		QString m_linkOther;
		
		// Programming
		QString m_port;
		QString m_program;
};


class ProcessOptionsHelper : public QObject
{
	Q_OBJECT
#define protected public
	signals:
#undef protected
		void textOutputtedTo( TextDocument * outputtedTo );
};


class ProcessOptions : public ProcessOptionsSpecial
{
	public:
		ProcessOptions();
		ProcessOptions( OutputMethodInfo info );
		
		class ProcessPath { public:
			enum MediaType
			{
				AssemblyAbsolute,
				AssemblyRelocatable,
				C,
				Disassembly,
				FlowCode,
				Library,
				Microbe,
				Object,
				Pic,
				Program,
				
				Unknown // Used for guessing the media type
			};
				
			enum Path // From_To				// processor that will be invoked first
			{
				AssemblyAbsolute_PIC,			// gpasm (indirect)
				AssemblyAbsolute_Program,		// gpasm (direct)
				
				AssemblyRelocatable_Library,	// gpasm (indirect)
				AssemblyRelocatable_Object,		// gpasm (direct)
				AssemblyRelocatable_PIC,		// gpasm (indirect)
				AssemblyRelocatable_Program,	// gpasm (indirect)
				                                          
				C_AssemblyRelocatable,			// sdcc (direct)
				C_Library,						// sdcc (indirect)
				C_Object,						// sdcc (indirect)
				C_PIC,							// sdcc (indirect)
				C_Program,						// sdcc (indirect)
				
				FlowCode_AssemblyAbsolute,		// flowcode (indirect)
				FlowCode_Microbe,				// flowcode (direct)
				FlowCode_PIC,					// flowcode (indirect)
				FlowCode_Program,				// flowcode (indirect)
				
				Microbe_AssemblyAbsolute,		// microbe (direct)
				Microbe_PIC,					// microbe (indirect)
				Microbe_Program,				// microbe (indirect)
				
				Object_Disassembly,				// gpdasm (direct)
				Object_Library,					// gplib (direct)
				Object_PIC,						// gplink (indirect)
				Object_Program,					// gplink (direct)
				
				PIC_AssemblyAbsolute,			// download from pic (direct)
				
				Program_Disassembly,			// gpdasm (direct)
				Program_PIC,					// upload to pic (direct)
				
				Invalid, // From and to types are incompatible
				None // From and to types are the same
			};
				
			static Path path( MediaType from, MediaType to );
			static MediaType from( Path path );
			static MediaType to( Path path );
		};
		
		class Method
		{
			public: enum type
			{
				Forget, // Don't do anything after processing successfully
				LoadAsNew, // Load the output as a new file
				Load // Load the output file
			};
		};
	
		/**
		 * Tries to guess the media type from the url (and possible the contents
		 * of the file as well).
		 */
		static ProcessPath::MediaType guessMediaType( const QString & url );
		/**
		 * The *final* target file (not any intermediatary ones)
		 */
		QString targetFile() const { return m_targetFile; }
		/**
		 * This sets the final target file, as well as the initial intermediatary one
		 */
		void setTargetFile( const QString &file );
		
		void setIntermediaryOutput( const QString &file ) { m_intermediaryFile = file; }
		QString intermediaryOutput() const { return m_intermediaryFile; }
		
		void setInputFiles( const QStringList & files ) { m_inputFiles = files; }
		QStringList inputFiles() const { return m_inputFiles; }
		
		void setMethod( Method::type method ) { m_method = method; }
		Method::type method() const { return m_method; }
		
		void setProcessPath( ProcessPath::Path path ) { m_processPath = path; }
		ProcessPath::Path processPath() const { return m_processPath; }
	
		/**
		 * If the output is text; If the user has selected (in config options)
		 * ReuseSameViewForOutput, then the given TextDocument will have its
		 * text set to the output if the TextDocument is not modified and has
		 * an empty url. Otherwise a new TextDocument will be created. Either
		 * way, once the processing has finished, a signal will be emitted
		 * to the given receiver passing a TextDocument * as an argument. This
		 * is not to be confused with setTextOutputtedTo, which is called once
		 * the processing has finished, and will call-back to the slot given.
		 */
		void setTextOutputTarget( TextDocument * target, QObject * receiver, const char * slot );
		/**
		 * @see setTextOutputTarget
		 */
		TextDocument * textOutputTarget() const { return m_pTextOutputTarget; }
		/**
		 * @see setTextOuputTarget
		 */
		void setTextOutputtedTo( TextDocument * outputtedTo );
		
	protected:
		TextDocument * m_pTextOutputTarget;
		ProcessOptionsHelper * m_pHelper;
		bool b_targetFileSet;
		QStringList m_inputFiles;
		QString m_targetFile;
		QString m_intermediaryFile;
		Method::type m_method;
		ProcessPath::Path m_processPath;
};


/**
@author Daniel Clarke
@author David Saxton
*/
class Language : public QObject
{
	Q_OBJECT
	public:
		Language( ProcessChain *processChain, const QString &name );
		~Language() override;
	
		/**
		 * Compile / assemble / dissassembly / whatever the given input.
		 */
		virtual void processInput( ProcessOptions options ) = 0;
		/**
		 * Return the ProcessOptions object current state
		 */
		ProcessOptions processOptions() const { return m_processOptions; }
		/**
		 * Return the output path from the given input path. Will return None
		 * if we've done processing.
		 */
		virtual ProcessOptions::ProcessPath::Path outputPath( ProcessOptions::ProcessPath::Path inputPath ) const = 0;
	
	signals:
		/**
		 * Emitted when the processing was successful.
		 * @param language Pointer to this class
		 */
		void processSucceeded( Language *language );
		/**
		 * Emitted when the processing failed.
		 * @param language Pointer to this class
		 */
		void processFailed( Language *language );
	
	protected:
		/**
		 * Examines the string for the line number if applicable, and creates a new
		 * MessageInfo for it.
		 */
		virtual MessageInfo extractMessageInfo( const QString &text );
	
		/**
		 * Reset the error count
		 */
		void reset();
		void outputMessage( const QString &message );
		void outputWarning( const QString &message );
		void outputError( const QString &error );
		void finish( bool successful );
	
		int m_errorCount;
		ProcessOptions m_processOptions;
		ProcessChain *p_processChain;
	
		/**
		 * A message appropriate to the language's success after compilation or similar.
		 */
		QString m_successfulMessage;
		/**
		 * A message appropriate to the language's failure after compilation or similar.
		 */
		QString m_failedMessage;
};

#endif
