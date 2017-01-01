/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "resistor.h"

#include "libraryitem.h"
#include "resistance.h"

#include <klocalizedstring.h>
#include <Qt/qpainter.h>

Item* Resistor::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new Resistor( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* Resistor::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/resistor")),
		i18n("Resistor"),
		i18n("Passive"),
		"resistor.png",
		LibraryItem::lit_component,
		Resistor::construct );
}

Resistor::Resistor( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "resistor" )
{
	m_name = i18n("Resistor");
	setSize( -16, -8, 32, 16 );
	
	init1PinLeft();
	init1PinRight();
	m_resistance = createResistance( m_pPNode[0], m_pNNode[0], 1. );
	
	createProperty( "resistance", Variant::Type::Double );
	property("resistance")->setCaption( i18n("Resistance") );
	property("resistance")->setUnit( QChar(0x3a9) );
	property("resistance")->setValue(1e4);
	property("resistance")->setMinValue(1e-6);
	
	addDisplayText( "res", QRect( -16, -22, 32, 12 ), "", false );
}

Resistor::~Resistor()
{
}

void Resistor::dataChanged()
{
	double resistance = dataDouble("resistance");
	
	QString display = QString::number( resistance / getMultiplier(resistance), 'g', 3 ) + getNumberMag(resistance) + QChar(0x3a9);
	setDisplayText( "res", display );
	
	m_resistance->setResistance(resistance);
}

void Resistor::drawShape( QPainter &p )
{
	initPainter(p);
	p.drawRect( (int)x()-16, (int)y()-6, width(), 12 );
	deinitPainter(p);
}


