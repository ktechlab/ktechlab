/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VOLTAGEPOINT_H
#define VOLTAGEPOINT_H

#include "element.h"

/**
@short VoltagePoint
@author David saxton
*/
class VoltagePoint : public Element
{
public:
	VoltagePoint(const double voltage = 5);
	virtual ~VoltagePoint();
	
	virtual Type type() const { return Element_VoltagePoint; }
	void setVoltage( const double voltage );
	double voltage() { return m_voltage; }
    virtual void updateCurrents();
protected:
	virtual void add_initial_dc();
	
private:
	double m_voltage; // Conductance
};

#endif
