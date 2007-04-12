/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MICRoLIBRARY_H
#define PICLIBRARY_H

#include "asminfo.h"
#include "microinfo.h"

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>

class MicroInfo;
class MicroLibrary;
typedef QValueList<MicroInfo*> MicroInfoList;

inline MicroLibrary *microLibrary();

/**
@short Stores all the avaiable PICs (info)
@author David Saxton
*/
class MicroLibrary
{
	public:
		static MicroLibrary * self();
		
		~MicroLibrary();
	
		MicroInfo * const microInfoWithID( QString id );
		void addMicroInfo( MicroInfo *microInfo );
		
		/**
		 * Returns a list of micro ids with the given properties (OR'ed
		 * together).
		 */
		QStringList microIDs( unsigned asmSet = AsmInfo::AsmSetAll, unsigned gpsimSupport = MicroInfo::AllSupport, unsigned flowCodeSupport = MicroInfo::AllSupport, unsigned microbeSupport = MicroInfo::AllSupport );

	private:
		MicroLibrary();
		static MicroLibrary * m_pSelf;
	
		MicroInfoList m_microInfoList;
		friend MicroLibrary *microLibrary();
};

#endif
