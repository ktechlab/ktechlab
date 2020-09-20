/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "voltagepoint.h"
#include "elementset.h"
#include "matrix.h"

VoltagePoint::VoltagePoint(const double voltage)
    : Element::Element()
{
    m_voltage = -voltage;
    m_numCBranches = 1;
    m_numCNodes = 1;
}

VoltagePoint::~VoltagePoint()
{
}

void VoltagePoint::setVoltage(const double v)
{
    if (-v == m_voltage)
        return;

    if (p_eSet)
        p_eSet->setCacheInvalidated();

    m_voltage = -v;
    add_initial_dc();
}

void VoltagePoint::add_initial_dc()
{
    if (!b_status)
        return;

    A_b(0, 0) = -1;
    A_c(0, 0) = -1;

    b_v(0) = m_voltage;
}

void VoltagePoint::updateCurrents()
{
    if (!b_status)
        return;
    m_cnodeI[0] = p_cbranch[0]->i;
}
