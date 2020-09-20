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

#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#endif

// BEGIN class MOSFETSettings
MOSFETSettings::MOSFETSettings()
{
    I_S = 1e-14;
    N = 1.0;
    K_P = 2e-5;
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
// END class MOSFETSettings

// BEGIN class MOSFETState
MOSFETState::MOSFETState()
{
    reset();
}

void MOSFETState::reset()
{
    for (unsigned i = 0; i < 4; ++i) {
        for (unsigned j = 0; j < 4; ++j)
            A[i][j] = 0.0;

        I[i] = 0.0;
    }
}

MOSFETState MOSFETState::operator-(const MOSFETState &s) const
{
    MOSFETState newState(*this);

    for (unsigned i = 0; i < 4; ++i) {
        for (unsigned j = 0; j < 4; ++j)
            newState.A[i][j] -= s.A[i][j];

        newState.I[i] -= s.I[i];
    }

    return newState;
}
// END class MOSFETState

// BEGIN class MOSFET
const uint MOSFET::PinD = 0;
const uint MOSFET::PinG = 1;
const uint MOSFET::PinS = 2;
const uint MOSFET::PinB = 3;

MOSFET::MOSFET(MOSFET_type type)
{
    m_pol = 10;
    switch (type) {
    case neMOSFET:
        // 		case ndMOSFET:
        m_pol = 1;
        break;

    case peMOSFET:
        // 		case pdMOSFET:
        m_pol = -1;
        break;
    }

    V_GS_prev = V_GD_prev = V_BD_prev = V_DS_prev = V_BS_prev = 0.0;
    m_numCNodes = 4;
    updateLim();
}

MOSFET::~MOSFET()
{
}

void MOSFET::add_initial_dc()
{
    V_GS_prev = V_GD_prev = V_BD_prev = V_DS_prev = V_BS_prev = 0.0;
    m_os.reset();
    update_dc();
}

void MOSFET::updateCurrents()
{
    if (!b_status)
        return;

    double V_D = p_cnode[PinD]->v;
    double V_G = p_cnode[PinG]->v;
    double V_S = p_cnode[PinS]->v;
    double V_B = p_cnode[PinB]->v;

    double V_GD = (V_G - V_D) * m_pol;
    double V_GS = (V_G - V_S) * m_pol;
    double V_BS = (V_B - V_S) * m_pol;
    double V_BD = (V_B - V_D) * m_pol;
    double V_DS = (V_D - V_S) * m_pol;

    double I_BS, I_BD, I_DS, g_BS, g_BD, g_DS, g_M;
    calcIg(V_BS, V_BD, V_DS, V_GS, V_GD, &I_BS, &I_BD, &I_DS, &g_BS, &g_BD, &g_DS, &g_M);

    m_cnodeI[PinD] = -I_DS + I_BD;
    m_cnodeI[PinB] = -I_BD - I_BS;
    m_cnodeI[PinS] = +I_DS + I_BS;
}

void MOSFET::update_dc()
{
    if (!b_status)
        return;

    calc_eq();

    MOSFETState diff = m_ns - m_os;
    for (unsigned i = 0; i < 4; ++i) {
        for (unsigned j = 0; j < 4; ++j)
            A_g(i, j) += diff.A[i][j];

        b_i(i) += diff.I[i];
    }

    m_os = m_ns;
}

void MOSFET::calc_eq()
{
    double N = m_mosfetSettings.N;

    double V_D = p_cnode[PinD]->v;
    double V_G = p_cnode[PinG]->v;
    double V_S = p_cnode[PinS]->v;
    double V_B = p_cnode[PinB]->v;

    double V_GD = (V_G - V_D) * m_pol;
    double V_GS = (V_G - V_S) * m_pol;
    double V_BS = (V_B - V_S) * m_pol;
    double V_BD = (V_B - V_D) * m_pol;
    double V_DS = (V_D - V_S) * m_pol;

    // help convergence
    if (V_DS >= 0) {
        V_GS = fetVoltage(V_GS, V_GS_prev, m_pol);

        // recalculate V_DS, same for other similar lines
        V_DS = V_GS - V_GD;

        V_DS = fetVoltageDS(V_DS, V_DS_prev);
        V_GD = V_GS - V_DS;

        V_BS = diodeVoltage(V_BS, V_BS_prev, N, V_lim);
        V_BD = V_BS - V_DS;
    } else {
        V_GD = fetVoltage(V_GD, V_GD_prev, m_pol);
        V_DS = V_GS - V_GD;

        V_DS = -fetVoltageDS(-V_DS, -V_DS_prev);
        V_GS = V_GD + V_DS;

        V_BD = diodeVoltage(V_BD, V_BD_prev, N, V_lim);
        V_BS = V_BD + V_DS;
    }

    V_GS_prev = V_GS;
    V_GD_prev = V_GD;
    V_BD_prev = V_BD;
    V_DS_prev = V_DS;
    V_BS_prev = V_BS;

    double I_BS, I_BD, I_DS, g_BS, g_BD, g_DS, g_M;
    calcIg(V_BS, V_BD, V_DS, V_GS, V_GD, &I_BS, &I_BD, &I_DS, &g_BS, &g_BD, &g_DS, &g_M);

    double I_BD_eq = I_BD - g_BD * V_BD;
    double I_BS_eq = I_BS - g_BS * V_BS;

    double sc = (V_DS >= 0) ? g_M : 0;
    double dc = (V_DS < 0) ? g_M : 0;
    double I_DS_eq = I_DS - (g_DS * V_DS) - (g_M * ((V_DS >= 0) ? V_GS : V_GD));

    m_ns.A[PinG][PinG] = 0;
    m_ns.A[PinG][PinD] = 0;
    m_ns.A[PinG][PinS] = 0;
    m_ns.A[PinG][PinB] = 0;

    m_ns.A[PinD][PinG] = g_M;
    m_ns.A[PinD][PinD] = g_DS + g_BD - dc;
    m_ns.A[PinD][PinS] = -g_DS - sc;
    m_ns.A[PinD][PinB] = -g_BD;

    m_ns.A[PinS][PinG] = -g_M;
    m_ns.A[PinS][PinD] = -g_DS + dc;
    m_ns.A[PinS][PinS] = g_BS + g_DS + sc;
    m_ns.A[PinS][PinB] = -g_BS;

    m_ns.A[PinB][PinG] = 0;
    m_ns.A[PinB][PinD] = -g_BD;
    m_ns.A[PinB][PinS] = -g_BS;
    m_ns.A[PinB][PinB] = g_BS + g_BD;

    m_ns.I[PinG] = 0;
    m_ns.I[PinD] = (+I_BD_eq - I_DS_eq) * m_pol;
    m_ns.I[PinS] = (+I_BS_eq + I_DS_eq) * m_pol;
    m_ns.I[PinB] = (-I_BD_eq - I_BS_eq) * m_pol;
}

void MOSFET::calcIg(double V_BS, double V_BD, double V_DS, double V_GS, double V_GD, double *I_BS, double *I_BD, double *I_DS, double *g_BS, double *g_BD, double *g_DS, double *g_M) const
{
    double I_S = m_mosfetSettings.I_S;
    double N = m_mosfetSettings.N;
    double beta = m_mosfetSettings.beta();

    // BD and BS diodes
    mosDiodeJunction(V_BS, I_S, N, I_BS, g_BS);
    mosDiodeJunction(V_BD, I_S, N, I_BD, g_BD);

    // bias-dependent threshold voltage
    double V_tst = ((V_DS >= 0) ? V_GS : V_GD) - m_pol;

    *g_DS = 0;
    *I_DS = 0;
    *g_M = 0;

    if (V_tst > 0) {
        double V_DS_abs = std::abs(V_DS);
        if (V_tst <= V_DS_abs) {
            // saturation region
            *g_M = beta * V_tst;
            *I_DS = *g_M * V_tst / 2;
        } else {
            // linear region
            *g_M = beta * V_DS_abs;
            *I_DS = *g_M * (V_tst - V_DS_abs / 2);
            *g_DS = beta * (V_tst - V_DS_abs);
        }
    }

    if (V_DS < 0)
        *I_DS = -*I_DS;
}

void MOSFET::setMOSFETSettings(const MOSFETSettings &settings)
{
    m_mosfetSettings = settings;
    updateLim();
    if (p_eSet)
        p_eSet->setCacheInvalidated();
}

void MOSFET::updateLim()
{
    double I_S = m_mosfetSettings.I_S;
    double N = m_mosfetSettings.N;
    V_lim = diodeLimitedVoltage(I_S, N);
}
// END class MOSFET
