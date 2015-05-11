/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "sub.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <klocale.h>

Item* Sub::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new Sub( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* Sub::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("flow/sub")),
		i18n("Subroutine"),
		i18n("Common"),
		"sub.png",
		LibraryItem::lit_flowpart,
		Sub::construct );
}

Sub::Sub( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowContainer( icnDocument, newItem, id ? id : "sub" )
{
	m_name = i18n("Sub");
	
	createProperty( "sub", Variant::Type::Combo );
	property("sub")->setCaption( i18n("Subroutine") );
	property("sub")->setValue("MySub");
}

Sub::~Sub()
{
}

void Sub::dataChanged()
{
	setCaption( "Sub " + dataString("sub") );
}

void Sub::generateMicrobe( FlowCode *code )
{
	code->addCode( "\nsub "+dataString("sub")+"\n{" );
	code->addCodeBranch( outputPart("int_in") );
	code->addCode("}");
}


// #include "sub.moc"
