/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "elementset.h"
#include "inductance.h"

Inductance::Inductance( double inductance, double delta)
	: Reactive(delta)
{
	m_inductance = inductance;
	r_eq_old = v_eq_old = 0.0;
	m_numCNodes = 2;
	m_numCBranches = 1;
	setMethod( Inductance::m_euler);
}

Inductance::~Inductance()
{
}

void Inductance::setInductance( double i)
{
	m_inductance = i;
}

void Inductance::add_initial_dc()
{
	A_c( 0, 0) = 1;
	A_b( 0, 0) = 1;
	A_c( 0, 1) = -1;
	A_b( 1, 0) = -1;
	
	// The adding of r_eg and v_eq will be done for us by time_step.
	// So for now, just reset the constants used.
	r_eq_old = v_eq_old = 0.0;
}

void Inductance::updateCurrents()
{
	if(!getStatus()) return;

	m_cnodeI[0] = cbranchCurrent(0);
	m_cnodeI[1] = -m_cnodeI[0];
}

void Inductance::add_map()
{
/*
	if(!getStatus()) return;
	
	if( !p_cnode[0]->isGround) {
		p_A->setUse_c( p_cbranch[0]->n(), p_cnode[0]->n(), Map::et_constant, true);
		p_A->setUse_b( p_cnode[0]->n(), p_cbranch[0]->n(), Map::et_constant, true);
	}
	
	if( !p_cnode[1]->isGround) {
		p_A->setUse_c( p_cbranch[0]->n(), p_cnode[1]->n(), Map::et_constant, true);
		p_A->setUse_b( p_cnode[1]->n(), p_cbranch[0]->n(), Map::et_constant, true);
	}
	
	p_A->setUse_d( p_cbranch[0]->n(), p_cbranch[0]->n(), Map::et_unstable, false);
*/
}


void Inductance::time_step()
{
	if(!getStatus()) return;
	
	double i = cbranchCurrent(0);
	double v_eq_new = 0.0, r_eq_new = 0.0;
	
	if( m_method == Inductance::m_euler) {
		r_eq_new = m_inductance/m_delta;
		v_eq_new = -i*r_eq_new;
	} else if( m_method == Inductance::m_trap) {
		// TODO Implement + test trapezoidal method
		r_eq_new = 2.0*m_inductance/m_delta;
	}
	
	if( r_eq_old != r_eq_new) {
		A_d( 0, 0) -= r_eq_new - r_eq_old;
	}
	
	if( v_eq_new != v_eq_old) {
		b_v( 0) += v_eq_new - v_eq_old;
	}
	
	r_eq_old = r_eq_new;
	v_eq_old = v_eq_new;
}

bool Inductance::updateStatus()
{
	b_status = Reactive::updateStatus();
	if( m_method == Inductance::m_none)
		b_status = false;
	return getStatus();
}

void Inductance::setMethod( Method m)
{
	m_method = m;
	status_changed = true;
}

