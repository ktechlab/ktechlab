/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "capacitance.h"
#include "eccapacitor.h"
#include "ecnode.h"
#include "libraryitem.h"

#include <klocale.h>
#include <qpainter.h>

Item* ECCapacitor::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECCapacitor( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECCapacitor::libraryItem()
{
	return new LibraryItem(
		"ec/capacitor",
		i18n("Capacitor"),
		i18n("Discrete"),
		"capacitor.png",
		LibraryItem::lit_component,
		ECCapacitor::construct
			);
}

ECCapacitor::ECCapacitor( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "capacitor" )
{
	m_name = i18n("Capacitor");
	m_desc = i18n("Stores electrical charge.<br><br>"
				   "The voltage across the capacitor and capacitance are related by <i>Charge = Capacitance x Voltage</i>.");
	setSize( -8, -8, 16, 16 );
	
	init1PinLeft();
	init1PinRight();
	
	m_capacitance = createCapacitance( m_pNNode[0], m_pPNode[0], 0.001 );
	
	createProperty( "Capacitance", Variant::Type::Double );
	property("Capacitance")->setCaption( i18n("Capacitance") );
	property("Capacitance")->setUnit("F");
	property("Capacitance")->setMinValue(1e-12);
	property("Capacitance")->setMaxValue(1e12);
	property("Capacitance")->setValue(1e-3);
	
	addDisplayText( "capacitance", QRect( -8, -24, 16, 16 ), "", false );
}

ECCapacitor::~ECCapacitor()
{
}

void ECCapacitor::dataChanged()
{
	double capacitance = dataDouble("Capacitance");
	
	QString display = QString::number( capacitance / getMultiplier(capacitance), 'g', 3 ) + getNumberMag(capacitance) + "F";
	setDisplayText( "capacitance", display );
	
	m_capacitance->setCapacitance(capacitance);
}

void ECCapacitor::drawShape( QPainter &p )
{
	initPainter(p);
	
	int _y = (int)y()-8;
	int _x = (int)x()-8;
	
	QPen pen;
	pen.setWidth(1);
	pen.setColor( p.pen().color() );
	p.setPen(pen);
	p.drawRect( _x, _y, 5, 16 );
	p.drawRect( _x+11, _y, 5, 16 );
	
	
	deinitPainter(p);
// 	p.drawPolyline( areaPoints() );
}


