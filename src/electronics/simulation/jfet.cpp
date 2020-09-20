/***************************************************************************
 *   Copyright (C) 2005-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "jfet.h"

#include <cmath>
using namespace std;

const uint JFET::PinD = 0;
const uint JFET::PinG = 1;
const uint JFET::PinS = 2;

// BEGIN class JFETSettings
JFETSettings::JFETSettings()
{
    V_Th = -2.0;
    beta = 1e-4;
    I_S = 1e-14;
    N = 1.0;
    N_R = 2.0;
}
// END class JFETSettings

// BEGIN class JFETState

JFETState::JFETState()
{
    reset();
}

void JFETState::reset()
{
    for (unsigned i = 0; i < 3; ++i) {
        for (unsigned j = 0; j < 3; ++j)
            A[i][j] = 0.0;

        I[i] = 0.0;
    }
}

JFETState JFETState::operator-(const JFETState &s) const
{
    JFETState newState(*this);

    for (unsigned i = 0; i < 3; ++i) {
        for (unsigned j = 0; j < 3; ++j)
            newState.A[i][j] -= s.A[i][j];

        newState.I[i] -= s.I[i];
    }

    return newState;
}

// END class JFETState

// BEGIN class JFET
JFET::JFET(JFET_type type)
{
    switch (type) {
    case nJFET:
        m_pol = 1;
        break;

    case pJFET:
        m_pol = -1;
        break;
    }

    V_GS_prev = 0.0;
    V_GD_prev = 0.0;
    m_numCNodes = 3;
    updateLim();
}

JFET::~JFET()
{
}

void JFET::add_initial_dc()
{
    V_GS_prev = 0.0;
    V_GD_prev = 0.0;
    m_os.reset();
    update_dc();
}

void JFET::updateCurrents()
{
    if (!b_status)
        return;

    double V_D = p_cnode[PinD]->v;
    double V_G = p_cnode[PinG]->v;
    double V_S = p_cnode[PinS]->v;

    double V_GS = V_G - V_S;
    double V_GD = V_G - V_D;
    double V_DS = V_D - V_S;

    double I_GS, I_GD, I_DS, g_GS, g_GD, g_DS, g_m;

    calcIg(V_GS, V_GD, V_DS, &I_GS, &I_GD, &I_DS, &g_GS, &g_GD, &g_DS, &g_m);

    m_cnodeI[PinD] = I_GD - I_DS;
    m_cnodeI[PinS] = I_GS + I_DS;
    m_cnodeI[PinG] = -(m_cnodeI[PinD] + m_cnodeI[PinS]);
}

void JFET::update_dc()
{
    if (!b_status)
        return;

    calc_eq();

    JFETState diff = m_ns - m_os;
    for (unsigned i = 0; i < 3; ++i) {
        for (unsigned j = 0; j < 3; ++j)
            A_g(i, j) += diff.A[i][j];

        b_i(i) += diff.I[i];
    }

    m_os = m_ns;
}

void JFET::calc_eq()
{
    double N = m_jfetSettings.N;

    double V_D = p_cnode[PinD]->v;
    double V_G = p_cnode[PinG]->v;
    double V_S = p_cnode[PinS]->v;

    // GS diode
    double V_GS = V_G - V_S;
    V_GS_prev = V_GS = diodeVoltage(V_GS, V_GS_prev, N, V_lim);

    // GD diode
    double V_GD = V_G - V_D;
    V_GD_prev = V_GD = diodeVoltage(V_GD, V_GD_prev, N, V_lim);

    double V_DS = V_GS - V_GD;

    double I_GS, I_GD, I_DS, g_GS, g_GD, g_DS, g_m;

    calcIg(V_GS, V_GD, V_DS, &I_GS, &I_GD, &I_DS, &g_GS, &g_GD, &g_DS, &g_m);

    // current sources
    double IeqG = I_GS - (g_GS * V_GS);
    double IeqD = I_GD - (g_GD * V_GD);
    double IeqS = I_DS - (g_m * V_GS) - (g_DS * V_DS);

    m_ns.A[PinG][PinG] = +g_GS + g_GD;
    m_ns.A[PinG][PinD] = -g_GD;
    m_ns.A[PinG][PinS] = -g_GS;

    m_ns.A[PinD][PinG] = -g_GD + g_m;
    m_ns.A[PinD][PinD] = +g_DS + g_GD;
    m_ns.A[PinD][PinS] = -g_m - g_DS;

    m_ns.A[PinS][PinG] = -g_GS - g_m;
    m_ns.A[PinS][PinD] = -g_DS;
    m_ns.A[PinS][PinS] = +g_GS + g_DS + g_m;

    m_ns.I[PinG] = (-IeqG - IeqD) * m_pol;
    m_ns.I[PinD] = (+IeqD - IeqS) * m_pol;
    m_ns.I[PinS] = (+IeqG + IeqS) * m_pol;
}

void JFET::calcIg(double V_GS, double V_GD, double V_DS, double *I_GS, double *I_GD, double *I_DS, double *g_GS, double *g_GD, double *g_DS, double *g_m) const
{
    double V_Th = m_jfetSettings.V_Th;
    double beta = m_jfetSettings.beta;
    double I_S = m_jfetSettings.I_S;
    double N = m_jfetSettings.N;
    double N_R = m_jfetSettings.N_R;

    // GS diode
    double g_tiny = (V_GS < (-10 * V_T * N)) ? I_S : 0;
    *g_GS = diodeConductance(V_GS, I_S, N) + diodeConductance(V_GS, 0, N_R) + g_tiny;
    *I_GS = diodeCurrent(V_GS, I_S, N) + diodeCurrent(V_GS, 0, N_R) + g_tiny * V_GS;

    // GD diode
    g_tiny = (V_GD < (-10 * V_T * N)) ? I_S : 0;
    *g_GD = diodeConductance(V_GD, I_S, N) + diodeConductance(V_GD, 0, N_R) + g_tiny;
    *I_GD = diodeCurrent(V_GD, I_S, N) + diodeCurrent(V_GD, 0, N_R) + (g_tiny * V_GD);

    double V_GST = V_GS - V_Th;
    double V_GDT = V_GD - V_Th;

    *I_DS = 0;
    *g_m = 0;
    *g_DS = 0;

    switch (getOpRegion(V_DS, V_GST, V_GDT)) {
    case NormalCutoff:
    case InverseCutoff:
        break;

    case NormalSaturation:
        *I_DS = beta * V_GST * V_GST;
        *g_DS = 0;
        *g_m = beta * 2 * V_GST;
        break;

    case NormalLinear:
        *I_DS = beta * V_DS * (2 * V_GST - V_DS);
        *g_DS = beta * 2 * (V_GST - V_DS);
        *g_m = beta * 2 * V_DS;
        break;

    case InverseSaturation:
        *I_DS = -beta * V_GDT * V_GDT;
        *g_DS = beta * 2 * V_GDT;
        *g_m = -beta * 2 * V_GDT;
        break;

    case InverseLinear:
        *I_DS = beta * V_DS * (2 * V_GDT + V_DS);
        *g_DS = 2 * beta * V_GDT;
        *g_m = beta * 2 * V_DS;
        break;
    }
}

JFET::OpRegion JFET::getOpRegion(double V_DS, double V_GST, double V_GDT) const
{
    if (V_DS > 0) {
        if (V_GST <= 0)
            return NormalCutoff;
        else if (V_GST <= V_DS)
            return NormalSaturation;
        else
            return NormalLinear;
    } else {
        if (V_GDT <= 0)
            return InverseCutoff;
        else if (V_GDT <= -V_DS)
            return InverseSaturation;
        else
            return InverseLinear;
    }
}

void JFET::setJFETSettings(const JFETSettings &settings)
{
    m_jfetSettings = settings;
    updateLim();

    if (p_eSet)
        p_eSet->setCacheInvalidated();
}

void JFET::updateLim()
{
    double I_S = m_jfetSettings.I_S;
    double N = m_jfetSettings.N;
    V_lim = diodeLimitedVoltage(I_S, N);
}

// END class JFET
