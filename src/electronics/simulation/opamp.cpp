/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "elementset.h"
#include "opamp.h"

OpAmp::OpAmp()
	: Element::Element()
{
	m_numCBranches = 1;
	m_numCNodes = 3;
}


OpAmp::~OpAmp()
{
}


void OpAmp::add_map()
{
/*
	if(!getStatus()) return;
	
	if( !p_cnode[0]->isGround)
	{
		// Non-inverting input
		p_A->setUse_c( p_cbranch[0]->n(), p_cnode[0]->n(), Map::et_constant, true);
	}
	
	if( !p_cnode[2]->isGround)
	{
		// Inverting input
		p_A->setUse_c( p_cbranch[0]->n(), p_cnode[2]->n(), Map::et_constant, true);
	}
	
	if( !p_cnode[1]->isGround)
	{
		// Output
		p_A->setUse_b( p_cnode[1]->n(), p_cbranch[0]->n(), Map::et_constant, true);
	}
*/
}

void OpAmp::add_initial_dc()
{
	if(!getStatus()) return;

	// Non-inverting input
	A_c( 0, 0) = 1;

	// Inverting input
	A_c( 0, 2) = -1;

	// Output
	A_b( 1, 0) = 1;
}


void OpAmp::updateCurrents()
{
	if(!getStatus()) return;
	m_cnodeI[0] = m_cnodeI[2] = 0.0;
	m_cnodeI[1] = cbranchCurrent(0);
}



