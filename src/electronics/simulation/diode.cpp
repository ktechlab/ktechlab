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

#include <cmath>

//BEGIN class Diode Settings
DiodeSettings::DiodeSettings() {
	reset();
}

void DiodeSettings::reset() {
	I_S = 1e-15;
	N = 1.0;
	V_B = 4.7;
}
//END class Diode Settings

//BEGIN class Diode
Diode::Diode()
		: NonLinear() {
	m_numCNodes = 2;
	g_old = I_old = 0.0;

	updateLim();
}

Diode::~Diode() {
}

void Diode::add_initial_dc() {
	g_old = I_old = 0.0;

	update_dc();
}

void Diode::updateCurrents() {
	p_cnode[0]->setCurrent(-I_old);
	p_cnode[1]->setCurrent( I_old);
}

void Diode::update_dc() {
	if (!b_status) return;

	double g_new, I_new;
	calc_eq(&g_new, &I_new);

	double tmp = g_new - g_old;

	A_g(0, 0) += tmp;
	A_g(1, 1) += tmp;
	A_g(0, 1) -= tmp;
	A_g(1, 0) -= tmp;

	tmp = I_new - I_old;

	b_i(0) += tmp;
	b_i(1) -= tmp;

	g_old = g_new;
	I_old = I_new;
}

void Diode::calc_eq(double *g_new, double *I_new) {
//	double N = m_diodeSettings.N;
//	double V_B = m_diodeSettings.V_B;

	double v = p_cnode[0]->voltage() - p_cnode[1]->voltage();

	v_prev = v = diodeVoltage(v, v_prev, m_diodeSettings.N, v_lim);

	calcIg(v, I_new, g_new);

	*I_new = -*I_new + (v * *g_new);
}

void Diode::calcIg(double V, double *I_D, double *g) const {
	double I_S = m_diodeSettings.I_S;
	double N   = m_diodeSettings.N;
	double V_B = m_diodeSettings.V_B;

	double g_tiny = (V < -10 * V_T * N) ? I_S : 0;

	if (V_B == 0 || V >= -V_B) {
		// diode is operating normally.
		diodeJunction(V, I_S, N, I_D, g);
		*g += g_tiny;
		*I_D += g_tiny * V;
	} else {
		// diode is in reverse breakdown.
		double a = exp(-(V_B + V) / N / V_T);
		*I_D = (-I_S * a) + (g_tiny * V);
		*g   = I_S * a / V_T / N + g_tiny;
	}
}

void Diode::setDiodeSettings(const DiodeSettings &settings) {
	m_diodeSettings = settings;
	updateLim();

	if (p_eSet) p_eSet->setCacheInvalidated();
}

void Diode::updateLim() {
	double I_S = m_diodeSettings.I_S;
	double N = m_diodeSettings.N;

	v_lim = diodeLimitedVoltage(I_S, N);
}
//END class Diode

