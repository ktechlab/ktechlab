/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <klocale.h>
#include <qpainter.h>

#include "simulator.h"
#include "capacitance.h"
#include "capacitor.h"
#include "ecnode.h"
#include "libraryitem.h"

Item* Capacitor::construct(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new Capacitor((ICNDocument*)itemDocument, newItem, id);
}

LibraryItem* Capacitor::libraryItem() {
	return new LibraryItem(
	           "ec/capacitor",
	           i18n("Capacitor"),
	           i18n("Passive"),
	           "capacitor.png",
	           LibraryItem::lit_component,
	           Capacitor::construct
	       );
}

Capacitor::Capacitor(ICNDocument *icnDocument, bool newItem, const char *id)
		: SimpleComponent(icnDocument, newItem, id ? id : "capacitor") {
	m_name = i18n("Capacitor");
	setSize(-8, -8, 16, 16);

	init1PinLeft();
	init1PinRight();

	m_capacitance = new Capacitance(0.001, LINEAR_UPDATE_PERIOD);
	setup2pinElement(m_capacitance, m_pNNode[0]->pin(), m_pPNode[0]->pin());

	createProperty("Capacitance", Variant::Type::Double);
	property("Capacitance")->setCaption(i18n("Capacitance"));
	property("Capacitance")->setUnit("F");
	property("Capacitance")->setMinValue(1e-12);
	property("Capacitance")->setMaxValue(1e12);
	property("Capacitance")->setValue(1e-3);

	addDisplayText("capacitance", QRect(-8, -24, 16, 16), "", false);
}

Capacitor::~Capacitor() {
	delete m_capacitance;
}

void Capacitor::dataChanged() {
	double capacitance = dataDouble("Capacitance");

	QString display = QString::number(capacitance / getMultiplier(capacitance), 'g', 3) + getNumberMag(capacitance) + "F";
	setDisplayText("capacitance", display);

	m_capacitance->setCapacitance(capacitance);
}

void Capacitor::drawShape(QPainter &p) {
	initPainter(p);

	int _y = (int)y() - 8;
	int _x = (int)x() - 8;

	QPen pen;
	pen.setWidth(1);
	pen.setColor(p.pen().color());
	p.setPen(pen);
	p.drawRect(_x, _y, 5, 16);
	p.drawRect(_x + 11, _y, 5, 16);

	deinitPainter(p);
}

