/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "forloop.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <klocalizedstring.h>

Item* ForLoop::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new ForLoop( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* ForLoop::libraryItem()
{
	return new LibraryItem(
		QStringList(QString("flow/forloop")),
		i18n("For"),
		i18n("Loops"),
		"for.png",
		LibraryItem::lit_flowpart,
		ForLoop::construct );
}

ForLoop::ForLoop( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowContainer( icnDocument, newItem, id ? id : "forloop" )
{
	m_name = i18n("For Loop");
	
	createTopContainerNode();
	createBotContainerNode();
	
	createProperty( "0-var", Variant::Type::Combo );
	property("0-var")->setToolbarCaption("for");
	property("0-var")->setEditorCaption( i18n("Variable") );
	property("0-var")->setValue("x");
	
	createProperty( "1-initial", Variant::Type::Combo );
	property("1-initial")->setToolbarCaption("=");
	property("1-initial")->setEditorCaption( i18n("Initial Value") );
	property("1-initial")->setValue("1");
	
	createProperty( "2-end", Variant::Type::Combo );
	property("2-end")->setToolbarCaption( i18nc( "for x = 1 to", "to" ) );
	property("2-end")->setEditorCaption( i18n("End Value") );
	property("2-end")->setValue("10");
	
	createProperty( "3-step", Variant::Type::Combo );
	property("3-step")->setToolbarCaption("step");
	property("3-step")->setEditorCaption( i18n("Step") );
	property("3-step")->setValue("1");
	property("3-step")->setAdvanced(true);
}

ForLoop::~ForLoop()
{
}

void ForLoop::dataChanged()
{
	if( dataString("3-step").toInt() == 1 )
	setCaption( "for " + dataString("0-var") + " = " + dataString("1-initial") + " to " + dataString("2-end") );
	else setCaption( "for " + dataString("0-var") + " = " + dataString("1-initial") + " to " + dataString("2-end") + " step " + dataString("3-step"));
}

void ForLoop::generateMicrobe( FlowCode *code )
{
	if( dataString("3-step").toInt() == 1 ) code->addCode( "for " + dataString("0-var") + " = " + dataString("1-initial") + " to " + dataString("2-end") + "\n{" );
	else code->addCode( "for " + dataString("0-var") + " = " + dataString("1-initial") + " to " + dataString("2-end") + " step " + dataString("3-step") +"\n{" );
	code->addCodeBranch( outputPart("int_in") );
	code->addCode("}");
	code->addCodeBranch( outputPart("ext_out") );
}

