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
#include "vccs.h"

VCCS::VCCS( const double gain )
	: Element::Element()
{
	m_g = gain;
	m_numCBranches = 1;
	m_numCNodes = 4;
}

VCCS::~VCCS()
{
}

void VCCS::setGain( const double g )
{
	if( g == m_g ) return;
	
	if(p_eSet)
		p_eSet->setCacheInvalidated();
	
	// Remove old values
	m_g = -m_g;
	add_initial_dc();
	
	// Add new values
	m_g = g;
	add_initial_dc();
}


void VCCS::add_map()
{
	if(!b_status) return;
	
	if( !p_cnode[0]->isGround )
	{
		p_A->setUse_c( p_cbranch[0]->n(), p_cnode[0]->n(), Map::et_stable, false );
	}
	if( !p_cnode[1]->isGround  )
	{
		p_A->setUse_c( p_cbranch[0]->n(), p_cnode[1]->n(), Map::et_stable, false );
	}
	if( !p_cnode[2]->isGround  )
	{
		p_A->setUse_b( p_cnode[2]->n(), p_cbranch[0]->n(), Map::et_constant, true );
		p_A->setUse_c( p_cbranch[0]->n(), p_cnode[2]->n(), Map::et_constant, true );
	}
	if( !p_cnode[3]->isGround  )
	{
		p_A->setUse_b( p_cnode[3]->n(), p_cbranch[0]->n(), Map::et_constant, true );
		p_A->setUse_c( p_cbranch[0]->n(), p_cnode[3]->n(), Map::et_constant, true );
	}
}


void VCCS::add_initial_dc()
{
	if(!b_status)
		return;
	
	A_g( 2, 0 ) += m_g;
	A_g( 3, 0 ) -= m_g;
	A_g( 2, 1 ) -= m_g;
	A_g( 3, 1 ) += m_g;
}


void VCCS::updateCurrents()
{
	if(!b_status) return;

	m_cnodeI[0] = m_cnodeI[1] = 0.;
	m_cnodeI[3] = (p_cnode[0]->v-p_cnode[1]->v)*m_g;
	m_cnodeI[2] = -m_cnodeI[3];
}


