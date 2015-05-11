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
#include "matrix.h"
#include "resistance.h"

// #include <kdebug.h>

Resistance::Resistance( const double resistance )
	: Element::Element()
{
	m_g = resistance < 1e-9 ? 1e9 : 1./resistance;
	m_numCNodes = 2;
// 	kDebug() << k_funcinfo << endl;
}

Resistance::~Resistance()
{
// 	kDebug() << k_funcinfo << endl;
}

void Resistance::setConductance( const double g )
{
	if ( g == m_g )
		return;
	
	if (p_eSet)
		p_eSet->setCacheInvalidated();
	
	// Remove old resistance
	m_g = -m_g;
	add_initial_dc();

	m_g = g;
	add_initial_dc();
}

void Resistance::setResistance( const double r )
{
	setConductance( r < 1e-9 ? 1e9 : 1./r );
}

void Resistance::add_initial_dc()
{
	if (!b_status) return;
	
	A_g( 0, 0 ) += m_g;
	A_g( 1, 1 ) += m_g;
	A_g( 0, 1 ) -= m_g;
	A_g( 1, 0 ) -= m_g;
}

void Resistance::updateCurrents()
{
	if (!b_status) return;
	const double v=p_cnode[0]->v-p_cnode[1]->v; 
	m_cnodeI[1] = v*m_g;
	m_cnodeI[0] = -m_cnodeI[1];
}




