/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ASMPARSER_H
#define ASMPARSER_H

#include <qstring.h>

class GpsimDebugger;

/**
Reads in an assembly file, and extracts useful information from it, such as the
PIC ID

@author David Saxton
*/
class AsmParser
{
	public:
		/// @param url a path to a file in the local filesystem
		AsmParser( const QString &url );
		~AsmParser();
		
		enum Type { Relocatable, Absolute };
		
		/**
		 * Read in data from file, return success status.
		 * @param debugger if this is non-null, then source-line markers read
		 * from the assembly file (such as those beginning with ";#CSRC" will be
		 * passed to hllDebugger).
		 */
		bool parse( GpsimDebugger * debugger = nullptr );
		/**
		 * Returns the PIC ID
		 */
		QString picID() const { return m_picID; }
		/**
		 * Returns whether or not the assembly file contained the "set radix"
		 * directive
		 */
		bool containsRadix() const { return m_bContainsRadix; }
		/**
		 * If the assembly file contains any of several key words that identify
		 * it as a relocatable object, then this will return Relocatable.
		 */
		Type type() const { return m_type; }
		
	protected:
		const QString m_url;
		QString m_picID;
		bool m_bContainsRadix;
		Type m_type;
};

#endif
