/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "eccurrentsource.h"
#include "ecnode.h"
#include "libraryitem.h"
#include "pin.h"

#include <klocale.h>
#include <qpainter.h>

Item* ECCurrentSource::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECCurrentSource( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECCurrentSource::libraryItem()
{
	return new LibraryItem(
		"ec/current_source",
		i18n("Current Source"),
		i18n("Sources"),
		"current_source.png",
		LibraryItem::lit_component,
		ECCurrentSource::construct );
}

ECCurrentSource::ECCurrentSource(ICNDocument *icnDocument, bool newItem, const char *id)
	: SimpleComponent(icnDocument, newItem, id ? id : "current_source") ,
                          m_currentSource(0.02)
{
	m_name = i18n("Current Source");
	setSize(-16, -8, 24, 24);

	init1PinLeft(8);
	init1PinRight(8);
	m_pNNode[0]->pin()->setGroundType(Pin::gt_low);

	setup2pinElement(m_currentSource, m_pNNode[0]->pin(), m_pPNode[0]->pin());

	createProperty("current", Variant::Type::Double);
	property("current")->setCaption(i18n("Current"));
	property("current")->setUnit("A");
	property("current")->setMinValue(-1e12);
	property("current")->setMaxValue(1e12);
	property("current")->setValue(0.02);
	
	addDisplayText("current", QRect(-16, -16, 24, 0), "");
}


ECCurrentSource::~ECCurrentSource()
{
}

void ECCurrentSource::dataChanged()
{
	double current = dataDouble("current");
	m_currentSource.setCurrent(current);
	
	QString display = QString::number( current / getMultiplier(current), 'g', 3 ) + getNumberMag(current) + "A";
	setDisplayText("current", display );
}

void ECCurrentSource::drawShape( QPainter &p )
{
	initPainter(p);
	
	int _x = (int)x()-16;
	int _y = (int)y()-24;
	
	// Top arrow indicating current direction
	p.drawLine( _x+width(), _y+19, _x, _y+19 );
	p.drawLine( _x+width(), _y+19, _x+width()-3, _y+16 );
	p.drawLine( _x+width(), _y+19, _x+width()-3, _y+22 );
	
	// Double circules
	p.drawEllipse( _x, _y+24, 16, 16 );
	p.drawEllipse( _x+8, _y+24, 16, 16 );
	
	deinitPainter(p);
}



