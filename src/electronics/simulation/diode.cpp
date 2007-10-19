/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
 
#include <vector>
#include "diode.h"
#include "elementset.h"
#include "matrix.h"

#include <cmath>


//BEGIN class Diode Settings
DiodeSettings::DiodeSettings()
{
	reset();
}

void DiodeSettings::reset()
{
	I_S = 1e-15;
	N = 1.0;
	V_B = 4.7;
}
//END class Diode Settings

//BEGIN class Diode
Diode::Diode()
	: NonLinear()
{
	m_numCNodes = 2;
	g_new = g_old = I_new = I_old = V_prev = 0.0;
}

Diode::~Diode()
{
}

void Diode::add_map()
{
/*
	if(!getStatus()) return;
	
	if( !p_cnode[0]->isGround) {
		p_A->setUse( p_cnode[0]->n(), p_cnode[0]->n(), Map::et_unstable, false);
	}

	if( !p_cnode[1]->isGround) {
		p_A->setUse( p_cnode[1]->n(), p_cnode[1]->n(), Map::et_unstable, false);
	}
	
	if( !p_cnode[0]->isGround && !p_cnode[1]->isGround) {
		p_A->setUse( p_cnode[0]->n(), p_cnode[1]->n(), Map::et_unstable, false);
		p_A->setUse( p_cnode[1]->n(), p_cnode[0]->n(), Map::et_unstable, false);
	}
*/
}

void Diode::add_initial_dc()
{
	g_new = g_old = I_new = I_old = V_prev = 0.0;
	update_dc();
}

double Diode::current()
{
	if(!getStatus()) return 0.0;

	double I;
	calcIg(cnodeVoltage(cnodes[0]) - cnodeVoltage(cnodes[1]), &I, 0);

	return I;
}

void Diode::updateCurrents()
{
	if(!getStatus()) return;

	m_cnodeI[1] = current();
	m_cnodeI[0] = -m_cnodeI[1];
}

void Diode::update_dc()
{
	if(!getStatus()) return;

	calc_eq();

	double tmp = g_new - g_old;

	A_g( 0, 0) += tmp;
	A_g( 1, 1) += tmp;
	A_g( 0, 1) -= tmp;
	A_g( 1, 0) -= tmp;

	tmp = I_new - I_old;

	b_i( 0) -= tmp;
	b_i( 1) += tmp;

	g_old = g_new;
	I_old = I_new;
}

#ifndef MIN
# define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

void Diode::calc_eq()
{
	double I_S = m_diodeSettings.I_S;
	double N = m_diodeSettings.N;
	double V_B = m_diodeSettings.V_B;

	double v = cnodeVoltage(0) - cnodeVoltage(1);

	// adjust voltage to help convergence
	double V_crit = diodeCriticalVoltage( I_S, N * V_T);
	if(V_B != 0 && v < MIN (0, -V_B + 10 * N * V_T)) {
		double V = -(v + V_B);
		V = diodeVoltage( V, -(V_prev + V_B), V_T * N, V_crit);
		v = -(V + V_B);
	} else	v = diodeVoltage( v, V_prev, V_T * N, V_crit);

	V_prev = v;

	double I_D;
	calcIg( v, & I_D, & g_new);

	I_new = I_D - (v * g_new);
}

void Diode::calcIg(double V, double *I_D, double *g) const
{
	double I_S = m_diodeSettings.I_S;
	double N = m_diodeSettings.N;
	double V_B = m_diodeSettings.V_B;

	double gtiny = (V < - 10 * V_T * N && V_B != 0) ? I_S : 0;

	if( V >= (-3 * N * V_T)) {
		if(g) *g = diodeConductance( V, I_S, V_T * N) + gtiny;
		*I_D = diodeCurrent( V, I_S, V_T * N) + (gtiny * V);
	} else if( V_B == 0 || V >= -V_B) {
		double a = (3 * N * V_T) / (V * M_E);
		a = a * a * a;
		*I_D = (-I_S * (1 + a)) + (gtiny * V);
		if( g) *g = ((I_S * 3 * a) / V) + gtiny;
	} else {
		double a = exp( -(V_B + V) / N / V_T);
		*I_D = (-I_S * a) + (gtiny * V);
		if( g) *g = I_S * a / V_T / N + gtiny;
	}
}

void Diode::setDiodeSettings( const DiodeSettings & settings)
{
	m_diodeSettings = settings;
	if(p_eSet) p_eSet->setCacheInvalidated();
}

//END class Diode

