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
		~Gpasm();

		virtual void processInput( ProcessOptions options ) override;
		virtual ProcessOptions::ProcessPath::Path outputPath( ProcessOptions::ProcessPath::Path inputPath ) const override;

	protected:
		virtual bool isError( const QString &message ) const override;
		virtual bool isWarning( const QString &message ) const override;
};

#endif
