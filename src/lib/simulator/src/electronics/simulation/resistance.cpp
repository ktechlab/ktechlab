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
#include "resistance.h"

// #include <kdebug.h>

Resistance::Resistance(const double resistance)
		: Element::Element() {
	m_g = resistance < 1e-9 ? 1e9 : 1. / resistance;
	m_numCNodes = 2;
// 	kdDebug() << k_funcinfo << endl;
}

Resistance::~Resistance() {
// 	kdDebug() << k_funcinfo << endl;
}

void Resistance::setConductance(const double g) {
	if (g == m_g)
		return;

	if (p_eSet)
		p_eSet->setCacheInvalidated();

	m_g = g - m_g;
	add_initial_dc();

	m_g = g; 
}

void Resistance::setResistance(const double r) {
	setConductance(r < 1e-9 ? 1e9 : 1. / r);
}

void Resistance::add_initial_dc() {
	if (!b_status) return;

	A_g(0, 0) += m_g;
	A_g(1, 1) += m_g;
	A_g(0, 1) -= m_g;
	A_g(1, 0) -= m_g;
}

void Resistance::updateCurrents() {
	if (!b_status) return;

	const double i = (p_cnode[0]->voltage() - p_cnode[1]->voltage()) * m_g;

    m_cnodeCurrent[0] = i;
    m_cnodeCurrent[1] = -i;
}

