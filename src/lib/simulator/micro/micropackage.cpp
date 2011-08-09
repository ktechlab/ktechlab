/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "micropackage.h"

#include <qdebug.h>

PicPin::PicPin()
{
	pinID = "INVALID";
	type = PicPin::type_bidir;
	portName = "INVALID";
	portPosition = -1;
}


PicPin::PicPin( const QString &_pinID, PicPin::pin_type _type, const QString &_portName, int _portPosition )
{
	pinID = _pinID;
	type = _type;
	portName = _portName;
	portPosition = _portPosition;
}

MicroPackage::MicroPackage( const int pinCount )
{
	m_numPins = pinCount;
}

MicroPackage::~MicroPackage()
{
}

void MicroPackage::assignPin( int pinPosition, PicPin::pin_type type, const QString& pinID, const QString& portName, int portPosition )
{
	if ( m_picPinMap.find(pinPosition) != m_picPinMap.end() )
	{
		qCritical() << "PicDevice::assignBidirPin: Attempting to reset pin "<<pinPosition<<endl;
		return;
	}
	if ( !m_portNames.contains(portName) && !portName.isEmpty() )
	{
		m_portNames.append(portName);
		m_portNames.sort();
	}
	
	m_picPinMap[pinPosition] = PicPin( pinID, type, portName, portPosition );
		
}

PicPinMap MicroPackage::pins( uint pinType, const QString& portName )
{
	if ( pinType == 0 ) pinType = (1<<30)-1;
	
	PicPinMap list;
	
	const PicPinMap::iterator picPinSetEnd = m_picPinMap.end();
	for ( PicPinMap::iterator it = m_picPinMap.begin(); it != picPinSetEnd; ++it )
	{
		if ( (it.value().type & pinType) &&
					(portName.isEmpty() || it.value().portName == portName) )
		{
			list[it.key()] = it.value();
		}
	}
	
	return list;
}

QStringList MicroPackage::pinIDs( uint pinType, const QString& portName )
{
	if ( pinType == 0 ) pinType = (1<<30)-1;
	QStringList list;

	const PicPinMap::iterator picPinSetEnd = m_picPinMap.end();
	for ( PicPinMap::iterator it = m_picPinMap.begin(); it != picPinSetEnd; ++it )
	{
		if ( (it.value().type & pinType) &&
					(portName.isEmpty() || it.value().portName == portName) )
		{
			list.append( it.value().pinID );
		}
	}
	
	return list;
}

int MicroPackage::pinCount( uint pinType, const QString& portName )
{
	if ( pinType == 0 ) pinType = (1<<30)-1;
	int count = 0;
	
	const PicPinMap::iterator picPinSetEnd = m_picPinMap.end();
	for ( PicPinMap::iterator it = m_picPinMap.begin(); it != picPinSetEnd; ++it )
	{
		if ( (it.value().type & pinType) &&
					(portName.isEmpty() || it.value().portName == portName) ) count++;
	}
	
	return count;
}

