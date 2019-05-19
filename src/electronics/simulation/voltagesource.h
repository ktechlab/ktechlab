/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VOLTAGESOURCE_H
#define VOLTAGESOURCE_H

#include "element.h"

/**
CNode n0 is the negative terminal, CNode n1 is the positive terminal
@short Voltage Source
*/
class VoltageSource : public Element
{
public:
	VoltageSource( const double voltage );
	virtual ~VoltageSource();

	virtual Type type() const override { return Element_VoltageSource; }
	void setVoltage( const double v );

protected:
	virtual void updateCurrents() override;
	virtual void add_initial_dc() override;

private:
	double m_v; // Voltage
};

#endif
