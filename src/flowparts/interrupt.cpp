/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "interrupt.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <klocalizedstring.h>

Item* Interrupt::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new Interrupt( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* Interrupt::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("flow/interrupt")),
		i18n("Interrupt"),
		i18n("Common"),
		"interrupt.png",
		LibraryItem::lit_flowpart,
		Interrupt::construct );
}

Interrupt::Interrupt( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowContainer( icnDocument, newItem, id ? id : "interrupt" )
{
	m_name = i18n("Interrupt");
	
	QStringList interruptTypes;
	interruptTypes.append("changed");
	interruptTypes.append("external");
	interruptTypes.append("timer");
	interruptTypes.append("trigger");

	createProperty( "interrupt", Variant::Type::Select );
	property("interrupt")->setAllowed(interruptTypes);
	property("interrupt")->setCaption( i18n("Interrupt") );
	property("interrupt")->setValue("trigger");
}

Interrupt::~Interrupt()
{
}

void Interrupt::dataChanged()
{
	setCaption( i18n("Interrupt %1", dataString("interrupt")) );
}

void Interrupt::generateMicrobe( FlowCode *code )
{
	code->addCode( "\ninterrupt "+dataString("interrupt")+"\n{" );
	code->addCodeBranch( outputPart("int_in") );
	code->addCode("}");
}


