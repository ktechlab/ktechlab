/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef GPASM_H
#define GPASM_H

#include "externallanguage.h"

/**
@short Interface to the GNU PIC assembler
@author David Saxton
*/
class Gpasm : public ExternalLanguage
{
	public:
		Gpasm( ProcessChain *processChain );
		~Gpasm() override;
	
		void processInput( ProcessOptions options ) override;
		ProcessOptions::ProcessPath::Path outputPath( ProcessOptions::ProcessPath::Path inputPath ) const override;
	
	protected:
		bool isError( const QString &message ) const override;
		bool isWarning( const QString &message ) const override;
};

#endif
