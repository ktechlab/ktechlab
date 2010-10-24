/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "colorcombo.h"
#include "diode.h"
#include "led.h"
#include "ecnode.h"
#include "ecsevensegment.h"
#include "libraryitem.h"
#include "simulator.h"

#include <klocale.h>
#include <qpainter.h>
#include <qstring.h>

Item* ECSevenSegment::construct(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new ECSevenSegment((ICNDocument*)itemDocument, newItem, id);
}

LibraryItem* ECSevenSegment::libraryItem() {
	return new LibraryItem(
	           "ec/seven_segment",
	           i18n("Seven Segment"),
	           i18n("Outputs"),
	           "seven_segment.png",
	           LibraryItem::lit_component,
	           ECSevenSegment::construct);
}

ECSevenSegment::ECSevenSegment(ICNDocument *icnDocument, bool newItem, const char *id)
		: DIPComponent(icnDocument, newItem, id ? id : "seven_segment") {
	m_name = i18n("Seven Segment LED");
	m_bDynamicContent = true;

	createProperty("0-color", Variant::Type::Color);
	property("0-color")->setCaption(i18n("Color"));
	property("0-color")->setColorScheme(ColorCombo::LED);

	createProperty("diode-polarity", Variant::Type::Select);
	property("diode-polarity")->setCaption(i18n("Configuration"));
	QStringMap allowed;
	allowed["Common Cathode"] = i18n("Common Cathode");
	allowed["Common Anode"] = i18n("Common Anode");
	property("diode-polarity")->setAllowed(allowed);
	property("diode-polarity")->setValue("Common Cathode");

	for (int i = 0; i < 8; i++) {
		m_diodes[i] = 0;
		m_nodes[i] = 0;
		avg_brightness[i] = 0.;
		last_brightness[i] = 255;
	}

	m_nNode = 0;
	lastUpdatePeriod = 0.;
	QStringList pins = QStringList::split(',', "g,f,e,d," + QString(QChar(0xB7)) + ",c,b,a");

	initDIPSymbol(pins, 64);
	initDIP(pins);

	m_nNode = createPin(width() / 2 + offsetX(), height() + 8 + offsetY(), 270, "-v");

	for (int i = 0; i < 7; i++)
		m_nodes[i] = ecNodeWithID(QChar('a' + i));

	m_nodes[7] = ecNodeWithID(QChar(0xB7));

	m_bCommonCathode = false; // Force update

for (int i = 0; i < 8; i++) {
m_diodes[i] = new Diode();}

}

ECSevenSegment::~ECSevenSegment() {}

void ECSevenSegment::dataChanged() {
	QColor color = dataColor("0-color");
	r = color.red() / 0x100;
	g = color.green() / 0x100;
	b = color.blue() / 0x100;

	bool commonCathode = dataString("diode-polarity") == "Common Cathode";

	if (commonCathode != m_bCommonCathode) {
		m_bCommonCathode = commonCathode;

		for (int i = 0; i < 8; i++) {
			removeElement(m_diodes[i], false);

			if (commonCathode)
				setup2pinElement(*(m_diodes[i]), m_nodes[i]->pin(), m_nNode->pin());
			else	setup2pinElement(*(m_diodes[i]), m_nNode->pin(), m_nodes[i]->pin());
		}
	}

	update();
}

void ECSevenSegment::stepNonLogic() {
	if (!m_diodes[0]) return;

	for (int i = 0; i < 8; i++)
		avg_brightness[i] += LED::brightnessFromCurrent(m_diodes[i]->current());

	lastUpdatePeriod++;
}

void ECSevenSegment::drawShape(QPainter &p) {
	CNItem::drawShape(p);

	initPainter(p);

	const int _width = 20;
	const int _height = 32;

	const int x1 = (int)x() + offsetX() + (width() - _width) / 2 - 1;
	const int x2 = x1 + _width;
	const int y1 = (int)y() + offsetY() + (height() - _height) / 2;
	const int y2 = y1 + _height / 2;
	const int y3 = y1 + _height;
	const int ds = 2; // "Slope"

// 	QPen pen;
// 	pen.setWidth(2);
// 	pen.setCapStyle(Qt::RoundCap);
// 	p.setPen(pen);

	if (lastUpdatePeriod != 0.) {
		for (uint i = 0; i < 8; ++i) {
			last_brightness[i] = (uint)(avg_brightness[i] / lastUpdatePeriod);
	
			avg_brightness[i] = 0;
		}
	}

	lastUpdatePeriod = 0.;

	double _b;

	// Top
	_b = last_brightness[0];
	p.setPen(QPen(QColor(uint(255 - (255 - _b) * (1 - r)), 
			     uint(255 - (255 - _b) * (1 - g)), 
			     uint(255 - (255 - _b) * (1 - b))
			    ), 2));

	p.drawLine(x1 + 3 + ds, y1, x2 - 3 + ds, y1);

	// Top right
	_b = last_brightness[1];
	p.setPen(QPen(QColor(uint(255 - (255 - _b) * (1 - r)), 
			     uint(255 - (255 - _b) * (1 - g)), 
			     uint(255 - (255 - _b) * (1 - b))), 2));
	p.drawLine(x2 + 0 + ds, y1 + 3, x2 + 0, y2 - 3);

	// Bottom right
	_b = last_brightness[2];
	p.setPen(QPen(QColor(uint(255 - (255 - _b) * (1 - r)), uint(255 - (255 - _b) * (1 - g)), uint(255 - (255 - _b) * (1 - b))), 2));
	p.drawLine(x2 + 0, y2 + 3, x2 + 0 - ds, y3 - 3);

	// Bottom
	_b = last_brightness[3];
	p.setPen(QPen(QColor(uint(255 - (255 - _b) * (1 - r)), uint(255 - (255 - _b) * (1 - g)), uint(255 - (255 - _b) * (1 - b))), 2));
	p.drawLine(x2 - 3 - ds, y3 + 0, x1 + 3 - ds, y3 + 0);

	// Bottom left
	_b = last_brightness[4];
	p.setPen(QPen(QColor(uint(255 - (255 - _b) * (1 - r)), uint(255 - (255 - _b) * (1 - g)), uint(255 - (255 - _b) * (1 - b))), 2));
	p.drawLine(x1 + 0 - ds, y3 - 3, x1 + 0, y2 + 3);

	// Top left
	_b = last_brightness[5];
	p.setPen(QPen(QColor(uint(255 - (255 - _b) * (1 - r)), uint(255 - (255 - _b) * (1 - g)), uint(255 - (255 - _b) * (1 - b))), 2));
	p.drawLine(x1 + 0, y2 - 3, x1 + 0 + ds, y1 + 3);

	// Middle
	_b = last_brightness[6];
	p.setPen(QPen(QColor(uint(255 - (255 - _b) * (1 - r)), uint(255 - (255 - _b) * (1 - g)), uint(255 - (255 - _b) * (1 - b))), 2));
	p.drawLine(x1 + 3, y2 + 0, x2 - 3, y2 + 0);

	// Decimal point
	_b = last_brightness[7];
	p.setBrush(QBrush(QColor(uint(255 - (255 - _b)*(1 - r)), uint(255 - (255 - _b) * (1 - g)), uint(255 - (255 - _b) * (1 - b)))));
	p.setPen(Qt::NoPen);
	p.drawPie(x2 + 3, y3 - 2, 3, 3, 0, 16 * 360);

	deinitPainter(p);
}
