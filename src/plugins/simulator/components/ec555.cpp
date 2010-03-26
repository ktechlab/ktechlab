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
#include "ecnode.h"
#include "libraryitem.h"

#include <kiconloader.h>
#include <klocale.h>
#include <qpainter.h>

Item* EC555::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
	return new EC555((ICNDocument*)itemDocument, newItem, id);
}

LibraryItem* EC555::libraryItem()
{
	return new LibraryItem(
		"ec/555",
		i18n("555"),
		i18n("Integrated Circuits"),
		"ic1.png",
		LibraryItem::lit_component,
		EC555::construct );
}

EC555::EC555(ICNDocument *icnDocument, bool newItem, const char *id)
	: Component(icnDocument, newItem, (id) ? id : "555")
{
	m_name = i18n("555");

	m_com1 = false;
	m_com2 = false;
	m_q    = false;

	setSize(-32, -32, 64, 64);

	// Pins down left
	// Pin 7
	discharge = &createPin(-40, -16, 0, "Dis")->pin();
	addDisplayText("dis", QRect(-32, -24, 24, 16), "Dis");

	// Pin 6
	threshold = &createPin( -40, 0, 0, "Th" )->pin();
	addDisplayText("th", QRect(-32, -8, 24, 16), "Th");

	// Pin 2
	trigger = &createPin(-40, 16, 0, "Trg")->pin();
	addDisplayText("trg", QRect(-32, 8, 24, 16), "Trg");

	// Top two
	// Pin 8
	vcc = &createPin(-16, -40, 90, "Vcc")->pin();
	addDisplayText("vcc", QRect(-24, -32, 16, 8), "+");

	// Pin 4
	reset = &createPin(16, -40, 90, "Res")->pin();
	addDisplayText("res", QRect(8, -28, 16, 16), "Res");

	// Bottom two
	// Pin 1
	ground = &createPin(-16, 40, 270, "Gnd")->pin();
	addDisplayText("gnd", QRect(-24, 20, 16, 8), "-");

	// Pin 5
	control = &createPin(16, 40, 270, "CV")->pin();
	addDisplayText("cv", QRect(8, 12, 16, 16), "CV");

	// Output on right
	// Pin 3
	output = &createPin(40, 0, 180, "Out")->pin();
	addDisplayText("out", QRect(8, -8, 16, 16), "Out");

	m_r1.setResistance(5e3);
	m_r23.setResistance(1e4);
	m_po_sink.setResistance(10);
	m_po_source.setConductance(0);
	m_r_discharge.setResistance(0.0001);

	setup2pinElement(m_r1, *vcc, *control);
	setup2pinElement(m_r23, *control, *ground);
	setup2pinElement(m_po_sink, *output, *ground);
	setup2pinElement(m_po_source, *output, *vcc);
	setup2pinElement(m_r_discharge, *discharge, *ground);
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

