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

Capacitance::Capacitance(const double capacitance, const double delta)
    : Reactive(delta)
{
    m_cap = capacitance;
    m_scaled_cap = i_eq_old = 0.;
    m_numCNodes = 2;
    setMethod(Capacitance::m_euler);
}

Capacitance::~Capacitance()
{
}

void Capacitance::setCapacitance(const double c)
{
    m_cap = c;
}

void Capacitance::add_initial_dc()
{
    // We don't need to do anything here, as time_step() will do that for us,
    // apart from to make sure our old values are 0
    m_scaled_cap = i_eq_old = 0.;
}

void Capacitance::updateCurrents()
{
    if (!b_status)
        return;
    const double r_i = (p_cnode[0]->v - p_cnode[1]->v) * m_scaled_cap;
    m_cnodeI[0] = -i_eq_old - r_i;
    m_cnodeI[1] = -m_cnodeI[0];
}

void Capacitance::time_step()
{
    if (!b_status)
        return;

    double v = p_cnode[0]->v - p_cnode[1]->v;
    double i_eq_new = 0.0, scaled_cap_new = 0.0;

    if (m_method == Capacitance::m_euler) {
        scaled_cap_new = m_cap / m_delta;
        i_eq_new = -v * scaled_cap_new;
    } else if (m_method == Capacitance::m_trap) {
        // TODO Implement + test trapezoidal method
        scaled_cap_new = 2. * m_cap / m_delta;
    }

    if (m_scaled_cap != scaled_cap_new) {
        const double tmp = scaled_cap_new - m_scaled_cap;
        A_g(0, 0) += tmp;
        A_g(0, 1) -= tmp;
        A_g(1, 0) -= tmp;
        A_g(1, 1) += tmp;
    }

    if (i_eq_new != i_eq_old) {
        const double tmp = i_eq_new - i_eq_old;
        b_i(0) -= tmp;
        b_i(1) += tmp;
    }

    m_scaled_cap = scaled_cap_new;
    i_eq_old = i_eq_new;
}

bool Capacitance::updateStatus()
{
    b_status = Reactive::updateStatus();
    if (m_method == Capacitance::m_none)
        b_status = false;
    return b_status;
}

void Capacitance::setMethod(Method m)
{
    m_method = m;
    updateStatus();
}
