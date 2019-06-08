/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VOLTAGESIGNAL_H
#define VOLTAGESIGNAL_H

#include "reactive.h"
#include "elementsignal.h"

/**
@short VoltageSignal
@author David saxton
*/
class VoltageSignal : public Reactive, public ElementSignal
{
public:
	VoltageSignal( const double delta, const double voltage );
	~VoltageSignal() override;

	Element::Type type() const override { return Element_VoltageSignal; }
	void setVoltage( const double voltage );
	double voltage() { return m_voltage; }
	void time_step() override;

protected:
	void updateCurrents() override;
	void add_initial_dc() override;

private:
	double m_voltage; // Voltage
};

#endif
