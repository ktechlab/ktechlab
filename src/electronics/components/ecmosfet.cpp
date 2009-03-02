/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecmosfet.h"
#include "libraryitem.h"
#include "mosfet.h"

#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <qpainter.h>

Item *ECMOSFET::constructNEM(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new ECMOSFET(MOSFET::neMOSFET, (ICNDocument*)itemDocument, newItem, id ? id : "nemosfet");
}

Item *ECMOSFET::constructPEM(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new ECMOSFET(MOSFET::peMOSFET, (ICNDocument*)itemDocument, newItem, id ? id : "pemosfet");
}

#if 0
Item *ECMOSFET::constructNDM(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new ECMOSFET(MOSFET::ndMOSFET, (ICNDocument*)itemDocument, newItem, id ? id : "ndmosfet");
}

Item *ECMOSFET::constructPDM(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new ECMOSFET(MOSFET::pdMOSFET, (ICNDocument*)itemDocument, newItem, id ? id : "pdmosfet");
}

#endif


LibraryItem* ECMOSFET::libraryItemNEM() {
	return new LibraryItem(
	           "ec/nemosfet",
// 	i18n("n-e MOSFET"),
	           i18n("n-MOSFET"),
	           i18n("Discrete"),
	           "nemosfet.png",
	           LibraryItem::lit_component,
	           ECMOSFET::constructNEM);
}


LibraryItem* ECMOSFET::libraryItemPEM() {
	return new LibraryItem(
	           "ec/pemosfet",
// 	i18n("p-e MOSFET"),
	           i18n("p-MOSFET"),
	           i18n("Discrete"),
	           "pemosfet.png",
	           LibraryItem::lit_component,
	           ECMOSFET::constructPEM);
}


#if 0
LibraryItem* ECMOSFET::libraryItemNDM() {
	return new LibraryItem(
	           "ec/ndmosfet",
	           i18n("n-d MOSFET"),
	           i18n("Discrete"),
	           "ndmosfet.png",
	           LibraryItem::lit_component,
	           ECMOSFET::constructNDM);
}


LibraryItem* ECMOSFET::libraryItemPDM() {
	return new LibraryItem(
	           "ec/pdmosfet",
	           i18n("p-d MOSFET"),
	           i18n("Discrete"),
	           "pdmosfet.png",
	           LibraryItem::lit_component,
	           ECMOSFET::constructPDM);
}
#endif

ECMOSFET::ECMOSFET(int MOSFET_type, ICNDocument *icnDocument, bool newItem, const char *id)
		: Component(icnDocument, newItem, id) {
	m_MOSFET_type = MOSFET_type;

	switch ((MOSFET::MOSFET_type) m_MOSFET_type) {

	case MOSFET::neMOSFET: {
		m_name = i18n("N-Channel Enhancement MOSFET");
		break;
	}

	case MOSFET::peMOSFET: {
		m_name = i18n("P-Channel Enhancement MOSFET");
		break;
	}

#if 0
	case MOSFET::ndMOSFET: {
		m_name = i18n("N-Channel Depletion MOSFET");
		break;
	}

	case MOSFET::pdMOSFET: {
		m_name = i18n("P-Channel Depletion MOSFET");
		break;
	}
#endif

	}

	setSize(-8, -16, 16, 32);

	ECNode *NodeS = createPin(8, 24, 270, "s");

	m_pMOSFET = new MOSFET((MOSFET::MOSFET_type)m_MOSFET_type);
	setup4pinElement(m_pMOSFET, createPin(8, -24, 90, "d")->pin(),
				createPin(-16, 8, 0, "g")->pin(),
				NodeS->pin(), NodeS->pin());

	m_bHaveBodyPin = false;

	Variant *v = createProperty("bodyPin", Variant::Type::Bool);
	v->setCaption(i18n("mosfet body/bulk pin", "Body Pin"));
	v->setValue(false);

#if 0
	MOSFETSettings s; // will be created with the default settings
	v = createProperty("I_S", Variant::Type::Double);
	v->setCaption(i18n("Saturation Current"));
	v->setUnit("A");
	v->setMinValue(1e-20);
	v->setMaxValue(1e-0);
	v->setValue(s.I_S);
	v->setAdvanced(true);

	v = createProperty("N_F", Variant::Type::Double);
	v->setCaption(i18n("Forward Coefficient"));
	v->setMinValue(1e0);
	v->setMaxValue(1e1);
	v->setValue(s.N_F);
	v->setAdvanced(true);

	v = createProperty("N_R", Variant::Type::Double);
	v->setCaption(i18n("Reverse Coefficient"));
	v->setMinValue(1e0);
	v->setMaxValue(1e1);
	v->setValue(s.N_R);
	v->setAdvanced(true);

	v = createProperty("B_F", Variant::Type::Double);
	v->setCaption(i18n("Forward Beta"));
	v->setMinValue(1e-1);
	v->setMaxValue(1e3);
	v->setValue(s.B_F);
	v->setAdvanced(true);

	v = createProperty("B_R", Variant::Type::Double);
	v->setCaption(i18n("Reverse Beta"));
	v->setMinValue(1e-1);
	v->setMaxValue(1e3);
	v->setValue(s.B_R);
	v->setAdvanced(true);
#endif
}

ECMOSFET::~ECMOSFET() {
	delete m_pMOSFET;
}

void ECMOSFET::dataChanged() {
	bool haveBodyPin = dataBool("bodyPin");

	if (haveBodyPin != m_bHaveBodyPin) {
		m_bHaveBodyPin = haveBodyPin;

		if (m_bHaveBodyPin) {
			// Creating a body pin
			removeElement(m_pMOSFET, false);
			delete m_pMOSFET;

			m_pMOSFET = new MOSFET((MOSFET::MOSFET_type)m_MOSFET_type);
			setup4pinElement(m_pMOSFET, ecNodeWithID("d")->pin(),
				ecNodeWithID("g")->pin(), ecNodeWithID("s")->pin(),
				createPin(16, 0, 180, "b")->pin());

		} else {
			// Removing a body pin
			removeNode("b");
			removeElement(m_pMOSFET, false);
			delete m_pMOSFET;

		// Our class requires that we initialize four pins, even if we tie two of those pins together. 
			m_pMOSFET = new MOSFET((MOSFET::MOSFET_type)m_MOSFET_type);
			setup4pinElement(m_pMOSFET, ecNodeWithID("d")->pin(),
				ecNodeWithID("g")->pin(), ecNodeWithID("s")->pin(),
				ecNodeWithID("s")->pin());
		}
	}

#if 0
	MOSFETSettings s;

	s.I_S = dataDouble("I_S");
	s.N_F = dataDouble("N_F");
	s.N_R = dataDouble("N_R");
	s.B_F = dataDouble("B_F");
	s.B_R = dataDouble("B_R");

	m_pMOSFET->setMOSFETSettings(s);
#endif
}


void ECMOSFET::drawShape(QPainter &p) {
	const int _x = int(x());
	const int _y = int(y());

	initPainter(p);

	// Middle three horizontal lines
	p.drawLine(_x - 3, _y - 11, _x + 8, _y - 11);
	p.drawLine(_x - 3, _y, _x + 8, _y);
	p.drawLine(_x - 3, _y + 11, _x + 8, _y + 11);

	// Right middle vertical line

	if (m_bHaveBodyPin)
		p.drawLine(_x + 8, _y + 11, _x + 8, _y + 16);
	else	p.drawLine(_x + 8, _y, _x + 8, _y + 16);

	// Right top vertical line
	p.drawLine(_x + 8, _y - 11, _x + 8, _y - 16);

	QPen pen = p.pen();

	pen.setWidth(2);

	p.setPen(pen);

	// Back line
	p.drawLine(_x - 7, _y - 10, _x - 7, _y + 11);

	if (m_MOSFET_type == MOSFET::neMOSFET ||
	        m_MOSFET_type == MOSFET::peMOSFET) {
		// Middle three vertical lines
		p.drawLine(_x - 2, _y - 14, _x - 2, _y - 7);
		p.drawLine(_x - 2, _y - 3, _x - 2, _y + 4);
		p.drawLine(_x - 2, _y + 8, _x - 2, _y + 15);
	} else {
		// Middle vertical line
		p.drawLine(_x - 3, _y - 14, _x - 3, _y + 15);
	}

	QPointArray pa(3);

	if (m_MOSFET_type == MOSFET::neMOSFET /*||
			m_MOSFET_type == MOSFET::ndMOSFET*/) {
		// Inwards facing arrow
		pa[0] = QPoint(0, 0);
		pa[1] = QPoint(5, -3);
		pa[2] = QPoint(5, 3);
	} else {
		// Outwards facing arrow
		pa[0] = QPoint(2, -3);
		pa[1] = QPoint(7, 0);
		pa[2] = QPoint(2, 3);
	}

	pa.translate(_x, _y);

	p.setPen(p.pen().color());
	p.setBrush(p.pen().color());
	p.drawPolygon(pa);

	deinitPainter(p);
}

