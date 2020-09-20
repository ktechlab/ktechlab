/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "currentsignal.h"
#include "element.h"
#include "matrix.h"

CurrentSignal::CurrentSignal(double delta, double current)
    : Reactive::Reactive(delta)
{
    m_current = current;
    m_oldCurrent = m_newCurrent = 0.0;
    m_numCNodes = 2;
}

CurrentSignal::~CurrentSignal()
{
}

void CurrentSignal::setCurrent(double i)
{
    // Instead of calling step again, we can just "adjust" what the current should be
    m_newCurrent *= i / m_current;
    m_current = i;
    addCurrents();
}

void CurrentSignal::add_initial_dc()
{
    m_oldCurrent = 0.0;
    // time_step() will handle everything for us now :)
}

void CurrentSignal::updateCurrents()
{
    m_cnodeI[1] = m_newCurrent;
    m_cnodeI[0] = -m_newCurrent;
}

void CurrentSignal::time_step()
{
    m_newCurrent = m_current * advance(m_delta);
    addCurrents();
}

void CurrentSignal::addCurrents()
{
    if (!b_status)
        return;

    if (m_newCurrent == m_oldCurrent)
        return;

    b_i(0) -= m_newCurrent - m_oldCurrent;
    b_i(1) += m_newCurrent - m_oldCurrent;

    m_oldCurrent = m_newCurrent;
}
