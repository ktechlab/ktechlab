/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecnode.h"
#include "libraryitem.h"
#include "logic.h"
#include "pin.h"
#include "probe.h" //HACK: This has to be included before the oscilloscope headers
#include "oscilloscope.h"
#include "oscilloscopedata.h"
#include "simulator.h"
#include "voltagesource.h"

#include <klocalizedstring.h>
#include <qpainter.h>

//BEGIN class Probe
Probe::Probe( ICNDocument *icnDocument, bool newItem, const char *id )
	: Component( icnDocument, newItem, id )
{
	p_probeData = 0l;
	setSize( -16, -8, 32, 16 );
	
	createProperty( "color", Variant::Type::Color );
	property("color")->setCaption( i18n("Color") );
	property("color")->setValue( Qt::black );
}


Probe::~ Probe()
{
	delete p_probeData;
}


void Probe::dataChanged()
{
	m_color = dataColor("color");
	if (p_probeData)
		p_probeData->setColor(m_color);
	setChanged();
}
//END class Probe



//BEGIN class FloatingProbe
FloatingProbe::FloatingProbe( ICNDocument *icnDocument, bool newItem, const char *id )
	: Probe( icnDocument, newItem, id )
{
	p_probeData = m_pFloatingProbeData = static_cast<FloatingProbeData*>(registerProbe(this));
	property("color")->setValue( p_probeData->color() );
	
	createProperty( "scaling", Variant::Type::Select );
	property("scaling")->setCaption( i18n("Scaling") );
	QStringMap allowed;
	allowed["Linear"] = i18n("Linear");
	allowed["Logarithmic"] = i18n("Logarithmic");
	property("scaling")->setAllowed( allowed );
	property("scaling")->setValue("Linear");
	property("scaling")->setAdvanced( true );
	
	createProperty( "upper_abs_value", Variant::Type::Double );
	property("upper_abs_value")->setCaption( i18n("Upper Absolute Value") );
	property("upper_abs_value")->setValue(10.0);
	property("upper_abs_value")->setMinValue(0.0);
	property("upper_abs_value")->setUnit("V");
	property("upper_abs_value")->setAdvanced(true);
	
	createProperty( "lower_abs_value", Variant::Type::Double );
	property("lower_abs_value")->setCaption( i18n("Lower Absolute Value") );
	property("lower_abs_value")->setValue(0.1);
	property("lower_abs_value")->setMinValue(0.0);
	property("lower_abs_value")->setUnit("V");
	property("lower_abs_value")->setAdvanced(true);
}


FloatingProbe::~FloatingProbe()
{
}


void FloatingProbe::dataChanged()
{
	Probe::dataChanged();
	
	if ( dataString("scaling") == "Linear" )
		m_pFloatingProbeData->setScaling( FloatingProbeData::Linear );
	else
		m_pFloatingProbeData->setScaling( FloatingProbeData::Logarithmic );
	
	m_pFloatingProbeData->setUpperAbsValue( dataDouble("upper_abs_value") );
	m_pFloatingProbeData->setLowerAbsValue( dataDouble("lower_abs_value") );
}


void FloatingProbe::drawShape( QPainter &p )
{
	initPainter(p);
	
	int _x = int(x())-16;
	int _y = int(y())-8;
	
	p.drawRect( _x, _y, 32, 16 );
	
	QPolygon bezier(4);
	
	bezier[0] = QPoint( _x+4, _y+10 );
	bezier[1] = QPoint( _x+12, _y-6 );
	bezier[2] = QPoint( _x+20, _y+24 );
	bezier[3] = QPoint( _x+28, _y+4 );
	
	p.setPen( QPen( m_color, 2 ) );
	p.drawCubicBezier(bezier);
	
	deinitPainter(p);
}
//END class FloatingProbe



//BEGIN class VoltageProbe
Item* VoltageProbe::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new VoltageProbe( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* VoltageProbe::libraryItem()
{
	return new LibraryItem(
			QStringList(QString("ec/voltageprobe")),
	i18n("Voltage Probe"),
	i18n("Outputs"),
	"floatingprobe.png",
	LibraryItem::lit_component,
	VoltageProbe::construct );
}

VoltageProbe::VoltageProbe( ICNDocument *icnDocument, bool newItem, const char *id )
	: FloatingProbe( icnDocument, newItem, id ? id : "voltageprobe" )
{
	m_name = i18n("Voltage Probe");
	
	init1PinLeft();
	init1PinRight();
	m_pPin1 = m_pNNode[0]->pin();
	m_pPin2 = m_pPNode[0]->pin();
}


VoltageProbe::~VoltageProbe()
{
}


void VoltageProbe::stepNonLogic()
{
	m_pFloatingProbeData->addDataPoint( m_pPin1->voltage() - m_pPin2->voltage() );
}
//END class VoltageProbe



//BEGIN class CurrentProbe
Item* CurrentProbe::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new CurrentProbe( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* CurrentProbe::libraryItem()
{
	return new LibraryItem(
			QStringList(QString("ec/currentprobe")),
	i18n("Current Probe"),
	i18n("Outputs"),
	"floatingprobe.png",
	LibraryItem::lit_component,
	CurrentProbe::construct );
}

CurrentProbe::CurrentProbe( ICNDocument *icnDocument, bool newItem, const char *id )
	: FloatingProbe( icnDocument, newItem, id ? id : "currentprobe" )
{
	m_name = i18n("Current Probe");
	
	init1PinLeft(0);
	init1PinRight(0);
	
	m_voltageSource = createVoltageSource( m_pNNode[0], m_pPNode[0], 0. );
}


CurrentProbe::~CurrentProbe()
{
}


void CurrentProbe::stepNonLogic()
{
	m_pFloatingProbeData->addDataPoint( -m_voltageSource->cbranchCurrent(0) );
}
//END class CurrentProbe




//BEGIN class LogicProbe
Item* LogicProbe::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new LogicProbe( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* LogicProbe::libraryItem()
{
	QStringList ids;
	ids << "ec/probe" << "ec/logicprobe";
	return new LibraryItem(
		ids,
		i18n("Logic Probe"),
		i18n("Outputs"),
		"logicprobe.png",
		LibraryItem::lit_component,
		LogicProbe::construct );
}

LogicProbe::LogicProbe( ICNDocument *icnDocument, bool newItem, const char *id )
	: Probe( icnDocument, newItem, id ? id : "probe" )
{
	m_name = i18n("Logic Probe");
	
	init1PinRight();
	m_pIn = createLogicIn( m_pPNode[0] );
	
	p_probeData = p_logicProbeData = static_cast<LogicProbeData*>(registerProbe(this));
	property("color")->setValue( p_probeData->color() );
	
	m_pSimulator = Simulator::self();
	m_pIn->setCallback( this, (CallbackPtr)(&LogicProbe::logicCallback) );
	logicCallback(false);
}


LogicProbe::~LogicProbe()
{
}


void LogicProbe::logicCallback( bool value )
{
	p_logicProbeData->addDataPoint( LogicDataPoint( value, m_pSimulator->time() ) );
}


void LogicProbe::drawShape( QPainter &p )
{
	initPainter(p);
	
	int _x = int(x())-16;
	int _y = int(y())-8;
	
	p.drawRect( _x, _y, 32, 16 );
	
	p.setPen( QPen( m_color, 2 ) );
	
	p.drawLine( _x+4, _y+11, _x+6, _y+11 );
	p.drawLine( _x+6, _y+11, _x+6, _y+4 );
	p.drawLine( _x+6, _y+4, _x+10, _y+4 );
	p.drawLine( _x+10, _y+4, _x+10, _y+11 );
	p.drawLine( _x+10, _y+11, _x+16, _y+11 );
	p.drawLine( _x+16, _y+11, _x+16, _y+4 );
	p.drawLine( _x+16, _y+4, _x+23, _y+4 );
	p.drawLine( _x+23, _y+4, _x+23, _y+11 );
	p.drawLine( _x+23, _y+11, _x+28, _y+11 );
// 	p.drawLine( _x+23, _y+11, _x+26, _y+11 );
// 	p.drawLine( _x+26, _y+11, _x+26, _y+4 );
// 	p.drawLine( _x+26, _y+4, _x+28, _y+4 );
	
	deinitPainter(p);
}
//END class LogicProbe


