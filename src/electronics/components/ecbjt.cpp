/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "bjt.h"
#include "ecbjt.h"
#include "ecnode.h"
#include "libraryitem.h"

#include <kiconloader.h>
#include <klocalizedstring.h>
#include <Qt/qpainter.h>

Item * ECBJT::constructNPN( ItemDocument * itemDocument, bool newItem, const char * id )
{
	return new ECBJT( true, (ICNDocument*)itemDocument, newItem, id );
}


Item * ECBJT::constructPNP( ItemDocument * itemDocument, bool newItem, const char * id )
{
	return new ECBJT( false, (ICNDocument*)itemDocument, newItem, id );
}


LibraryItem* ECBJT::libraryItemNPN()
{
	return new LibraryItem(
		QStringList(QString("ec/npnbjt")),
		i18n("NPN"),
		i18n("Discrete"),
		"npn.png",
		LibraryItem::lit_component,
		ECBJT::constructNPN );
}


LibraryItem* ECBJT::libraryItemPNP()
{
	return new LibraryItem(
		QStringList(QString("ec/pnpbjt")),
		i18n("PNP"),
		i18n("Discrete"),
		"pnp.png",
		LibraryItem::lit_component,
		ECBJT::constructPNP );
}


ECBJT::ECBJT( bool isNPN, ICNDocument * icnDocument, bool newItem, const char * id )
	: Component( icnDocument, newItem, id ? id : (isNPN ? "npnbjt" : "pnpbjt") )
{
	m_bIsNPN = isNPN;
	if ( m_bIsNPN )
		m_name = i18n("NPN Transistor");
	else
		m_name = i18n("PNP Transistor");
	
	setSize( -8, -8, 16, 16 );
	m_pBJT = createBJT( createPin( 8, -16, 90, "c" ), createPin( -16, 0, 0, "b" ), createPin( 8, 16, 270, "e" ), m_bIsNPN );
	
	BJTSettings s; // will be created with the default settings
	
	Variant * v = createProperty( "I_S", Variant::Type::Double );
	v->setCaption( i18n("Saturation Current") );
	v->setUnit("A");
	v->setMinValue(1e-20);
	v->setMaxValue(1e-0);
	v->setValue( s.I_S );
	v->setAdvanced(true);
	
	v = createProperty( "N_F", Variant::Type::Double );
	v->setCaption( i18n("Forward Coefficient") );
	v->setMinValue(1e0);
	v->setMaxValue(1e1);
	v->setValue( s.N_F );
	v->setAdvanced(true);
	
	v = createProperty( "N_R", Variant::Type::Double );
	v->setCaption( i18n("Reverse Coefficient") );
	v->setMinValue(1e0);
	v->setMaxValue(1e1);
	v->setValue( s.N_R );
	v->setAdvanced(true);
	
	v = createProperty( "B_F", Variant::Type::Double );
	v->setCaption( i18n("Forward Beta") );
	v->setMinValue(1e-1);
	v->setMaxValue(1e3);
	v->setValue( s.B_F );
	v->setAdvanced(true);
	
	v = createProperty( "B_R", Variant::Type::Double );
	v->setCaption( i18n("Reverse Beta") );
	v->setMinValue(1e-1);
	v->setMaxValue(1e3);
	v->setValue( s.B_R );
	v->setAdvanced(true);
}

ECBJT::~ECBJT()
{
}


void ECBJT::dataChanged()
{
	BJTSettings s;
	s.I_S = dataDouble( "I_S" );
	s.N_F = dataDouble( "N_F" );
	s.N_R = dataDouble( "N_R" );
	s.B_F = dataDouble( "B_F" );
	s.B_R = dataDouble( "B_R" );
	
	m_pBJT->setBJTSettings( s );
}


void ECBJT::drawShape( QPainter &p )
{
	const int _x = int(x());
	const int _y = int(y());
	
	initPainter(p);
	
	p.drawLine( _x-8, _y-8, _x-8, _y+8 );
	p.drawLine( _x+8, _y-8, _x-8, _y );
	p.drawLine( _x+8, _y+8, _x-8, _y );
	
	Q3PointArray pa(3);
	if ( m_bIsNPN )
	{
		pa[0] = QPoint( _x+6, _y+7 );
		pa[1] = QPoint( _x+2, _y+8 );
		pa[2] = QPoint( _x+5, _y+3 );
	}
	else
	{
		pa[0] = QPoint( _x-7, _y+1 );
		pa[1] = QPoint( _x-4, _y+5 );
		pa[2] = QPoint( _x-2, _y );
	}
	p.setBrush( p.pen().color() );
	p.drawPolygon(pa);
	
	deinitPainter(p);
}
