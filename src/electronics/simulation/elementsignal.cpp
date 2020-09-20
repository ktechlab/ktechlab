/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "elementsignal.h"
#include <cmath>

ElementSignal::ElementSignal()
{
    m_type = ElementSignal::st_sinusoidal;
    m_time = 0.;
    m_frequency = 0.;
}

ElementSignal::~ElementSignal()
{
}

void ElementSignal::setStep(Type type, double frequency)
{
    m_type = type;
    m_frequency = frequency;
    m_omega = 2 * M_PI * m_frequency;
    m_time = 1. / (4. * m_frequency);
}

double ElementSignal::advance(double delta)
{
    m_time += delta;
    if (m_time >= 1. / m_frequency)
        m_time -= 1. / m_frequency;

    switch (m_type) {
    case ElementSignal::st_sawtooth: {
        double val = (m_time * m_omega / M_PI);
        return 1 - remainder(val, 2);
    }
    case ElementSignal::st_square:
        return (((int)trunc(m_time * m_omega / M_PI) & 1) == 0) ? 1 : -1;
    case ElementSignal::st_triangular:
        // TODO Triangular signal
        return 0.;
    case ElementSignal::st_sinusoidal:
    default:
        return sin(m_time * m_omega);
    }
}
