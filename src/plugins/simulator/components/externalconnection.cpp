/***************************************************************************
 *   Copyright (C) 2004 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "externalconnection.h"
#include "libraryitem.h"

#include <klocale.h>
#include <qpainter.h>

Item* ExternalConnection::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ExternalConnection( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ExternalConnection::libraryItem()
{
	return new LibraryItem(
		"ec/external_connection",
		i18n("External Connection"),
		i18n("Connections"),
		"external_connection.png",
		LibraryItem::lit_component,
		ExternalConnection::construct );
}

ExternalConnection::ExternalConnection( ICNDocument *icnDocument, bool newItem, const char *id )
	: SimpleComponent( icnDocument, newItem, id ? id : "external_connection" )
{
	m_name = i18n("External Connection");
	setSize( -8, -8, 16, 16 );
	
	createProperty( "name", Variant::Type::Combo );
	property("name")->setCaption( i18n("Name") );
	property("name")->setValue("ExtCon");

	init1PinLeft();
	
	addDisplayText( "name", QRect( -24, 8, 3*width(), 16 ), "ExtCon" );
}

ExternalConnection::~ExternalConnection()
{
}


void ExternalConnection::dataChanged()
{
	QString name = dataString("name");
	
	QRect r( -width(), 16, 3*width(), 16 );
	setDisplayText( "name", name );
}


void ExternalConnection::drawShape( QPainter &p )
{
	initPainter(p);
	int _x = (int)x()-8;
	int _y = (int)y()-8;
	p.drawEllipse( _x, _y, width(), height() );
	
	p.drawLine( _x+3, _y+6, _x+12, _y+6 );
	p.drawLine( _x+8, _y+3, _x+12, _y+6 );
	
	p.drawLine( _x+3, _y+9, _x+12, _y+9 );
	p.drawLine( _x+3, _y+9, _x+8, _y+12 );
	
	deinitPainter(p);
}

