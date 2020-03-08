/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"
#ifndef NO_GPSIM

#ifndef DEBUGMANAGER_H
#define DEBUGMANAGER_H

#include <qpointer.h>
#include <qmap.h>
#include <qobject.h>

class GpsimProcessor;
class TextDocument;

typedef QList< QPointer<GpsimProcessor> > GpsimProcessorList;

/**
@author David Saxton
*/
class DebugManager : public QObject
{
	Q_OBJECT
	public:
		static DebugManager * self();
		~DebugManager() override;
		
		void registerGpsim( GpsimProcessor * gpsim );
		/**
		 * Called from TextDocument when it opens a URL so that it can be
		 * connected up to any processors that refer to its url.
		 */
		void urlOpened( TextDocument * td );
		
	protected:
		GpsimProcessorList m_processors;

    public:
        DebugManager();
	private:
// 		static DebugManager * m_pSelf;
		
};

#endif

#endif
