/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "voltagesource.h"
#include "elementset.h"

VoltageSource::VoltageSource(const double voltage)
		: Element::Element() {
	m_v = voltage;
	m_numCBranches = 1;
	m_numCNodes = 2;
}

VoltageSource::~VoltageSource() {
}

void VoltageSource::setVoltage(const double v) {
	if (m_v == v) return;

	if (p_eSet)
		p_eSet->setCacheInvalidated();

	m_v = v;

	add_initial_dc();
}

/* FIXME:
	Often times, you want to use one of these to set a voltage in a circuit but not pass
current. This creates a singular matrix. Depending on how Matrix is set up, it might either cause the
circuit to glitch out or will create a virtual one ohm resistor either into a component that shouldn't be conducting and draw/source that current from wherever it is avaliable. =P

VoltageSignal also exhibits this problem.
*/
void VoltageSource::add_initial_dc() {
	if (!b_status)
		return;

	A_b(0, 0) = -1;
	A_c(0, 0) = -1;
	A_b(1, 0) = 1;
	A_c(0, 1) = 1;

	b_v(0) = m_v;
}

void VoltageSource::updateCurrents() {
	if (!b_status) return;

	double i = p_cbranch[0]->current();

    m_cnodeCurrent[0] = -i;
    m_cnodeCurrent[1] = i;
}

