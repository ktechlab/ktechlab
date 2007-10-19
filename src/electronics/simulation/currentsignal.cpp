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

CurrentSignal::CurrentSignal( const double delta, const double current)
	: Reactive::Reactive(delta)
{
	m_current = current;
	m_oldCurrent = m_newCurrent = 0.;
	m_numCNodes = 2;
}

CurrentSignal::~CurrentSignal()
{
}

void CurrentSignal::setCurrent( const double i)
{
	if( m_oldCurrent != m_newCurrent) add_initial_dc();
	m_newCurrent *= i/m_current; // Instead of calling step again, we can just "adjust" what the current should be
	m_current = i;
	add_initial_dc();
}


void CurrentSignal::add_map()
{
	// We don't need a map for current signal :-)
}


void CurrentSignal::add_initial_dc()
{
	if(!getStatus()) return;

	if(m_newCurrent == m_oldCurrent) return;

	b_i( 0) -= m_newCurrent-m_oldCurrent;
	b_i( 1) += m_newCurrent-m_oldCurrent;
	
	m_oldCurrent = m_newCurrent;
}

void CurrentSignal::updateCurrents()
{
	m_cnodeI[1] = m_newCurrent;
	m_cnodeI[0] = -m_newCurrent;
}

void CurrentSignal::time_step()
{
	add_initial_dc(); // Make sure our old and new are synced
	m_newCurrent = m_current*advance();
	add_initial_dc();
}
