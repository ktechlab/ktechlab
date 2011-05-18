/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mosfet.h"

#include <cmath>

using namespace std;

#define BULK_THRESHOLD  0.37
#define BULK_JUNCTION_POTENTIAL 0.87

//BEGIN class MOSFETSettings
MOSFETSettings::MOSFETSettings() {
	I_S = 1e-14;
	N = 1.0;
	K_P = 6e-5;
	L = 1e-4;
	W = 1e-4;

#if 0
	phi = 0.6;
	T_OX = 1e-7;
	P_b = 0.8;
	M_J = 0.5;
	F_C = 0.5;
	M_JSW = 0.33;
	U_0 = 600.0;
	N_RD = 1;
	N_RS = 1;
#endif
}
//END class MOSFETSettings

//BEGIN class MOSFETState
MOSFETState::MOSFETState() {
	reset();
}

void MOSFETState::reset() {
	for(unsigned i = 0; i < 4; ++i) {
		for(unsigned j = 0; j < 4; ++j)
			A[i][j] = 0.0;

		I[i] = 0.0;
	}
}

MOSFETState MOSFETState::operator-(const MOSFETState &s) const {
	MOSFETState newState(*this);

	for(unsigned i = 0; i < 4; ++i) {
		for(unsigned j = 0; j < 4; ++j)
			newState.A[i][j] -= s.A[i][j];

		newState.I[i] -= s.I[i];
	}

	return newState;
}
//END class MOSFETState

//BEGIN class MOSFET
const uint32_t MOSFET::PinD = 0;
const uint32_t MOSFET::PinG = 1;
const uint32_t MOSFET::PinS = 2;
const uint32_t MOSFET::PinB = 3;

MOSFET::MOSFET(MOSFET_type type) {
	m_pol = 10;

	switch (type) {

	case neMOSFET:
// 	case ndMOSFET:
		m_pol = 1;
		break;

	case peMOSFET:
// 	case pdMOSFET:
		m_pol = -1;
		break;
	}

	m_numCNodes = 4;
	updateLim();
}

MOSFET::~MOSFET() {
}

void MOSFET::add_initial_dc() {
	m_os.reset();
	update_dc();
}

void MOSFET::updateCurrents() {
    m_cnodeCurrent[PinD] = -I_D  + I_BD;
    m_cnodeCurrent[PinB] = -(I_BD + I_BS);
    m_cnodeCurrent[PinS] = I_D  + I_BS;
}

void MOSFET::update_dc() {
	if (!b_status) return;

	calc_eq();

	MOSFETState diff = m_ns - m_os;

	for (unsigned i = 0; i < 4; ++i) {
		for (unsigned j = 0 ; j < 4; ++j)
			A_g(i, j) += diff.A[i][j];

		b_i(i) += diff.I[i];
	}

	m_os = m_ns;
}

void MOSFET::calc_eq() {
//	double N = m_mosfetSettings.N;

	double V_GS, V_DS, V_BS;

	{
		const double V_D = p_cnode[PinD]->voltage();
		const double V_G = p_cnode[PinG]->voltage();
		const double V_S = p_cnode[PinS]->voltage();
		const double V_B = p_cnode[PinB]->voltage();

// convert useless circuit voltages into component voltages. 
		V_GS = (V_G - V_S) * m_pol;
		V_DS = (V_D - V_S) * m_pol;
		V_BS = (V_B - V_S) * m_pol;
	}

//*************************
	double g_BS, g_BD, g_DS, g_M, g_mb;
	calcIg(V_BS, V_DS, V_GS,
	       &I_BS, &I_BD, &I_D,
	       &g_BS, &g_BD, &g_DS,
	       &g_M, &g_mb);
//************************

	double I_BD_eq = I_BD - g_BD * (V_BS - V_DS);
	double I_BS_eq = I_BS - g_BS * V_BS;
	double I_DS_eq = I_D - g_M  * V_GS - g_mb * V_BS - g_DS * V_DS;

	m_ns.A[PinG][PinG] = 0;
	m_ns.A[PinG][PinD] = 0;
	m_ns.A[PinG][PinS] = 0;
	m_ns.A[PinG][PinB] = 0;

	m_ns.A[PinD][PinG] =  g_M;
	m_ns.A[PinD][PinD] =  g_DS + g_BD;
	m_ns.A[PinD][PinS] = -g_DS - g_M - g_mb;
	m_ns.A[PinD][PinB] =  g_mb - g_BD;

	m_ns.A[PinS][PinG] = -g_M;
	m_ns.A[PinS][PinD] = -g_DS;
	m_ns.A[PinS][PinS] =  g_BS + g_DS + g_M + g_mb;
	m_ns.A[PinS][PinB] = -g_BS - g_mb;

	m_ns.A[PinB][PinG] = 0;
	m_ns.A[PinB][PinD] = -g_BD;
	m_ns.A[PinB][PinS] = -g_BS;
	m_ns.A[PinB][PinB] =  g_BS + g_BD;

	m_ns.I[PinG] = 0;
	m_ns.I[PinD] = ( I_BD_eq - I_DS_eq) * m_pol;
	m_ns.I[PinS] = ( I_BS_eq + I_DS_eq) * m_pol;
	m_ns.I[PinB] = (-I_BD_eq - I_BS_eq) * m_pol;
}

void MOSFET::calcIg(double V_BS, double V_DS, double V_GS,
                    double *I_BS, double *I_BD, double *I_D,
                    double *g_BS, double *g_BD, double *g_DS,
                    double *g_M, double *g_mb) const {
	const double I_S = m_mosfetSettings.I_S;
	const double N = m_mosfetSettings.N;
	const double length = m_mosfetSettings.L;

	// BD and BS diodes
//	mosDiodeJunction(V_BS, I_S, N, I_BS, g_BS);
//	mosDiodeJunction(V_BS - V_DS, I_S, N, I_BD, g_BD);

	diodeJunction(std::max(V_BS,        V_lim), I_S, N, I_BS, g_BS);
	diodeJunction(std::max(V_BS - V_DS, V_lim), I_S, N, I_BD, g_BD);

	// bias-dependent threshold voltage
	const double V_tst = V_GS - V_T;

	*I_D  = 0;
	*g_DS = 0;
	*g_M  = 0;
	*g_mb = 0;

	if(V_tst > 0) {
		const double gate_length_term = (1 +  length * V_DS);
		const double beta = m_mosfetSettings.beta();

		if (V_tst < V_DS) {
			// saturation region
			const double tmp = beta / 2 * V_tst * V_tst;

			*I_D  = tmp * gate_length_term;
			*g_DS = tmp * length;
			*g_M  = beta * gate_length_term * V_tst;

		} else {
			// linear region
			const double tmp = beta * gate_length_term;
			const double tmp2 = (V_GS - V_T - V_DS / 2);

			*I_D  = beta * tmp2 * V_DS;
			*g_DS = tmp * (V_GS - V_T - V_DS) + beta * length * V_DS * tmp2;
			*g_M  = tmp * V_DS;
		}

		*g_mb = *g_M * BULK_THRESHOLD / (2 * sqrt(BULK_JUNCTION_POTENTIAL - V_BS));
	}
}

void MOSFET::setMOSFETSettings(const MOSFETSettings &settings) {
	m_mosfetSettings = settings;
	updateLim();

	if (p_eSet)
		p_eSet->setCacheInvalidated();
}

void MOSFET::updateLim() {
	double I_S = m_mosfetSettings.I_S;
	double N = m_mosfetSettings.N;
	V_lim = diodeLimitedVoltage(I_S, N);
}

//END class MOSFET
