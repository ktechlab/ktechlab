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
		~Gplink() override;
	
		void processInput( ProcessOptions options ) override;
		MessageInfo extractMessageInfo( const QString &text ) override;
		ProcessOptions::ProcessPath::Path outputPath( ProcessOptions::ProcessPath::Path inputPath ) const override;

	protected:
		bool isError( const QString &message ) const override;
		bool isWarning( const QString &message ) const override;

		QString m_sdccLibDir;
};

#endif
