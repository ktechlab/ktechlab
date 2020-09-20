/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cccs.h"
#include "elementset.h"
#include "matrix.h"

CCCS::CCCS(const double gain)
    : Element::Element()
{
    m_g = gain;
    m_numCBranches = 2;
    m_numCNodes = 4;
}

CCCS::~CCCS()
{
}

void CCCS::setGain(const double g)
{
    if (m_g == g)
        return;

    if (p_eSet)
        p_eSet->setCacheInvalidated();

    m_g = g;
    add_initial_dc();
}

void CCCS::add_initial_dc()
{
    if (!b_status)
        return;

    A_b(0, 0) = 1;
    A_c(0, 0) = 1;
    A_b(1, 0) = -1;
    A_c(0, 1) = -1;
    A_b(2, 1) = 1;
    A_b(3, 1) = -1;
    A_d(1, 0) = -m_g;
    A_d(1, 1) = 1;
}

void CCCS::updateCurrents()
{
    if (!b_status)
        return;
    m_cnodeI[1] = p_cbranch[0]->i;
    m_cnodeI[0] = -m_cnodeI[1];
    m_cnodeI[3] = p_cbranch[1]->i;
    m_cnodeI[2] = -m_cnodeI[3];
}
