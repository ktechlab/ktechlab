/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "elementset.h"
#include "ccvs.h"

CCVS::CCVS( const double gain )
	: Element::Element()
{
	m_g = gain;
	m_numCBranches = 2;
	m_numCNodes = 4;
}

CCVS::~CCVS()
{
}

void CCVS::setGain( const double g )
{
	if ( m_g == g )
		return;
	
	if (p_eSet)
		p_eSet->setCacheInvalidated();
	
	m_g = g;

//	add_initial_dc();
	if(b_status) A_d(0, 0) = -m_g;
}

void CCVS::add_initial_dc()
{
	if (!b_status) return;

	A_b(0, 0) = 1;
	A_b(1, 0) = -1;
	A_b(2, 1) = -1;
	A_b(3, 1) = 1;
	
	A_c(1, 0) = 1;
	A_c(1, 1) = -1;
	A_c(0, 2) = 1;
	A_c(0, 3) = -1;

	A_d(0, 0) = -m_g;
}

void CCVS::updateCurrents()
{
	if (!b_status) return;
// CONTROL SIDE
	double i = p_cbranch[0]->current();
    m_cnodeCurrent[0] = -i;
    m_cnodeCurrent[1] = i;

// DPENDANT SIDE 
	i = p_cbranch[1]->current();
    m_cnodeCurrent[2] = -i;
    m_cnodeCurrent[3] = i;
}

