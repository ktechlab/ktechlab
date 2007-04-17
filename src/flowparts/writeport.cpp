/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "writeport.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <klocale.h>

Item* WritePort::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new WritePort( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* WritePort::libraryItem()
{
	return new LibraryItem(
		"flow/writeport",
		i18n("Write to Port"),
		i18n("I\\/O"),
		"portwrite.png",
		LibraryItem::lit_flowpart,
		WritePort::construct );
}

WritePort::WritePort( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowPart( icnDocument, newItem, id ? id : "writeport" )
{
	m_name = i18n("Write to Port");
	initIOSymbol();
	createStdInput();
	createStdOutput();
	
	createProperty( "0-var", Variant::Type::Combo );
	property("0-var")->setToolbarCaption( i18n("Write") );
	property("0-var")->setEditorCaption( i18n("Variable") );
	property("0-var")->setValue("x");
	
	createProperty( "1-port", Variant::Type::Port );
	property("1-port")->setToolbarCaption( i18n( "write to port", "to" ) );
	property("1-port")->setEditorCaption( i18n("Port") );
	property("1-port")->setValue("PORTA");
}


WritePort::~WritePort()
{
}


void WritePort::dataChanged()
{
	setCaption(  i18n("Write %1 to %2").arg(dataString("0-var")).arg(dataString("1-port")) );
}


void WritePort::generateMicrobe( FlowCode *code )
{
	code->addCode( dataString("1-port")+" = "+dataString("0-var") );
	code->addCodeBranch( outputPart("stdoutput") );
	
#if 0
	QString var = dataString("var");
	QString port = dataString("port");
	
	// WTF? I don't want to do this!
// 	QString newCode = "bsf STATUS,5 ; Move to bank 1\n";
	QString newCode;
	
	if ( FlowCode::isLiteral(var) ) newCode += "movlw " + var + " ; Move " + var + " to working register w\n";
	else
	{
		code->addVariable(var);
		newCode += "movf " + var + ",0 ; Move " + var + " to working register w\n";
	}
	
	newCode += "movwf " + port + " ; Move register w to port\n";
	
	// Same for below as for above
// 	newCode += "bcf STATUS,5 ; Come back to bank 0\n";
	
	newCode += gotoCode("stdoutput") + "\n";
	
	code->addCodeBlock( id(), newCode );
#endif
}
