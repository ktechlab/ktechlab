/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "capacitance.h"

Capacitance::Capacitance( const double capacitance, const double delta)
	: Reactive(delta), m_cap(capacitance)
{
	g_eq_old = i_eq_old = 0.;
	m_numCNodes = 2;
	setMethod( Capacitance::m_euler);
}

Capacitance::~Capacitance()
{}

void Capacitance::setCapacitance( const double c)
{
	m_cap = c;
}

void Capacitance::add_initial_dc()
{
	// We don't need to do anything here, as time_step() will do that for us,
	// apart from to make sure our old values are 0
	g_eq_old = i_eq_old = 0.;
}

void Capacitance::updateCurrents()
{
	if(!getStatus()) return;
	const double r_i = (cnodeVoltage(0)-cnodeVoltage(1))*g_eq_old;
	m_cnodeI[0] = -i_eq_old-r_i;
	m_cnodeI[1] = -m_cnodeI[0];
}

void Capacitance::add_map()
{
/*
	if(!getStatus()) return;
	
	if( !p_cnode[0]->isGround)
	{
		p_A->setUse( p_cnode[0]->n(), p_cnode[0]->n(), Map::et_unstable, false);
	}
	if( !p_cnode[1]->isGround)
	{
		p_A->setUse( p_cnode[1]->n(), p_cnode[1]->n(), Map::et_unstable, false);
	}
	
	if( !p_cnode[0]->isGround && !p_cnode[1]->isGround)
	{
		p_A->setUse( p_cnode[0]->n(), p_cnode[1]->n(), Map::et_unstable, false);
		p_A->setUse( p_cnode[1]->n(), p_cnode[0]->n(), Map::et_unstable, false);
	}
*/
}

void Capacitance::time_step()
{
	if(!getStatus()) return;

	double v = cnodeVoltage(0) - cnodeVoltage(1);
	double i_eq_new = 0.0, g_eq_new = 0.0;

	if( m_method == Capacitance::m_euler) {
		g_eq_new = m_cap/m_delta;
		i_eq_new = -v*g_eq_new;
	} else if( m_method == Capacitance::m_trap) {
		// TODO Implement + test trapezoidal method
		g_eq_new = 2.*m_cap/m_delta;
	}

	if( g_eq_old != g_eq_new) {
		double foo = g_eq_new-g_eq_old;
		A_g( 0, 0) += foo;
		A_g( 1, 1) += foo;
		A_g( 0, 1) -= foo;
		A_g( 1, 0) -= foo;
	}

	if(i_eq_new != i_eq_old) {
		double foo = i_eq_new-i_eq_old;
		b_i( 0) -= foo;
		b_i( 1) += foo;
	}

	g_eq_old = g_eq_new;
	i_eq_old = i_eq_new;
}

bool Capacitance::updateStatus()
{
	Reactive::updateStatus();
	if(m_method == Capacitance::m_none) b_status = false;
	return b_status;
}

void Capacitance::setMethod(Method m)
{
	m_method = m;
	status_changed = true;
}

