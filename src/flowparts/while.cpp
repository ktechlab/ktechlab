/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "while.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <klocale.h>

Item* While::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new While( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* While::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("flow/while")),
		i18n("While"),
		i18n("Loops"),
		"while.png",
		LibraryItem::lit_flowpart,
		While::construct );
}

While::While( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowContainer( icnDocument, newItem, id ? id : "whileloop" )
{
	m_name = i18n("While");
	createTopContainerNode();
	createBotContainerNode();
	
	createProperty( "0var1", Variant::Type::Combo );
	property("0var1")->setToolbarCaption( "while" );
	property("0var1")->setEditorCaption( i18n("Variable") );
	property("0var1")->setValue("x");
	
	createProperty( "1op", Variant::Type::Select );
	property("1op")->setToolbarCaption(" ");
	property("1op")->setEditorCaption( i18n("Operation") );
	property("1op")->setAllowed( QStringList::split( ',', "==,<,>,<=,>=,!=" ) );
	property("1op")->setValue("==");
	
	createProperty( "2var2", Variant::Type::Combo );
	property("2var2")->setToolbarCaption(" ");
	property("2var2")->setEditorCaption( i18n("Value") );
	property("2var2")->setValue("0");
}

While::~While()
{
}

void While::dataChanged()
{
	setCaption( i18n("while %1 %2 %3", dataString("0var1"), dataString("1op"), dataString("2var2")) );
}

void While::generateMicrobe( FlowCode *code )
{
	code->addCode("while "+dataString("0var1")+" "+dataString("1op")+" " + dataString("2var2")+"\n{" );
	code->addCodeBranch( outputPart("int_in") );
	code->addCode("}");
	code->addCodeBranch( outputPart("ext_out") );
}





