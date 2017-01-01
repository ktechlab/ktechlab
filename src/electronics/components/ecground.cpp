/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecground.h"

#include "ecnode.h"
#include "libraryitem.h"
#include "pin.h"

#include <klocalizedstring.h>
#include <Qt/qpainter.h>

Item* ECGround::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECGround( (ICNDocument*)itemDocument, newItem, id );
}
LibraryItem* ECGround::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/ground")),
		i18n("Ground (0V)"),
		i18n("Sources"),
		"ground.png",
		LibraryItem::lit_component,
		ECGround::construct );
}

ECGround::ECGround( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, (id) ? id : "ground" )
{
	m_name = i18n("Ground");
	setSize( -8, -8, 16, 16 );
	init1PinRight();
	m_pPNode[0]->pin()->setGroundType( Pin::gt_always );
	setAngleDegrees(270);
}

ECGround::~ECGround()
{
}

void ECGround::drawShape( QPainter &p )
{
	initPainter(p);
	int _x = (int)x()-8;
	int _y = (int)y()-8;
	QPen pen;
	pen.setWidth(2);
	pen.setColor( p.pen().color() );
	p.setPen(pen);
	p.drawLine( _x+15, _y, _x+15, _y+16 );
	p.drawLine( _x+10, _y+3, _x+10, _y+13 );
	p.drawLine( _x+5, _y+6, _x+5, _y+10 );
	deinitPainter(p);
}



