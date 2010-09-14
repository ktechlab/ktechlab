/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef GPLINK_H
#define GPLINK_H

#include <externallanguage.h>

/**
@short Interface to the GNU PIC linker
@author David Saxton
*/
class Gplink : public ExternalLanguage
{
	public:
		Gplink( ProcessChain *processChain );
		~Gplink();
	
		virtual void processInput( ProcessOptions options );
		virtual MessageInfo extractMessageInfo( const QString &text );
		virtual ProcessOptions::ProcessPath::Path outputPath( ProcessOptions::ProcessPath::Path inputPath ) const;
	
	protected:
		virtual bool isError( const QString &message ) const;
		virtual bool isWarning( const QString &message ) const;
		
		QString m_sdccLibDir;
};

#endif
