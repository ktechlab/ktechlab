/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "varassignment.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <klocale.h>

Item* VarAssignment::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new VarAssignment( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* VarAssignment::libraryItem()
{
	return new LibraryItem(
		"flow/varassignment",
		i18n("Assignment"),
		i18n("Variables"),
		"assignment.png",
		LibraryItem::lit_flowpart,
		VarAssignment::construct );
}

VarAssignment::VarAssignment( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowPart( icnDocument, newItem, id ? id : "varassignment" )
{
	m_name = i18n("Variable Assignment");
	initProcessSymbol();
	createStdInput();
	createStdOutput();
	
	createProperty( "0-var1", Variant::Type::VarName );
	property("0-var1")->setCaption( i18n("Variable") );
	property("0-var1")->setValue("x");
	
	createProperty( "2-var2", Variant::Type::Combo );
	property("2-var2")->setToolbarCaption(" = ");
	property("2-var2")->setEditorCaption( i18n("Value") );
	property("2-var2")->setValue("0");
	
}

VarAssignment::~VarAssignment()
{
}

void VarAssignment::dataChanged()
{
	setCaption( dataString("0-var1") + " " + "=" /*dataString("1-op")*/ + " " + dataString("2-var2") );
}

void VarAssignment::generateMicrobe( FlowCode *code )
{
	code->addCode( dataString("0-var1")+" "+"="/*dataString("1-op")*/+" "+dataString("2-var2") );
	code->addCodeBranch( outputPart("stdoutput") );

#if 0
	QString var1 = dataString("0-var1");
	QString var2 = dataString("2-var2");
	QString op = dataString("1-op");
	
	if ( FlowCode::isLiteral(var1) ) return;
	code->addVariable(var1);
	
	QString newCode;
	
	if ( !FlowCode::isLiteral(var1) )
	{
		if ( FlowCode::isLiteral(var2) ) newCode += "movlw " + var2 + " ; Assign " + var2 + " to w register\n";
	}
	
	if ( !FlowCode::isLiteral(var2) )
	{
		code->addVariable(var2);
		newCode += "movf " + var2 + ",0 ; Move " + var2 + " to w register\n";
	}
	
	if		( op == "=" ) newCode += "movwf " + var1 + " ; Move contents of w register to " + var1 + "\n";
	else if ( op == "+=" ) newCode += "addwf " + var1 + ",1 ; Add contents of w register to " + var1 + " and place result back in " + var1 + "\n";
	else if ( op == "-=" ) newCode += "subwf " + var1 + ",1 ; Subtract contents of w register from " + var1 + " and place result back in " + var1 + "\n";
	else if ( op == "&=" ) newCode += "andwf " + var1 + ",1 ; Binary AND contents of w register with " + var1 + " and place result back in " + var1 + "\n";
	else if ( op == "or=" ) newCode += "iorwf " + var1 + ",1 ; Binary inclusive OR contents of w register with " + var1 + " and place result back in " + var1 + "\n";
	else if ( op == "xor=" ) newCode += "xorwf " + var1 + ",1 ; Binary exclusive OR contents of w register with " + var1 + " and place result back in " + var1 + "\n";
	
	newCode += gotoCode("stdoutput");
	
	code->addCodeBlock( id(), newCode );
#endif
}
