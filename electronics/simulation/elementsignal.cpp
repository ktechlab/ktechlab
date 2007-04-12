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

void ElementSignal::setStep( double delta, Type type, double frequency )
{
	m_type = type;
	m_delta = delta;
	m_frequency = frequency;
	m_omega = 6.283185307179586*m_frequency;
	m_time = 1./(4.*m_frequency);
}

double ElementSignal::advance()
{
	m_time += m_delta;
	if ( m_time >= 1./m_frequency ) m_time -= 1./m_frequency;

	// TODO: Compute the phase angle in radians.

	switch (m_type)  // TODO would function pointers be faster? 
	{
		case ElementSignal::st_sawtooth:
		{
			// TODO Sawtooth signal
			return 0.;
		}
		case ElementSignal::st_square:
		{
			return (sin(m_time*m_omega)>=0)?1:-1;
		}
		case ElementSignal::st_triangular:
		{
			// TODO Triangular signal
			return 0.;
		}
		case ElementSignal::st_sinusoidal:
		default:
		{
			// TODO: convert to phase angle version.
			return sin(m_time*m_omega);
		}
	}
}



