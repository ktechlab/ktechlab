/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef GPDASM_H
#define GPDASM_H

#include <externallanguage.h>

/**
Interface to the GNU Pic Disassembler
@author David Saxton
*/
class Gpdasm : public ExternalLanguage
{
public:
	Gpdasm( ProcessChain *processChain );
	~Gpdasm();
	
	virtual void processInput( ProcessOptions options );
	virtual MessageInfo extractMessageInfo( const QString &text );
	virtual ProcessOptions::ProcessPath::Path outputPath( ProcessOptions::ProcessPath::Path inputPath ) const;

protected:
	virtual void outputtedMessage( const QString &message );
	virtual bool isError( const QString &message ) const;
	virtual bool isWarning( const QString &message ) const;
	virtual bool processExited( bool successfully );

	QString m_asmOutput; // Outputed by gpdasm
};

#endif
