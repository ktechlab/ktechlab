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
	VoltagePoint( const double voltage );
	virtual ~VoltagePoint();

	virtual Type type() const override { return Element_VoltagePoint; }
	void setVoltage( const double voltage );
	double voltage() { return m_voltage; }
protected:
	virtual void updateCurrents() override;
	virtual void add_initial_dc() override;

private:
	double m_voltage; // Conductance
};

#endif
