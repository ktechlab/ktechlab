/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "sourceline.h"


//BEGIN class SourceLine
SourceLine::SourceLine()
	: m_line(-1)
{
}


SourceLine::SourceLine( const QString & fileName, int line )
	: m_fileName(fileName),
	m_line(line)
{
}


bool SourceLine::operator < ( const SourceLine & sourceLine ) const
{
	return (m_fileName < sourceLine.fileName()) ||
			(m_fileName == sourceLine.fileName() && m_line < sourceLine.line());
}


bool SourceLine::operator == ( const SourceLine & sourceLine ) const
{
	return (sourceLine.fileName() == fileName()) &&
			(sourceLine.line() == line());
}
//END class SourceLine


