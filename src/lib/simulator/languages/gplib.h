/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef GPLIB_H
#define GPLIB_H

#include <externallanguage.h>

/**
@author David Saxton
*/
class Gplib : public ExternalLanguage
{
	public:
		Gplib( ProcessChain *processChain );
		~Gplib();
	
		virtual void processInput( ProcessOptions options );
		virtual MessageInfo extractMessageInfo( const QString &text );
		virtual ProcessOptions::ProcessPath::Path outputPath( ProcessOptions::ProcessPath::Path inputPath ) const;
	
	protected:
		virtual bool isError( const QString &message ) const;
		virtual bool isWarning( const QString &message ) const;
};

#endif
