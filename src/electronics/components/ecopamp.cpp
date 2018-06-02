/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecopamp.h"

#include "ecnode.h"
#include "libraryitem.h"

#include <klocalizedstring.h>
#include <qpainter.h>

Item* ECOpAmp::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECOpAmp( (ICNDocument*)itemDocument, newItem, id );
}


LibraryItem* ECOpAmp::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/opamp")),
		i18n("Op Amp"),
		i18n("Integrated Circuits"),
		"opamp.png",
		LibraryItem::lit_component,
		ECOpAmp::construct );
}


ECOpAmp::ECOpAmp( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "opamp" )
{
	m_name = i18n("Operational Amplifier");
	
	QPolygon pa(3);
	pa[0] = QPoint( -16, -16 );
	pa[1] = QPoint( 16, 0 );
	pa[2] = QPoint( -16, 16 );
	setItemPoints( pa, true );
	
	init2PinLeft( -8, 8 );
	init1PinRight();
	createOpAmp( m_pNNode[0], m_pPNode[0], m_pNNode[1] );
}


ECOpAmp::~ECOpAmp()
{
}


void ECOpAmp::drawShape( QPainter & p )
{
	initPainter(p);
	
	int _x = int(x());
	int _y = int(y());
	
	QPolygon pa(3);
	pa[0] = QPoint( _x-16, _y-16 );
	pa[1] = QPoint( _x+16, _y );
	pa[2] = QPoint( _x-16, _y+16 );
	
	p.drawPolygon(pa);
	p.drawPolyline(pa);
	
	// Plus symbol
	p.drawLine( _x-9, _y-8, _x-9, _y-2 );
	p.drawLine( _x-12, _y-5, _x-6, _y-5 );
	
	// Minus symbol
	p.drawLine( _x-11, _y+6, _x-7, _y+6 );
	
	deinitPainter(p);
}


