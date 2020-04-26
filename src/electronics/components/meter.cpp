/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "canvasitemparts.h"
#include "ecnode.h"
#include "element.h"
#include "libraryitem.h"
#include "meter.h"
#include "variant.h"
#include "voltagesource.h"
#include "pin.h"
#include "simulator.h"

#include <cmath>
#include <KLocalizedString>
#include <qpainter.h>


//BEGIN class Meter
Meter::Meter( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id )
{
	b_timerStarted = false;
	m_timeSinceUpdate = 0.;
	m_old_value  = 0.;
	m_avgValue = 0.;
	b_firstRun = true;
	m_prevProp = 0.0;
	setSize( -16, -16, 32, 32 );

	p_displayText = addDisplayText( "meter", QRect( -16, 16, 32, 16 ), displayText() );
	
	createProperty( "0-minValue", Variant::Type::Double );
	property("0-minValue")->setCaption( i18n("Minimum Value") );
	property("0-minValue")->setMinValue(1e-12);
	property("0-minValue")->setMaxValue(1e12);
	property("0-minValue")->setValue(1e-3);
	
	createProperty( "1-maxValue", Variant::Type::Double );
	property("1-maxValue")->setCaption( i18n("Maximum Value") );
	property("1-maxValue")->setMinValue(1e-12);
	property("1-maxValue")->setMaxValue(1e12);
	property("1-maxValue")->setValue(1e3);
}


Meter::~Meter()
{
}


void Meter::dataChanged()
{
	m_minValue = dataDouble("0-minValue");
	m_maxValue = dataDouble("1-maxValue");
	setChanged();
}

void Meter::stepNonLogic()
{
	if (b_firstRun)
	{
		p_displayText->setText(displayText());
		updateAttachedPositioning();
		setChanged();
		property("0-minValue")->setUnit(m_unit);
		property("1-maxValue")->setUnit(m_unit);
		b_firstRun = false;
	}
		
	const double v = meterValue();
	if ( !b_timerStarted && std::abs(((v-m_old_value)/m_old_value)) > 1e-6 ) {
		b_timerStarted = true;
	}
	
	if (b_timerStarted) {
		m_timeSinceUpdate += LINEAR_UPDATE_PERIOD;
		m_avgValue += v * LINEAR_UPDATE_PERIOD;
// 		setChanged();
		if ( m_timeSinceUpdate > 0.05 )
		{
			if ( p_displayText->setText(displayText()) )
				updateAttachedPositioning();
		}
	}
}


bool Meter::contentChanged() const
{
	return (m_prevProp != calcProp( m_old_value ));
}


void Meter::drawShape( QPainter &p )
{
	initPainter(p);
	p.drawEllipse( (int)x()-16, (int)y()-16, width(), width() );
	p.setPen(QPen(Qt::black,2));
	p.setBrush(Qt::black);
	
	// The proportion between 0.1mV and 10KV, on a logarithmic scale
	m_prevProp = calcProp( m_old_value );
	double sin_prop = 10*std::sin(m_prevProp*1.571); // 1.571 = pi/2
	double cos_prop = 10*std::cos(m_prevProp*1.571); // 1.571 = pi/2
	
	int cx = int(x()-16+(width()/2));
	int cy = int(y()-16+(height()/2));
	p.drawLine( int(cx-sin_prop), int(cy-cos_prop), int(cx+sin_prop), int(cy+cos_prop) );
	
	QPolygon pa(3);
	pa[0] = QPoint( int(cx-sin_prop), int(cy-cos_prop) ); // Arrow head
	pa[1] = QPoint( int(cx-sin_prop + 8*std::sin(1.571*(-0.3+m_prevProp))), int(cy-cos_prop + 8*std::cos(1.571*(-0.3+m_prevProp))) );
	pa[2] = QPoint( int(cx-sin_prop + 8*std::sin(1.571*(0.3+m_prevProp))), int(cy-cos_prop + 8*std::cos(1.571*(0.3+m_prevProp))) );
	p.drawPolygon(pa);
	
	deinitPainter(p);
}


double Meter::calcProp( double v ) const
{
	double abs_value = std::abs( v );
	
	double prop;
	if ( abs_value <= m_minValue )
		prop = 0.0;
	else if ( abs_value >= m_maxValue )
		prop = 1.0;
	else
		prop = std::log10( abs_value/m_minValue ) / std::log10( m_maxValue/m_minValue );
	
	if ( m_old_value>0 )
		prop *= -1;
	
	return prop;
}


QString Meter::displayText()
{
	double value = m_avgValue/m_timeSinceUpdate;
	if ( !std::isfinite(value) ) value = m_old_value;
	if ( std::abs((value)) < 1e-9 ) value = 0.;
	m_old_value = value;
	m_avgValue = 0.;
	m_timeSinceUpdate = 0.;
	b_timerStarted = false;
	return QString::number( value/CNItem::getMultiplier(value), 'g', 3 ) + CNItem::getNumberMag(value) + m_unit;
}
//END class Meter


//BEGIN class FrequencyMeter
Item* FrequencyMeter::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new FrequencyMeter( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* FrequencyMeter::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/frequencymeter")),
		i18n("Frequency Meter (TODO)"),
		i18n("Outputs"),
		"frequencymeter.png",
		LibraryItem::lit_component,
		FrequencyMeter::construct );
}

FrequencyMeter::FrequencyMeter( ICNDocument *icnDocument, bool newItem, const char *id )
	: Meter( icnDocument, newItem, id ? id : "frequencymeter" )
{
	m_name = i18n("Frequency Meter");
	m_unit = "Hz";
	
	m_probeNode = createPin( 0, -24, 90, "n1" );
}

FrequencyMeter::~FrequencyMeter()
{
}

double FrequencyMeter::meterValue()
{
	return 0;
}
//END class FrequencyMeter


//BEGIN class ECAmmeter
Item* ECAmmeter::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECAmmeter( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECAmmeter::libraryItem()
{
	QStringList ids;
	ids << "ec/ammeter" << "ec/ammmeter";
	return new LibraryItem(
		ids,
		i18n("Ammeter"),
		i18n("Outputs"),
		"ammeter.png",
		LibraryItem::lit_component,
		ECAmmeter::construct
			);
}

ECAmmeter::ECAmmeter( ICNDocument *icnDocument, bool newItem, const char *id )
	: Meter( icnDocument, newItem, id ? id : "ammeter" )
{
	m_name = i18n("Ammeter");
	setSize( -16, -16, 32, 32 );
	m_unit = "A";
	
	init1PinLeft(0);
	init1PinRight(0);
	
	m_voltageSource = createVoltageSource( m_pNNode[0], m_pPNode[0], 0. );
}

ECAmmeter::~ECAmmeter()
{
}

double ECAmmeter::meterValue()
{
	return -m_voltageSource->cbranchCurrent(0);
}
//END class ECAmmeter


//BEGIN class ECVoltmeter
Item* ECVoltMeter::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ECVoltMeter( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ECVoltMeter::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("ec/voltmeter")),
		i18n("Voltmeter"),
		i18n("Outputs"),
		"voltmeter.png",
		LibraryItem::lit_component,
		ECVoltMeter::construct );
}

ECVoltMeter::ECVoltMeter( ICNDocument *icnDocument, bool newItem, const char *id )
	: Meter( icnDocument, newItem, id ? id : "voltmeter" )
{
	m_name = i18n("Voltmeter");
	m_unit = "V";
	
	init1PinLeft(0);
	init1PinRight(0);
}

ECVoltMeter::~ECVoltMeter()
{
}

double ECVoltMeter::meterValue()
{
	return m_pNNode[0]->pin()->voltage() - m_pPNode[0]->pin()->voltage();
}
//END class ECVoltMeter

