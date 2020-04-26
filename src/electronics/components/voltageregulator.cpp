/***************************************************************************
 *   Copyright (C) 2006 by William Hillerby - william.hillerby@ntlworld.com*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "voltageregulator.h"

#include "canvasitemparts.h"
#include "libraryitem.h"
#include "ecnode.h"

#include <KLocalizedString>

#include <QPainter>
#include <QStyle>
#include <QDebug>

Item* VoltageRegulator::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new VoltageRegulator( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* VoltageRegulator::libraryItem()
{
	return new LibraryItem(
			QStringList(QString("ec/voltageregulator")),
	i18n("Voltage Regulator"),
	i18n("Passive"),
	"voltage_regulator.png",
	LibraryItem::lit_component,
	VoltageRegulator::construct 
						  );
}

VoltageRegulator::VoltageRegulator( ICNDocument* icnDocument, bool newItem, const QString& id )
	: Component( icnDocument, newItem, (!id.isEmpty()) ? id : "voltageregulator" )
{
	
	createProperty( "voltageout",  Variant::Type::Double );
	property("voltageout")->setCaption( i18n( "Voltage Out" ) );
	property("voltageout")->setMinValue( 2 );
	property("voltageout")->setMaxValue( maxVoltageOut );
	property("voltageout")->setValue( 5 );
	
}

VoltageRegulator::~VoltageRegulator()
{
}

void VoltageRegulator::dataChanged()
{
}
	
void VoltageRegulator::drawShape( QPainter &p )
{
	initPainter(p);
	
	// Get centre point of component.
	//int _y = (int)y();
	//int _x = (int)x();
	
	deinitPainter(p);
}


