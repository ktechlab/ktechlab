/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "testpin.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <KLocalizedString>

Item* TestPin::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new TestPin( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* TestPin::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("flow/testpin")),
		i18n("Test Pin State"),
		i18n("I\\/O"),
		"pinread.png",
		LibraryItem::lit_flowpart,
		TestPin::construct );
}

TestPin::TestPin( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowPart( icnDocument, newItem, id ? id : "testpin" )
{
	m_name = i18n("Test Pin State");
	initDecisionSymbol();
	createStdInput();
	createStdOutput();
	createAltOutput();
	
	createProperty( "pin", Variant::Type::Pin );
	property("pin")->setCaption( i18n("Pin") );
	property("pin")->setValue("RA0");
	
	addDisplayText( "output_false", QRect( offsetX()+width(), 2, 40, 20 ), "Low" );
	addDisplayText( "output_true", QRect( 0, offsetY()+height(), 50, 20 ), "High" ); 
}


TestPin::~TestPin()
{
}


void TestPin::dataChanged()
{
	setCaption( "Test " + dataString("pin") );
}


void TestPin::generateMicrobe( FlowCode *code )
{
	const QString pin = dataString("pin");
	const QString port = "PORT" + QString((QChar)pin[1]);
	const QString bit = (QChar)pin[2];
	
	handleIfElse( code, port+"."+bit+" is high", port+"."+bit+" is low", "stdoutput", "altoutput" );
	
#if 0
	QString newCode;
	
	newCode += "btfss "+port+","+bit+" ; Check if pin is clear\n";
	newCode += gotoCode("altoutput") + " ; Pin is low\n";
	newCode += gotoCode("stdoutput") + " ; Pin is high, continue on from this point\n";
	
	code->addCodeBlock( id(), newCode );
#endif
}


