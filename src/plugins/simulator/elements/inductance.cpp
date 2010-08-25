/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "inductance.h"

Inductance::Inductance(const double inductance, const double delta)
	: Reactive(delta)
{
	m_inductance = inductance;
	scaled_inductance = 0.0;
	m_numCNodes = 2;
	m_numCBranches = 1; // DC short-circuit.
	setMethod(Inductance::m_euler);
}

Inductance::~Inductance()
{
}

void Inductance::setInductance(double i)
{
	m_inductance = i;
}

void Inductance::add_initial_dc()
{
	A_c(0, 0) = 1;
	A_c(0, 1) = -1;

	A_b(0, 0) = 1;
	A_b(1, 0) = -1;

	A_d(0, 0) = 0.0;
	// The adding of r_eg and v_eq will be done for us by time_step.
	// So for now, just reset the constants used.
	scaled_inductance = 0.0;
}

void Inductance::updateCurrents()
{
	if (!b_status) return;

	const double i = p_cbranch[0]->current();

	p_cnode[1]->setCurrent(i);
	p_cnode[0]->setCurrent(-i);
}

void Inductance::time_step()
{
	if (!b_status) return;

	double i = p_cbranch[0]->current();
	double v_eq_new = 0.0, r_eq_new = 0.0;

	if(m_method == Inductance::m_euler) {
		r_eq_new = m_inductance / m_delta;
		v_eq_new = -i * r_eq_new;
	} else if ( m_method == Inductance::m_trap ) {
		// TODO Implement + test trapezoidal method
		r_eq_new = 2.0 * m_inductance / m_delta;
// We need to update v_eq_new here but I don't know if this is the right code.
		v_eq_new = -i * r_eq_new;
	}

	if(scaled_inductance != r_eq_new) {
		A_d(0, 0) -= r_eq_new - scaled_inductance;
	}

	b_v(0) = v_eq_new;

	scaled_inductance = r_eq_new;
}

bool Inductance::updateStatus()
{
	b_status = Reactive::updateStatus();
	if(m_method == Inductance::m_none)
		b_status = false;
	return b_status;
}

void Inductance::setMethod(Method m)
{
	m_method = m;
	updateStatus();
}

