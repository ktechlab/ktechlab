/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef SOURCELINE_H
#define SOURCELINE_H

#include <Qt/qstring.h>

/**
@author David Saxton
 */
class SourceLine
{
	public:
		/**
		 * Creates an invalid source line (line is negative).
		 */
		SourceLine();
		SourceLine( const QString & fileName, int line );
		
		QString fileName() const { return m_fileName; }
		int line() const { return m_line; }
		
		bool isValid() const { return m_line >= 0; }
		
		bool operator < ( const SourceLine & sourceLine ) const;
		bool operator == ( const SourceLine & sourceLine ) const;
		
	protected:
		QString m_fileName;
		int m_line;
};


#endif
