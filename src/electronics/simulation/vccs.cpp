/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "vccs.h"
#include "elementset.h"

VCCS::VCCS(const double gain)
    : Element::Element()
{
    m_g = gain;
    m_numCBranches = 1;
    m_numCNodes = 4;
}

VCCS::~VCCS()
{
}

void VCCS::setGain(const double g)
{
    if (g == m_g)
        return;

    if (p_eSet)
        p_eSet->setCacheInvalidated();

    // Remove old values
    m_g = -m_g;
    add_initial_dc();

    // Add new values
    m_g = g;
    add_initial_dc();
}

void VCCS::add_initial_dc()
{
    if (!b_status)
        return;

    A_c(0, 0) = +1.0;
    A_c(0, 1) = -1.0;
    A_b(3, 0) = +1.0;
    A_b(2, 0) = -1.0;
    A_d(0, 0) = -1.0 / m_g;

#if 0
	A_g( 2, 0 ) += m_g;
	A_g( 3, 0 ) -= m_g;
	A_g( 2, 1 ) -= m_g;
	A_g( 3, 1 ) += m_g;
#endif
}

void VCCS::updateCurrents()
{
    if (!b_status)
        return;

    m_cnodeI[0] = m_cnodeI[1] = 0.;
    m_cnodeI[3] = (p_cnode[0]->v - p_cnode[1]->v) * m_g;
    m_cnodeI[2] = -m_cnodeI[3];
}
