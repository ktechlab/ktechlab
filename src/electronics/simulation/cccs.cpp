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
#include "cccs.h"

CCCS::CCCS( const double gain)
	: Element::Element()
{
	m_g = gain;
	m_numCBranches = 2;
	m_numCNodes = 4;
}

CCCS::~CCCS()
{
}

void CCCS::setGain( const double g)
{
	if( m_g == g)
		return;
	
	if(p_eSet)
		p_eSet->setCacheInvalidated();
	
	m_g = g;
	add_initial_dc();
}


void CCCS::add_map()
{
/*
	if(!getStatus()) return;
	
	if( !p_cnode[0]->isGround)
	{
		p_A->setUse_b( p_cnode[0]->n(), p_cbranch[0]->n(), Map::et_constant, true);
		p_A->setUse_c( p_cbranch[0]->n(), p_cnode[0]->n(), Map::et_constant, true);
	}
	if( !p_cnode[1]->isGround)
	{
		p_A->setUse_b( p_cnode[1]->n(), p_cbranch[0]->n(), Map::et_constant, true);
		p_A->setUse_c( p_cbranch[0]->n(), p_cnode[1]->n(), Map::et_constant, true);
	}
	if( !p_cnode[2]->isGround)
	{
		p_A->setUse_b( p_cnode[2]->n(), p_cbranch[1]->n(), Map::et_constant, true);
	}
	if( !p_cnode[3]->isGround)
	{
		p_A->setUse_b( p_cnode[3]->n(), p_cbranch[1]->n(), Map::et_constant, true);
	}
	p_A->setUse_d( p_cbranch[1]->n(), p_cbranch[0]->n(), Map::et_stable, true);
	p_A->setUse_d( p_cbranch[1]->n(), p_cbranch[1]->n(), Map::et_constant, true);
*/
}

void CCCS::add_initial_dc()
{
	if(!getStatus()) return;
	
	A_b( 0, 0) = 1;
	A_c( 0, 0) = 1;
	A_b( 1, 0) = -1;
	A_c( 0, 1) = -1;
	A_b( 2, 1) = 1;
	A_b( 3, 1) = -1;
	A_d( 1, 0) = -m_g;
	A_d( 1, 1) = 1;
}

void CCCS::updateCurrents()
{
	if(!getStatus()) return;
	m_cnodeI[1] = cbranchCurrent(0);
	m_cnodeI[0] = -m_cnodeI[1];
	m_cnodeI[3] = cbranchCurrent(1);
	m_cnodeI[2] = -m_cnodeI[3];
}

