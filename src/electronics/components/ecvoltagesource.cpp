/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecvoltagesource.h"

#include "ecnode.h"
#include "voltagesource.h"
#include "libraryitem.h"
#include "pin.h"

#include <klocalizedstring.h>
#include <Qt/qpainter.h>

Item* ECCell::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECCell( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECCell::libraryItem()
{
	QStringList ids;
	ids << "ec/battery" << "ec/cell";
	return new LibraryItem(
		ids,
		i18n("Battery"),
		i18n("Sources"),
		"cell.png",
		LibraryItem::lit_component,
		ECCell::construct );
}

ECCell::ECCell( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "cell" )
{
	m_name = i18n("Battery");
	setSize( -8, -8, 16, 16 );
	voltage = 0;

	init1PinLeft();
	init1PinRight();
	
	m_pNNode[0]->pin()->setGroundType( Pin::gt_medium );
	m_voltageSource = createVoltageSource( m_pNNode[0], m_pPNode[0], voltage );
	
	createProperty( "voltage", Variant::Type::Double );
	property("voltage")->setUnit("V");
	property("voltage")->setCaption( i18n("Voltage") );
	property("voltage")->setMinValue(-1e12);
	property("voltage")->setMaxValue(1e12);
	property("voltage")->setValue(5.0);
	
	addDisplayText( "voltage", QRect( -16, -24, 32, 16 ), "" );
}

ECCell::~ECCell()
{
}

void ECCell::dataChanged()
{
	voltage = dataDouble("voltage");
	m_voltageSource->setVoltage(voltage);
	
	QString display = QString::number( voltage / getMultiplier(voltage), 'g', 3 ) + getNumberMag(voltage) + "V";
	setDisplayText( "voltage", display );
}

void ECCell::drawShape( QPainter &p )
{
	initPainter(p);
	
	int _x = (int)x()-8;
	int _y = (int)y()-24;
	
	p.drawLine( _x,		_y+20,	_x,	_y+28 );
	p.drawLine( _x+5,	_y+16,	_x+5,	_y+32 );
	p.drawLine( _x+10,	_y+20,	_x+10,	_y+28 );
	p.drawLine( _x+15,	_y+16,	_x+15,	_y+32 );
	
	deinitPainter(p);
// 	p.drawPolyline( areaPoints() );
}

