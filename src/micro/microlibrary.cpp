/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "microinfo.h"
#include "microlibrary.h"

// #include <QDebug>
// #include <k3staticdeleter.h>

#include "picinfo12bit.h"
#include "picinfo14bit.h"
#include "picinfo16bit.h"

#include "micropackage.h"

#include <QGlobalStatic>

// MicroLibrary * MicroLibrary::m_pSelf = nullptr;
// static K3StaticDeleter<MicroLibrary> staticMicroLibraryDeleter;

Q_GLOBAL_STATIC( MicroLibrary, globalMicroLibrary);

MicroLibrary * MicroLibrary::self()
{
    return globalMicroLibrary;
// 	if ( !m_pSelf )
// 		staticMicroLibraryDeleter.setObject( m_pSelf, new MicroLibrary() );
// 	return m_pSelf;
}

MicroLibrary::MicroLibrary()
{
	addMicroInfo( new PicInfo12C508() );
	addMicroInfo( new PicInfo12C509() );
	addMicroInfo( new PicInfo16C54 () );
	addMicroInfo( new PicInfo16C55() );
	addMicroInfo( new PicInfo16C61() );
	addMicroInfo( new PicInfo16C62() );
	addMicroInfo( new PicInfo16C63() );
	addMicroInfo( new PicInfo16C64() );
	addMicroInfo( new PicInfo16F627() );
	addMicroInfo( new PicInfo16F628() );
	addMicroInfo( new PicInfo16C65() );
	addMicroInfo( new PicInfo16C71() );
	addMicroInfo( new PicInfo16C72() );
	addMicroInfo( new PicInfo16C73() );
	addMicroInfo( new PicInfo16C712() );
	addMicroInfo( new PicInfo16C716() );
	addMicroInfo( new PicInfo16C74() );
	addMicroInfo( new PicInfo16C84() );
	addMicroInfo( new PicInfo16CR83() );
	addMicroInfo( new PicInfo16F83() );
	addMicroInfo( new PicInfo16CR84() );
	addMicroInfo( new PicInfo16F84() );
	addMicroInfo( new PicInfo16F873() );
	addMicroInfo( new PicInfo16F874() );
	addMicroInfo( new PicInfo16F877() );
	addMicroInfo( new PicInfo17C752() );
	addMicroInfo( new PicInfo17C756() );
	addMicroInfo( new PicInfo17C762() );
	addMicroInfo( new PicInfo17C766() );
	addMicroInfo( new PicInfo18C242() );
	addMicroInfo( new PicInfo18C252() );
	addMicroInfo( new PicInfo18C442() );
	addMicroInfo( new PicInfo18C452() );
	addMicroInfo( new PicInfo18F442() );
	addMicroInfo( new PicInfo18F452() );
}

MicroLibrary::~MicroLibrary()
{
	const MicroInfoList::iterator end = m_microInfoList.end();
	for ( MicroInfoList::iterator it = m_microInfoList.begin(); it != end; ++it ) {
		delete *it;
	}
}

MicroInfo *MicroLibrary::microInfoWithID( QString id )
{
	id = id.toUpper();
	const MicroInfoList::iterator end = m_microInfoList.end();
	for ( MicroInfoList::iterator it = m_microInfoList.begin(); it != end; ++it )
	{
		if ( (*it)->id() == id ) return *it;
	}

	return nullptr;
}

void MicroLibrary::addMicroInfo( MicroInfo *microInfo )
{
	if (microInfo)
		m_microInfoList += microInfo;
}

QStringList MicroLibrary::microIDs( unsigned asmSet, unsigned gpsimSupport, unsigned flowCodeSupport, unsigned microbeSupport )
{
	QStringList ids;

	const MicroInfoList::iterator end = m_microInfoList.end();
	for ( MicroInfoList::iterator it = m_microInfoList.begin(); it != end; ++it )
	{
		MicroInfo * info = *it;
		if ( (info->instructionSet()->set() & asmSet) &&
					(info->gpsimSupport() & gpsimSupport) &&
					(info->flowcodeSupport() & flowCodeSupport) &&
					(info->microbeSupport() & microbeSupport) )
			ids << info->id();
	}
	return ids;
}
