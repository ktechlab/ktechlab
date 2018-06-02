/***************************************************************************
 *   Copyright (C) 2006 by William Hillerby - william.hillerby@ntlworld.com*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "variableresistor.h"
#include "canvasitemparts.h"
#include "resistance.h"
#include "ecnode.h"
#include "libraryitem.h"

#include <klocalizedstring.h>
#include <qpainter.h>
#include <qstyle.h>
#include <kdebug.h>

Item* VariableResistor::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new VariableResistor( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* VariableResistor::libraryItem()
{
	return new LibraryItem(
			QStringList(QString("ec/variableresistor")),
	i18n("Variable Resistor"),
	i18n("Passive"),
	"variable_resistor.png",
	LibraryItem::lit_component,
	VariableResistor::construct 
						  );
}

VariableResistor::VariableResistor( ICNDocument* icnDocument, bool newItem, const QString& id )
	: Component( icnDocument, newItem, (!id.isEmpty()) ? id : "variable resistor" )
{
	m_name = i18n("Resistor");
		
	// Top Left(x,y) from centre point, width, height.
	setSize( -16, -16, 32, 32 );
	
	init1PinLeft();
	init1PinRight();
	
	// (see comment in variablecapacitor.cpp) - david
	m_tickValue = 1;
	
	m_minResistance = 0.5;
	m_maxResistance = 1.0;
	
	m_currResistance = m_minResistance + ( ( m_maxResistance - m_minResistance ) / 2 ) ;
	
	m_pResistance = createResistance( m_pPNode[0], m_pNNode[0], m_currResistance );
	
	createProperty( "resistance", Variant::Type::Double );
	property("resistance")->setCaption( i18n("Resistance") );
	property("resistance")->setUnit( QChar( 0x3a9 ) );
	property("resistance")->setMinValue( 1e-6 );
	property("resistance")->setValue( m_currResistance );
	
	createProperty( "minimum resistance", Variant::Type::Double );
	property("minimum resistance")->setCaption( i18n("Min") );
	property("minimum resistance")->setUnit( QChar( 0x3a9 ) );
	property("minimum resistance")->setMinValue( 1e-6 );
	property("minimum resistance")->setValue( m_minResistance );

	createProperty( "maximum resistance", Variant::Type::Double );
	property("maximum resistance")->setCaption( i18n("Max") );
	property("maximum resistance")->setUnit( QChar( 0x3a9 ) );
	property("maximum resistance")->setMinValue( 1e-6 );
	property("maximum resistance")->setValue( m_maxResistance );

	addDisplayText( "res", QRect( -16, -26, 32, 12 ), "", false );
	
	Slider * s = addSlider( "slider", 0, 100, 1, 50, Qt::Horizontal, QRect( -16, 14, width(), 16 ) );
	m_pSlider = static_cast<QSlider*>( s->widget() );

}

VariableResistor::~VariableResistor()
{
}

void VariableResistor::dataChanged()
{

	double new_minResistance = dataDouble( "minimum resistance" );
	double new_maxResistance = dataDouble( "maximum resistance" );
	
	if( new_minResistance != m_minResistance )
	{
		if( new_minResistance >= m_maxResistance )
		{
			m_minResistance = m_maxResistance;
			property( "minimum resistance" )->setValue( m_minResistance );
		} else m_minResistance = new_minResistance;
	}

	if( new_maxResistance != m_maxResistance )
	{
		if( new_maxResistance <= m_minResistance )
		{
			m_maxResistance = m_minResistance;
			property( "maximum resistance" )->setValue( m_maxResistance );
		} else m_maxResistance = new_maxResistance;
	}

	m_tickValue = ( m_maxResistance - m_minResistance ) / m_pSlider->maxValue();
	
	// Calculate the resistance jump per tick of a 100 tick slider.
	sliderValueChanged( "slider", slider("slider")->value() );
}

void VariableResistor::sliderValueChanged( const QString &id, int newValue )
{
	if ( id != "slider" ) return;

	/** @todo fix slider so current cap can be set in toolbar and editor and slider updates */ 
	m_currResistance = m_minResistance + ( newValue * m_tickValue );
	
	// Set the new capacitance value.
	m_pResistance->setResistance( m_currResistance );
	
	// Update property.
	property( "resistance" )->setValue( m_currResistance );
	
	QString display = QString::number( m_currResistance / getMultiplier( m_currResistance ), 'g', 3 ) 
			+ getNumberMag( m_currResistance ) + QChar( 0x3a9 );
	
	setDisplayText( "res", display );
}
		
void VariableResistor::drawShape( QPainter &p )
{
	initPainter(p);
	
	// Get centre point of component.
	int _y = (int)y();
	int _x = (int)x();
	
	p.drawRect( _x-16, _y-6, width(), 12 );
	p.drawLine( _x-12, _y+12, _x+13, _y-13 );
	
	QPolygon pa(3);
	
	// Diagonally pointing arrow
	pa[0] = QPoint( 0, 0 );
	pa[1] = QPoint( -4, 0 );
	pa[2] = QPoint( 0, 4 );
	
	pa.translate( _x+13, _y-13 );
	p.setBrush( p.pen().color() );
	p.drawPolygon( pa );
	
	deinitPainter(p);
}

