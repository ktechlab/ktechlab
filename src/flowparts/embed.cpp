/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "embed.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <klocalizedstring.h>

Item* Embed::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new Embed( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* Embed::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("flow/embed")),
		i18n("Embed"),
		i18n("Common"),
		"embed.png",
		LibraryItem::lit_flowpart,
		Embed::construct
			);
}

Embed::Embed( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowPart( icnDocument, newItem, id ? id : "embed" )
{
	m_name = i18n("Embed");
	initProcessSymbol();
	createStdInput();
	createStdOutput();
	
	createProperty( "type", Variant::Type::Select );
	property("type")->setAllowed( QStringList::split( ',', "Microbe,Assembly" ) );
	property("type")->setValue("Microbe");
	property("type")->setCaption( i18n("Type") ); // TODO: replace this with i18n( "the type", "Type" );
	
	createProperty( "code", Variant::Type::Multiline );
	property("code")->setCaption( i18n("Code") );
	property("code")->setValue( i18n("// Embedded code:") );
}

Embed::~Embed()
{
}


void Embed::dataChanged()
{
	const QString sample = dataString("code").left(10).replace("\n"," ");
	setCaption( i18n("%1: %2...", dataString("type"), sample) );
}


bool Embed::typeIsMicrobe() const
{
	return dataString("type") == "Microbe";
}


void Embed::generateMicrobe( FlowCode *code )
{
	if ( typeIsMicrobe() )
		code->addCode( dataString("code") );
	
	else
	{
		// Is assembly code, we need to microbe as such
		code->addCode("asm\n{");
		code->addCode( dataString("code") );
		code->addCode("}");
	}
	
	code->addCodeBranch( outputPart("stdoutput") );
}

