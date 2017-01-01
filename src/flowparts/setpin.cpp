/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "setpin.h"

#include "libraryitem.h"
#include "flowcode.h"
#include "picinfo.h"
#include "flowcodedocument.h"
#include "microsettings.h"

#include <klocalizedstring.h>

Item* SetPin::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new SetPin( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* SetPin::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("flow/setpin")),
		i18n("Set Pin State"),
		i18n("I\\/O"),
		"pinwrite.png",
		LibraryItem::lit_flowpart,
		SetPin::construct );
}

SetPin::SetPin( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowPart( icnDocument, newItem, id ? id : "setpin" )
{
	m_name = i18n("Set Pin State");
	initIOSymbol();
	createStdInput();
	createStdOutput();
	
	createProperty( "state", Variant::Type::Select );
	property("state")->setCaption( i18n("State") );
	property("state")->setAllowed( QStringList::split( ',', "high,low" ) );
	property("state")->setValue("high");
	
	createProperty( "pin", Variant::Type::Pin );
	property("pin")->setCaption( i18n("Pin") );
	property("pin")->setValue("RA0");
}

SetPin::~SetPin()
{
}

void SetPin::dataChanged()
{
	setCaption( i18n("Set %1 %2", dataString("pin"), dataString("state")) );
}

void SetPin::generateMicrobe( FlowCode *code )
{
	const QString pin = dataString("pin");
	const QString port = "PORT" + QString((QChar)pin[1]);
	const QString bit = (QChar)pin[2];
	code->addCode( port+"."+bit+" = "+dataString("state") );
	code->addCodeBranch( outputPart("stdoutput") );
	
#if 0
	const QString pin = dataString("pin");
	const bool isHigh = (dataString("state") == "High");
	const QString port = "PORT" + QString((QChar)pin[1]);
	const QString bit = (QChar)pin[2];
	
	QString newCode;
	if (isHigh)
	{
		newCode += "bsf " + port + "," + bit + " ; Set bit high\n";
	}
	else
	{
		newCode += "bcf " + port + "," + bit + " ; Set bit low\n";
	}
	
	code->addCodeBlock( id(), newCode );
#endif
}
