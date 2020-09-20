/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "opamp.h"
#include "elementset.h"
#include "matrix.h"

OpAmp::OpAmp()
    : Element::Element()
{
    m_numCBranches = 1;
    m_numCNodes = 3;
}

OpAmp::~OpAmp()
{
}

void OpAmp::add_initial_dc()
{
    if (!b_status)
        return;

    // Non-inverting input
    A_c(0, 0) = 1;

    // Inverting input
    A_c(0, 2) = -1;

    // Output
    A_b(1, 0) = 1;
}

void OpAmp::updateCurrents()
{
    if (!b_status)
        return;
    m_cnodeI[0] = m_cnodeI[2] = 0.0;
    m_cnodeI[1] = p_cbranch[0]->i;
}
