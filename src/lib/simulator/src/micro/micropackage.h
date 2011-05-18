/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PICPACKAGES_H
#define PICPACKAGES_H

#include <qstring.h>
#include <qstringlist.h>

#include <qmap.h>

/**
@author David Saxton
*/
class PicPin
{
public:
	enum pin_type
	{
		type_input	= 0x1, // Input only pin
		type_bidir	= 0x2, // Bi-directional (input/output)
		type_open	= 0x4, // Open collector
		type_vss	= 0x8, // Voltage source
		type_vdd	= 0x10, // Voltage drain
		type_mclr	= 0x20, // Memory clear
		type_osc	= 0x40 // Oscillator
	};
	
	PicPin();
	PicPin( const QString &_pinID, PicPin::pin_type _type, const QString &_portName = "", int _portPosition = -1 );

	PicPin::pin_type type;
	
	QString pinID; // Id of pin, eg 'MCLR'
	
	// For bidir (io) pins
	QString portName; // Name of port, eg 'PORTA'
	int portPosition; // Position in port
};

typedef QMap<int, PicPin> PicPinMap;

/**
@short Describes the PIC package (i.e. pins)
@author David Saxton
*/
class MicroPackage
{
public:
	MicroPackage( const int pinCount );
	virtual ~MicroPackage();
	
	/**
	 * Assigns a pin to a position in the package.
	 */
	void assignPin( int pinPosition, PicPin::pin_type type, const QString& pinID, const QString& portName = "", int portPosition = -1);
	/**
	 * Returns the pins of the given type(s). If portName is not specified, all pins will be returned;
	 * not just those belonging to a given port. pin_type's can be OR'ed together
	 * e.g. pins( PicPin::type_input | PicPin::type_bidir, "PORTA" ) will return all bidirectional or
	 * input pins belonging to PORTA. If pinType is 0, then this will return all pins
	 */
	PicPinMap pins( uint pinType = 0, const QString& portName = "" );
	/**
	 * Returns just a QStringList of the pin ids.
	 * @see pins( uint pinType, const QString& portName )
	 */
	QStringList pinIDs( uint pinType = 0, const QString& portName = "" );
	/**
	 * Returns the number of pins of the given type(s) (which can be OR'ed together), with the given
	 * port name if specified, while avoiding the construction of a new PicPinMap. if pinType is 0,
	 * then all pin types are considered
	 * @see pins
	 */
	int pinCount( uint pinType = 0, const QString& portName = "" );
	/**
	 * Returns a list of port names, eg 'PORTA', 'PORTB' for the package
	 */
	QStringList portNames() const { return m_portNames; }
	/**
	 * Returns the number of ports
	 */
	uint portCount() const { return m_portNames.size(); }
	
private:
	PicPinMap m_picPinMap;
	QStringList m_portNames;
	int m_numPins;
};

#endif


