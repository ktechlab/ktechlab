/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "bjt.h"
#include "elementset.h"

#include <cmath>
using namespace std;

//BEGIN class BJTSettings
BJTSettings::BJTSettings()
{
	I_S = 1e-16;
	N_F = 1.0;
	N_R = 1.0;
	B_F = 100.0;
	B_R = 1.0;
}
//END class BJTSettings

//BEGIN class BJTState
BJTState::BJTState()
{
	reset();
}

void BJTState::reset()
{
	for ( unsigned i = 0; i < 3; ++i )
	{
		for ( unsigned j = 0; j < 3; ++j )
			A[i][j] = 0.0;

		I[i] = 0.0;
	}
}

BJTState BJTState::operator-(const BJTState &s) const
{
	BJTState newState(*this);

	double *newI = newState.I;
	const double *sI   = s.I;

	for ( unsigned i = 0; i < 3; ++i ) {
		double *newAi = newState.A[i];
		const double *sAi = s.A[i];
	
		for ( unsigned j = 0; j < 3; ++j )
			newAi[j] -= sAi[j];

		newI[i] -= sI[i];
	}

	return newState;
}
//END class BJTState

//BEGIN class BJT
BJT::BJT( bool isNPN )
{
	V_BE_prev = V_BC_prev = 0.0;

	m_pol = isNPN ? 1 : -1;
	m_numCNodes = 3;
	updateLim();
}

BJT::~BJT()
{
}

void BJT::add_initial_dc()
{
	V_BE_prev = V_BC_prev = 0.0;
	m_os.reset();
	update_dc();
}

void BJT::updateCurrents()
{
    m_cnodeCurrent[1] = I_BC - I_T;
    m_cnodeCurrent[2] = I_BE + I_T;
    m_cnodeCurrent[0] = - (m_cnodeCurrent[1] + m_cnodeCurrent[2]);
}

void BJT::update_dc()
{
	if (!b_status) return;
	
	calc_eq();
	
	BJTState diff = m_ns - m_os;
	for(unsigned i = 0; i < 3; ++i)
	{
		for(unsigned j = 0 ; j < 3; ++j)
			A_g(i, j) += diff.A[i][j];

		b_i(i) += diff.I[i];
	}

	m_os = m_ns;
}

void BJT::calc_eq()
{
	double V_B = p_cnode[0]->voltage();
	double V_C = p_cnode[1]->voltage();
	double V_E = p_cnode[2]->voltage();
	
	double V_BE = (V_B - V_E) * m_pol;
	double V_BC = (V_B - V_C) * m_pol;

	// adjust voltage to help convergence
	V_BE_prev = V_BE = diodeVoltage(V_BE, V_BE_prev, m_bjtSettings.N_F, V_BE_lim);
	V_BC_prev = V_BC = diodeVoltage(V_BC, V_BC_prev, m_bjtSettings.N_R, V_BC_lim);

// #####	
	double g_BE, g_BC, g_IF, g_IR;
	calcIg(V_BE, V_BC, &I_BE, &I_BC, &I_T, &g_BE, &g_BC, &g_IF, &g_IR);
// #####

// matrix diagonals.
	m_ns.A[0][0] =  g_BC + g_BE;
	m_ns.A[1][1] =  g_BC + g_IR;
	m_ns.A[2][2] =  g_BE + g_IF;

// off-diags. 
	m_ns.A[0][1] = -g_BC;
	m_ns.A[0][2] = -g_BE;

	m_ns.A[1][0] = -g_BC + (g_IF - g_IR);
	m_ns.A[1][2] = -g_IF;

	m_ns.A[2][0] = -g_BE - (g_IF - g_IR);
	m_ns.A[2][1] = -g_IR;

	double I_eq_B = I_BE - V_BE * g_BE;
	double I_eq_C = I_BC - V_BC * g_BC;
	double I_eq_E = I_T  - V_BE * g_IF + V_BC * g_IR;

	m_ns.I[0] = (-I_eq_B - I_eq_C) * m_pol;
	m_ns.I[1] = ( I_eq_C - I_eq_E) * m_pol;
	m_ns.I[2] = ( I_eq_B + I_eq_E) * m_pol;
}

void BJT::calcIg(double  V_BE, double  V_BC,
		 double *I_BE, double *I_BC, double *I_T,
		 double *g_BE, double *g_BC,
		 double *g_IF, double *g_IR) const
{
	double I_S = m_bjtSettings.I_S;
	double N_F = m_bjtSettings.N_F;
	double N_R = m_bjtSettings.N_R;
	double B_F = m_bjtSettings.B_F;
	double B_R = m_bjtSettings.B_R;

	// BE diodes
	double g_tiny = (V_BE < (-10 * V_T * N_F)) ? I_S : 0;
	double I_F;
	diodeJunction(V_BE, I_S, N_F, &I_F, g_IF);
	*I_BE = (I_F / B_F) + (g_tiny * V_BE);
	*g_BE = (*g_IF / B_F) + (g_tiny);

	// BC diodes
	g_tiny = (V_BC < (-10 * V_T * N_R)) ? I_S : 0;
	double I_R;
	diodeJunction(V_BC, I_S, N_R, &I_R, g_IR);
	*I_BC = (I_R / B_R) + (g_tiny * V_BC);
	*g_BC = (*g_IR / B_R) + g_tiny;

	*I_T = I_F - I_R;
}

void BJT::setBJTSettings(const BJTSettings &settings)
{
	m_bjtSettings = settings;
	updateLim();
	if (p_eSet)
		p_eSet->setCacheInvalidated();
}

void BJT::updateLim()
{
	double I_S = m_bjtSettings.I_S;
	double N_F = m_bjtSettings.N_F;
	double N_R = m_bjtSettings.N_R;
	
	V_BE_lim = diodeLimitedVoltage(I_S, N_F);
	V_BC_lim = diodeLimitedVoltage(I_S, N_R);
}
//END class BJT

