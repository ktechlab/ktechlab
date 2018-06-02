/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "canvasitemparts.h"
#include "ecnode.h"
#include "ecpotentiometer.h"
#include "libraryitem.h"
#include "resistance.h"

#include <klocalizedstring.h>
#include <qpainter.h>
#include <qstyle.h>

Item* ECPotentiometer::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECPotentiometer( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECPotentiometer::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/potentiometer")),
		i18n("Potentiometer"),
		i18n("Passive"),
		"potentiometer.png",
		LibraryItem::lit_component,
		ECPotentiometer::construct );
}

ECPotentiometer::ECPotentiometer( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id ? id : "potentiometer" )
{
	m_name = i18n("Potentiometer");
	setSize( -16, -16, 40, 32 );
	
	m_p1 = createPin( 32, 0, 180, "p1" );
	
	m_sliderProp = 0.0;
	m_resistance = 5000.;
	m_r1 = createResistance( createPin( -8, -24, 90, "n1" ), m_p1, 1. );
	m_r2 = createResistance( createPin( -8, 24, 270, "n2" ), m_p1, 1. );
	
	Slider * s = addSlider( "slider", 0, 100, 5, 50, Qt::Vertical, QRect( 0, -16, 16, 32 ) );
	m_pSlider = static_cast<QSlider*>(s->widget());
	
	createProperty( "resistance", Variant::Type::Double );
	property("resistance")->setCaption( i18n("Resistance") );
	property("resistance")->setUnit( QChar(0x3a9) );
	property("resistance")->setMinValue(1e-6);
	property("resistance")->setValue(1e5);
	
	addDisplayText( "res", QRect( -56, -8, 40, 16 ), "" );
}

ECPotentiometer::~ECPotentiometer()
{
}

void ECPotentiometer::dataChanged()
{
	m_resistance = dataDouble("resistance");
	
	QString display = QString::number( m_resistance / getMultiplier(m_resistance), 'g', 3 ) + getNumberMag(m_resistance) + QChar(0x3a9);
	setDisplayText( "res", display );
	
	sliderValueChanged( "slider", slider("slider")->value() );
}

void ECPotentiometer::sliderValueChanged( const QString &id, int newValue )
{
	if ( id != "slider" )
		return;
	
	m_sliderProp = (newValue-50.0)/100.0;
	
	m_r1->setResistance( m_resistance*(double)newValue/100. );
	m_r2->setResistance( m_resistance*(double)(100.-newValue)/100. );
}

void ECPotentiometer::drawShape( QPainter &p )
{
	initPainter(p);
	int _x = int(x());
	int _y = int(y());
	
	p.drawRect( _x-14, _y-16, 12, 32 );
	
	QPolygon pa(3);
	pa[0] = QPoint( 0, 0 );
	pa[1] = QPoint( 4, -3 );
	pa[2] = QPoint( 4, 3 );
	
	int space = m_pSlider->style()->pixelMetric( QStyle::PM_SliderSpaceAvailable /*, m_pSlider TODO investigate parameter */ );
	int base_y = _y + int( space * m_sliderProp );
	
	pa.translate( _x+16, base_y );
	
	QColor c = m_p1->isSelected() ? m_selectedCol : Qt::black;
	
	p.setPen(c);
	p.setBrush(c);
	p.drawPolygon(pa);
	
	p.drawLine( _x+20, base_y, _x+24, base_y );
	p.drawLine( _x+24, base_y, _x+24, _y );
	
	deinitPainter(p);
}



