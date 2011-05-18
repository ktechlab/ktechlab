/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ec555.h"

#include "pin.h"

EC555::EC555(Circuit& ownerCircuit)
	: Component(ownerCircuit)
{

	m_com1 = false;
	m_com2 = false;
	m_q    = false;


	m_r1.setResistance(5e3);
	m_r23.setResistance(1e4);
	m_po_sink.setResistance(10);
	m_po_source.setConductance(0);
	m_r_discharge.setResistance(0.0001);

}

EC555::~EC555() {}

// TODO: This is simulation code not UI code, so it shouldn't be here.
// Would it be better to simulate the appropriate elements, ie comparator, voltage divider,
// and flip-flop instead of all this hand-wavy logic?

void EC555::stepNonLogic()
{
	double v_threshold = threshold->voltage();
	double v_control = control->voltage();
	double v_ground = ground->voltage();
	double v_trigger = trigger->voltage();
	double v_reset = reset->voltage();
	double v_vcc = vcc->voltage();
	double v_reset_t = (v_control + v_ground) / 2;

	if(v_threshold != v_control) { m_com1 = v_threshold < v_control; }
	if(v_reset_t   != v_trigger) { m_com2 = v_reset_t   > v_trigger; }

	bool reset_asserted = v_reset >= v_reset_t;
	bool r     = (reset_asserted && m_com1);

	if(v_vcc - v_ground >= 2.5 ) {
		m_q = m_com2 && r;
	} else m_q = false;

	m_r_discharge.setConductance(0.);
	
	if(m_q) {
		m_po_source.setResistance(10.);
		m_po_sink.setConductance(0.);
	} else {
		m_po_source.setConductance(0.);
		m_po_sink.setResistance(10.);

		if(v_ground + 0.7 <= v_vcc ) {
			m_r_discharge.setResistance(10.);
		}
	}
}

