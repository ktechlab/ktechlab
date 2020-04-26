/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "libraryitem.h"
#include "flowcode.h"
#include "pulse.h"

#include <KLocalizedString>

Item* Pulse::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new Pulse( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* Pulse::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("flow/pulse")),
		i18n("Pulse"),
		i18n("Functions"),
		"pppulse.png",
		LibraryItem::lit_flowpart,
		Pulse::construct
			);
}

Pulse::Pulse( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowPart( icnDocument, newItem, id ? id : "pulse" )
{
	m_name = i18n("Pulse");
	initProcessSymbol();
	createStdInput();
	createStdOutput();
	
	createProperty( "0-duration", Variant::Type::Double );
	property("0-duration")->setCaption( i18n("Duration") );
	property("0-duration")->setUnit("sec");
	property("0-duration")->setValue(2.0);
	
	createProperty( "1-high", Variant::Type::Double );
	property("1-high")->setCaption( i18n("High Time") );
	property("1-high")->setUnit("sec");
	property("1-high")->setValue(0.5);
	
	createProperty( "2-low", Variant::Type::Double );
	property("2-low")->setCaption( i18n("Low Time") );
	property("2-low")->setUnit("sec");
	property("2-low")->setValue(0.5);
	
	createProperty( "3-pin", Variant::Type::Pin );
	property("3-pin")->setCaption( i18n("Pin") );
	property("3-pin")->setValue("RA0");
}

Pulse::~Pulse()
{
}


void Pulse::dataChanged()
{
	double pulse = dataDouble("0-duration");
	setCaption( i18n("Pulse %1 for %2 sec", dataString("3-pin"), QString::number( pulse / getMultiplier(pulse), 'f', 1 ) + getNumberMag(pulse)) );
}

void Pulse::generateMicrobe( FlowCode *code )
{
	const double duration_ms = dataDouble("0-duration")*1e3;
	const double high_ms = dataDouble("1-high")*1e3;
	const double low_ms = dataDouble("2-low")*1e3;
	const QString pin = dataString("3-pin");
	
	// TODO Do we want to change the format for pulsing?
	code->addCode( "pulse "+pin+" "+QString::number(duration_ms)+" "+QString::number(high_ms)+" "+QString::number(low_ms) );
	code->addCodeBranch( outputPart("stdoutput") );
}

