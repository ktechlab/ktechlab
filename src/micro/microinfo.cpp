/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "microinfo.h"
#include "micropackage.h"

MicroInfo::MicroInfo()
{
    m_package = nullptr;
}

MicroInfo::~MicroInfo()
{
    delete m_package;
}

#if 0
QStringList MicroInfo::portNames()
{
	if (m_package) return m_package->portNames();
	else return "";
}

QStringList MicroInfo::pinIDs()
{
	if (m_package) return m_package->pinIDs();
	else return "";
}

QStringList MicroInfo::bidirPinIDs()
{
	if (m_package) return m_package->bidirPinIDs();
	else return "";
}

int MicroInfo::totalNumPins()
{
	if (m_package) return m_package->totalNumPins();
	else return 0;
}

int MicroInfo::numIOPins()
{
	if (m_package) return m_package->numIOPins();
	else return 0;
}

int MicroInfo::numIOPins( const QString &portName )
{
	if (m_package) return m_package->numIOPins(portName);
	else return 0;
}
#endif
