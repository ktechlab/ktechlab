/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecfixedvoltage.h"

#include "ecnode.h"
#include "voltagepoint.h"
#include "libraryitem.h"

#include <klocale.h>
#include <qpainter.h>

Item* ECFixedVoltage::construct( ItemDocument *itemDocument, bool newItem, const char *id)
{
	return new ECFixedVoltage( (ICNDocument*)itemDocument, newItem, id);
}
LibraryItem* ECFixedVoltage::libraryItem()
{
	return new LibraryItem(
		"ec/fixed_voltage",
		i18n("Fixed Voltage"),
		i18n("Sources"),
		"voltage.png",
		LibraryItem::lit_component,
		ECFixedVoltage::construct);
}

ECFixedVoltage::ECFixedVoltage( ICNDocument *icnDocument, bool newItem, const char *id)
	: Component( icnDocument, newItem, id ? id : "fixed_voltage")
{
	m_name = i18n("Fixed Voltage");
	m_desc = i18n("Provides a fixed voltage point to connect components to.");
	setSize( -8, -8, 16, 16);
	
	init1PinRight();
	m_voltagePoint = createVoltagePoint( m_pPNode[0], 5.0);
	
	addDisplayText( "voltage", QRect( -24, -20, width()+32, 12), "");
	
	createProperty( "voltage", Variant::Type::Double);
	property("voltage")->setUnit("V");
	property("voltage")->setCaption( i18n("Voltage"));
	property("voltage")->setMinValue(-1e15);
	property("voltage")->setMaxValue(1e15);
	property("voltage")->setValue(5.0);
}

ECFixedVoltage::~ECFixedVoltage()
{
}

void ECFixedVoltage::dataChanged()
{
	const double voltage = dataDouble("voltage");
	QString display = QString::number( voltage / getMultiplier(voltage), 'g', 3) + getNumberMag(voltage) + "V";
	setDisplayText( "voltage", display);
	m_voltagePoint->setVoltage(voltage);
}

void ECFixedVoltage::drawShape( QPainter &p)
{
	initPainter(p);
	int _x = int(x());
	int _y = int(y());
	p.drawEllipse( _x-4, _y-4, 8, 8);
	p.setPen( m_pPNode[0]->isSelected() ? m_selectedCol : Qt::black);
	p.drawLine( _x+4, _y, _x+8, _y);
	deinitPainter(p);
}

