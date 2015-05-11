/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "bidirled.h"
#include "colorcombo.h"
#include "diode.h"
#include "led.h"
#include "ecnode.h"
#include "libraryitem.h"
#include "simulator.h"

#include <klocale.h>
#include <Qt/qpainter.h>

Item* BiDirLED::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new BiDirLED( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* BiDirLED::libraryItem()
{
	return new LibraryItem (
		QStringList(QString("ec/bidir_led")),
		i18n("Bidirectional LED"),
		i18n("Outputs"),
		"bidirled.png",
		LibraryItem::lit_component,
		BiDirLED::construct
						   );
}

BiDirLED::BiDirLED( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "bidir_led" )
{
	m_name = i18n("Bidirectional LED");
	m_bDynamicContent = true;
	
	setSize( -8, -16, 16, 32 );
	init1PinLeft();
	init1PinRight();
	setSize( -8, -24, 24, 40 );
	
	m_pDiode[0] = createDiode( m_pNNode[0], m_pPNode[0] );
	m_pDiode[1] = createDiode( m_pPNode[0], m_pNNode[0] );
	
	avg_brightness[0] = avg_brightness[1] = 255;
	lastUpdatePeriod = 0.;
	r[0]=r[1]=g[0]=g[1]=b[0]=b[1]=0;
	last_brightness[0] = last_brightness[1] = 255;
	
	createProperty( "0-color1", Variant::Type::Color );
	property("0-color1")->setCaption( i18n("Color 1") );
	property("0-color1")->setColorScheme( ColorCombo::LED );
	
	createProperty( "0-color2", Variant::Type::Color );
	property("0-color2")->setCaption( i18n("Colour 2") );
	property("0-color2")->setColorScheme( ColorCombo::LED );
}

BiDirLED::~BiDirLED()
{
}

void BiDirLED::dataChanged()
{
	QString colors[] = { "0-color1", "0-color2" };
	for ( unsigned i = 0; i < 2; i++ )
	{
		QColor color = dataColor(colors[i]);
		r[i] = color.red() / 0x100;
		g[i] = color.green() / 0x100;
		b[i] = color.blue() / 0x100;
	}
}

void BiDirLED::stepNonLogic()
{
	lastUpdatePeriod += LINEAR_UPDATE_PERIOD;
	
	for ( unsigned i = 0; i < 2; i++ )
		avg_brightness[i] += LED::brightness(m_pDiode[i]->current()) * LINEAR_UPDATE_PERIOD;
}

void BiDirLED::drawShape( QPainter &p )
{
	initPainter(p);
	
	for ( unsigned i = 0; i < 2; i++ )
	{
		uint _b;
		if ( lastUpdatePeriod == 0. )
			_b = last_brightness[i];
		
		else
		{
			_b = uint(avg_brightness[i]/lastUpdatePeriod);
			last_brightness[i] = _b;
		}
		avg_brightness[i] = 0.;
	
		p.setBrush( QColor( uint(255-(255-_b)*(1-r[i])), uint(255-(255-_b)*(1-g[i])), uint(255-(255-_b)*(1-b[i])) ) );
		
		
		Q3PointArray pa(3);
		if ( i == 0 )
		{
			pa[0] = QPoint( 8, -8 );
			pa[1] = QPoint( -8, -16 );
			pa[2] = QPoint( -8, 0 );
		}
		else
		{
			pa[0] = QPoint( -8, 8 );
			pa[1] = QPoint( 8, 0 );
			pa[2] = QPoint( 8, 16 );
		}
		
		pa.translate( int(x()), int(y()) );
		p.drawPolygon(pa);
		p.drawPolyline(pa);
	}
	lastUpdatePeriod = 0.;
		
	// Draw the arrows indicating it's a LED
	int _x = (int)x()-2;
	int _y = (int)y()-21;
	
	p.drawLine( _x+9, _y+3, _x+12, _y ); // Tail of left arrow
	p.drawLine( _x+12, _y, _x+10, _y ); // Left edge of left arrow tip
	p.drawLine( _x+12, _y, _x+12, _y+2 ); // Right edge of left arrow tip
	
	p.drawLine( _x+12, _y+6, _x+15, _y+3 ); // Tail of right arrow
	p.drawLine( _x+15, _y+3, _x+13, _y+3 ); // Left edge of right arrow tip
	p.drawLine( _x+15, _y+3, _x+15, _y+5 ); // Right edge of right arrow tip
	
	p.drawLine( _x+10, _y, _x+15, _y+5 ); // Diagonal line that forms base of both arrow tips
	
	_x = int(x());
	_y = int(y());
	p.drawLine( _x+8, _y-16, _x+8, _y );
	p.drawLine( _x-8, _y, _x-8, _y+16 );
	
	deinitPainter(p);
}

