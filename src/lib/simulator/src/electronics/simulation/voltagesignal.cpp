/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "voltagesignal.h"
#include "elementset.h"

VoltageSignal::VoltageSignal( const double delta, const double voltage )
	: Reactive::Reactive(delta)
{
	m_voltage = voltage;
	m_numCNodes = 2;
	m_numCBranches = 1;
}

VoltageSignal::~VoltageSignal()
{
}

void VoltageSignal::setVoltage( const double v )
{
	m_voltage = v;
}

void VoltageSignal::add_initial_dc()
{
	if (!b_status)
		return;
	
	A_b(0, 0) = -1;
	A_c(0, 0) = -1;
	A_b(1, 0) =  1;
	A_c(0, 1) =  1;
}

void VoltageSignal::time_step()
{
	if (!b_status) return;
	b_v(0) =  m_voltage * advance(m_delta);
}

void VoltageSignal::updateCurrents()
{
	if (!b_status) return;

	const double i = p_cbranch[0]->current();

    m_cnodeCurrent[0] = -i;
    m_cnodeCurrent[1] = i;
}

