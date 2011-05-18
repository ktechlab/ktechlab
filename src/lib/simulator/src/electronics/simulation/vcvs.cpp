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
#include "vcvs.h"

VCVS::VCVS( const double gain )
	: Element::Element()
{
	m_g = gain;
	m_numCBranches = 1;
	m_numCNodes = 4;
}


VCVS::~VCVS()
{
}


void VCVS::setGain( const double g )
{
	if ( g == m_g )
		return;
	
	if (p_eSet)
		p_eSet->setCacheInvalidated();
	
	m_g = -m_g;
	add_initial_dc();

	m_g = g;
	add_initial_dc();
}

void VCVS::add_initial_dc()
{
	if (!b_status)
		return;
	
	A_c( 0, 0 ) = -m_g;
	A_c( 0, 1 ) = m_g;
	A_b( 2, 0 ) = 1;
	A_c( 0, 2 ) = 1;
	A_b( 3, 0 ) = -1;
	A_c( 0, 3 ) = -1;
}

void VCVS::updateCurrents()
{
	if (!b_status) return;

    m_cnodeCurrent[0] = 0;
    m_cnodeCurrent[1] = 0;

	double i = p_cbranch[0]->current();

    m_cnodeCurrent[2] = -i;
    m_cnodeCurrent[3] = i;
}

