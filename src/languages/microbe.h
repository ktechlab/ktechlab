/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MICROBE_H
#define MICROBE_H

#include "externallanguage.h"

#include <Qt/qmap.h>

typedef QMap< int, QString > ErrorMap;

/**
@author Daniel Clarke
@author David Saxton
*/
class Microbe : public ExternalLanguage
{
public:
	Microbe( ProcessChain *processChain );
	~Microbe();
	
	virtual void processInput( ProcessOptions options );
	virtual ProcessOptions::ProcessPath::Path outputPath( ProcessOptions::ProcessPath::Path inputPath ) const;
	
protected:
	virtual bool isError( const QString &message ) const;
	virtual bool isWarning( const QString &message ) const;
	
	ErrorMap m_errorMessages;
};

#endif
