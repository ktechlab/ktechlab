/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "currentsource.h"
#include "elementset.h"
#include "matrix.h"

CurrentSource::CurrentSource(const double current)
    : Element::Element()
{
    m_i = current;
    m_numCNodes = 2;
}

CurrentSource::~CurrentSource()
{
}

void CurrentSource::setCurrent(const double i)
{
    if (i == m_i)
        return;

    if (p_eSet)
        p_eSet->setCacheInvalidated();

    // Remove the old current
    m_i = -m_i;
    add_initial_dc();

    m_i = i;
    add_initial_dc();
}

void CurrentSource::add_initial_dc()
{
    if (!b_status)
        return;

    b_i(0) -= m_i;
    b_i(1) += m_i;
}

void CurrentSource::updateCurrents()
{
    m_cnodeI[0] = -m_i;
    m_cnodeI[1] = m_i;
}
