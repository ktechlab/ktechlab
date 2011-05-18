/***************************************************************************
 *   Copyright (C) 2006 by William Hillerby - william.hillerby@ntlworld.com*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ledbargraphdisplay.h"
#include "colorcombo.h"
#include "libraryitem.h"
#include "simulator.h"
#include "led.h"

#include <klocale.h>
#include <qpainter.h>
#include <qstyle.h>
#include <kdebug.h>

//BEGIN LEDPart
LEDPart::LEDPart(Component *pParent, const QString& strPNode, const QString& strNNode)
	: m_pParent(pParent), m_strPNode(strPNode), m_strNNode(strNNode)
{
	pParent->setup2pinElement(m_pDiode,
		pParent->ecNodeWithID(strPNode)->pin(),
		pParent->ecNodeWithID(strNNode)->pin());

	avg_brightness = 255;
	lastUpdatePeriod = 0.;
	last_brightness = 255;
	r = g = b = 0;
}

LEDPart::~LEDPart() {
	m_pParent->removeNode(m_strPNode);
	m_pParent->removeNode(m_strNNode);
	m_pParent->removeElement(&m_pDiode, false);
}

void LEDPart::setDiodeSettings(const DiodeSettings& ds) {
	m_pDiode.setDiodeSettings(ds);
}

void LEDPart::setColor(const QColor &color) {
	r = color.red()   / (double)0x100;
	g = color.green() / (double)0x100;
	b = color.blue()  / (double)0x100;
}

void LEDPart::step() {
	avg_brightness += LED::brightnessFromCurrent(m_pDiode.current()) * LINEAR_UPDATE_PERIOD;
	lastUpdatePeriod += LINEAR_UPDATE_PERIOD;
}

void LEDPart::draw(QPainter &p, int x, int y, int w, int h) {
	uint _b;

	if (lastUpdatePeriod == 0.)
		_b = last_brightness;
	else {
		_b = (uint)(avg_brightness / lastUpdatePeriod);
		last_brightness = _b;
	}

	avg_brightness = 0.;
	lastUpdatePeriod = 0.;

	p.setBrush(QColor(uint(255 - (255 - _b)*(1 - r)), uint(255 - (255 - _b)*(1 - g)), uint(255 - (255 - _b)*(1 - b))));
	p.drawRect(x, y, w, h);
}
//END LEDPart

//BEGIN LEDBarGraphDisplay
Item* LEDBarGraphDisplay::construct(ItemDocument *itemDocument, bool newItem, const char *id) {
	return new LEDBarGraphDisplay((ICNDocument*)itemDocument, newItem, id);
}

LibraryItem* LEDBarGraphDisplay::libraryItem() {
	return new LibraryItem(
	           "ec/bar_graph_display",
	           i18n("Bar Graph Display"),
	           i18n("Outputs"),
	           "bar_graph_display.png",
	           LibraryItem::lit_component,
	           LEDBarGraphDisplay::construct
	       );
}

LEDBarGraphDisplay::LEDBarGraphDisplay(ICNDocument* icnDocument, bool newItem, const QString& id)
		: DIPComponent(icnDocument, newItem, id ? id : "bar_graph_display") {
	m_name = i18n("Bar Graph Display");
	m_bDynamicContent = true;

	m_numRows = 0;

	for (unsigned i = 0; i < max_LED_rows; i++)
		m_LEDParts[i] = 0;

	// Create a Row property.
	createProperty("rows", Variant::Type::Int);
	property("rows")->setCaption(i18n("Rows"));
	property("rows")->setMinValue(1);
	property("rows")->setMaxValue(max_LED_rows);
	property("rows")->setValue(7);

	createProperty("color", Variant::Type::Color);
	property("color")->setCaption(i18n("Color"));
	property("color")->setColorScheme(ColorCombo::LED);
	DiodeSettings ds;

	createProperty("I_S", Variant::Type::Double);
	property("I_S")->setCaption("Saturation Current");
	property("I_S")->setUnit("A");
	property("I_S")->setMinValue(1e-20);
	property("I_S")->setMaxValue(1e-0);
	property("I_S")->setValue(ds.I_S);
	property("I_S")->setAdvanced(true);

	createProperty("N", Variant::Type::Double);
	property("N")->setCaption(i18n("Emission Coefficient"));
	property("N")->setMinValue(1e0);
	property("N")->setMaxValue(1e1);
	property("N")->setValue(ds.N);
	property("N")->setAdvanced(true);

	createProperty("V_B", Variant::Type::Double);
	property("V_B")->setCaption(i18n("Breakdown Voltage"));
	property("V_B")->setUnit("V");
	property("V_B")->setMinAbsValue(1e-5);
	property("V_B")->setMaxValue(1e10);
	property("V_B")->setValue(ds.V_B);
	property("V_B")->setAdvanced(true);
}

LEDBarGraphDisplay::~LEDBarGraphDisplay() {

// FIXME: why does this crash?
/*
	for (unsigned i = 0; i < m_numRows; i++)
		delete m_LEDParts[i];
*/
}

void LEDBarGraphDisplay::dataChanged() {
	DiodeSettings ds;
	QColor color = dataColor("color");

	ds.I_S = dataDouble("I_S");
	ds.V_B = dataDouble("V_B");
	ds.N = dataDouble("N");

	initPins();

	// Update each diode in array with new diode setting as they are acting individually.

	for (unsigned i = 0; i < m_numRows; i++) {
		m_LEDParts[i]->setDiodeSettings(ds);
		m_LEDParts[i]->setColor(color);
	}
}

void LEDBarGraphDisplay::initPins() {
	unsigned int numRows = dataInt("rows");

	if (numRows == m_numRows)
		return;

	if (numRows > max_LED_rows)
		numRows = max_LED_rows;

	if (numRows < 1)
		numRows = 1;

	// Create a list of named pins.
	// A default setup looks like:
	//       -------
	// p_0--|1    14|--n_0
	// p_1--|2    13|--n_1
	// p_2--|3    12|--n_2
	// p_3--|4    11|--n_3
	// p_4--|5    10|--n_4
	// p_5--|6     9|--n_5
	// p_6--|7     8|--n_6
	//		 -------
	// And this is the scheme used to create the nodes and diodes.

	// Create the positive & negative pin names in an anticlockwise fashion
	// as shown in the pin schematic above.
	QStringList pins;

	for (unsigned i = 0; i < numRows; i++)
		pins += QString("p_" + QString::number(i));

	for (int i = numRows - 1; i >= 0; i--)
		pins += QString("n_" + QString::number(i));

	// Set the size of the component *BEFORE* initDIP() is called
	// as initDIP() uses this data to initialise the pin positions.
	setSize(-16, -(numRows + 1) * 8, 32, (numRows + 1) * 16, true);

	// Create the nodes.
	initDIP(pins);

	// Create or remove LED parts
	if (numRows > m_numRows) {
		// Create the extra LED parts required.
		for (unsigned i = m_numRows; i < numRows; i++)
			m_LEDParts[i] = new LEDPart((Component*)this, QString("p_" + QString::number(i)),
		                            QString("n_" + QString::number(i)));
	} else {
		// Remove excess LED parts.
		for (unsigned i = numRows; i < m_numRows; i++) {
			delete m_LEDParts[i];
			m_LEDParts[i] = 0;
		}
	}

	m_numRows = numRows;
}

void LEDBarGraphDisplay::stepNonLogic() {
	// Update LED brightnesses.
	for (unsigned i = 0; i < m_numRows; i++)
		m_LEDParts[i]->step();
}

void LEDBarGraphDisplay::drawShape(QPainter &p) {
	Component::drawShape(p);
	initPainter(p);

	// Init _x and _y to top left hand corner of component.
	int _x = (int)(x() + offsetX());
	int _y = (int)(y() + offsetY());

	// Draw LED elements, passing in a position for each.

	for (unsigned i = 0; i < m_numRows; i++)
		m_LEDParts[i]->draw(p, _x + 4, _y + (i*16) + 10, 24, 12);

	deinitPainter(p);
}
//END LEDBarGraphDisplay

