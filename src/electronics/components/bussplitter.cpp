/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "bussplitter.h"
#include "ecnode.h"
#include "libraryitem.h"
#include "wire.h"

#include <klocale.h>
#include <Qt/qpainter.h>

Item* BusSplitter::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new BusSplitter( (ICNDocument*)itemDocument, newItem, id );
}


LibraryItem* BusSplitter::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/bus")),
		i18n("Bus"),
		i18n("Connections"),
		"bus.png",
		LibraryItem::lit_component,
		BusSplitter::construct );
}


const unsigned MAX_BUS_SIZE = 10000;


BusSplitter::BusSplitter( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "Bus" )
{
	m_name = i18n("Bus Splitter");

	m_busSize = 0;	
	init1PinLeft();
	m_pInNode = m_pNNode[0];
	
	createProperty( "size", Variant::Type::Int );
	property("size")->setCaption( i18n("Size") );
	property("size")->setMinValue(1);
	property("size")->setMaxValue(MAX_BUS_SIZE);
	property("size")->setValue(8);
}


BusSplitter::~BusSplitter()
{
}


void BusSplitter::dataChanged()
{
	unsigned busSize = dataInt("size");
	
	if ( busSize < 1 )
		busSize = 1;
	
	else if ( busSize > MAX_BUS_SIZE )
		busSize = MAX_BUS_SIZE;
	
	if ( busSize == m_busSize )
		return;
	
	m_pInNode->setNumPins(busSize);
	
	if ( busSize > m_busSize )
	{
		m_pWires.resize(busSize);
		for ( unsigned i = m_busSize; i < unsigned(busSize); i++ )
		{
			Pin * pin = createPin( 16, 0, 180, outNodeID(i) )->pin();
			m_pWires[i] = new Wire( m_pInNode->pin(i), pin );
		}
	}
	else
	{
		for ( unsigned i = busSize; i < unsigned(m_busSize); i++ )
		{
			removeNode( outNodeID(i) );
			delete m_pWires[i];
		}
		m_pWires.resize(busSize);
	}
	m_busSize = busSize;
	
	// Position pins
	setSize( 0, -int(m_busSize+1)*8, 8, int(m_busSize+1)*16, true );
	for ( int i = 0; i < int(m_busSize); i++ )
		m_nodeMap[ outNodeID(i) ].y = 16*i - int(m_busSize+1)*8 + 24;
	m_nodeMap["n1"].y = -int(m_busSize+1)*8 + 8;
	
	updateAttachedPositioning();
}


QString BusSplitter::outNodeID( unsigned node ) const
{
	return QString("out_%1").arg(QString::number(node));
}


void BusSplitter::drawShape( QPainter &p )
{
	initPainter(p);
	
// 	QPen pen(p.pen());
// 	pen.setWidth();
// 	p.setPen(pen);
	
	int _x = int(x());
	int _y = int(y());
	
	QRect r = m_sizeRect;
	r.moveBy( _x, _y );
	p.drawRect(r);
	
	deinitPainter(p);
}



