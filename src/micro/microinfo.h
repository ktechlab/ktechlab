/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MICROINFO_H
#define MICROINFO_H

#include <QStringList>

class AsmInfo;
class MicroPackage;

/**
@author David Saxton
*/
class MicroInfo
{
public:
	enum Support
	{
		FullSupport	=		1 << 0,
		PartialSupport =	1 << 1,
		NoSupport =			1 << 2
	};
	enum { AllSupport = FullSupport | PartialSupport | NoSupport };
	
	MicroInfo();
	virtual ~MicroInfo();
	
	virtual AsmInfo * instructionSet() = 0;
	/**
	 * Returns the gpsim emulator support status
	 */
	virtual Support gpsimSupport() const { return NoSupport; }
	/**
	 * Returns the FlowCode support (i.e. constructing flowcode for the PIC)
	 */
	virtual Support flowcodeSupport() const { return NoSupport; }
	/**
	 * Returns the Microbe support (i.e. compiling)
	 */
	virtual Support microbeSupport() const { return NoSupport; }
	/**
	 * Returns a pointer to the Micro Package in use
	 */
	MicroPackage *package() const { return m_package; }
	/**
	 * Returns an id unique to the Micro
	 */
	QString id() const { return m_id; }
	
protected:
	QString m_id;
	MicroPackage *m_package;
};

#endif

