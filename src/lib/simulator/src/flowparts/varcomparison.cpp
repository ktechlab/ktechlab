/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "varcomparison.h"

#include "libraryitem.h"
#include "flowcode.h"

#include <klocale.h>

Item* VarComparison::construct( ItemDocument *itemDocument, bool newItem, const char *id )
{
	return new VarComparison( (ICNDocument*)itemDocument, newItem, id );
}

LibraryItem* VarComparison::libraryItem()
{
	return new LibraryItem(
		"flow/varcomparison",
		i18n("Comparison"),
		i18n("Variables"),
		"branch.png",
		LibraryItem::lit_flowpart,
		VarComparison::construct );
}

VarComparison::VarComparison( ICNDocument *icnDocument, bool newItem, const char *id )
	: FlowPart( icnDocument, newItem, id ? id : "varcomparison" )
{
	m_name = i18n("Variable Comparison");
	initDecisionSymbol();
	createStdInput();
	createStdOutput();
	createAltOutput();
	
	createProperty( "0var1", Variant::Type::Combo );
	property("0var1")->setCaption( i18n("Variable") );
	property("0var1")->setValue("x");
	
	createProperty( "1op", Variant::Type::Select );
	property("1op")->setAllowed( QStringList::split( ',', "==,<,>,<=,>=,!=" ) );
	property("1op")->setValue("==");
	property("1op")->setToolbarCaption(" ");
	property("1op")->setEditorCaption( i18n("Operation") );
	
	createProperty( "2var2", Variant::Type::Combo );
	property("2var2")->setToolbarCaption(" ");
	property("2var2")->setEditorCaption( i18n("Value") );
	property("2var2")->setValue("0");
	
	addDisplayText( "output_false", QRect( offsetX()+width(), 2, 40, 20 ), "No" );
	addDisplayText( "output_true", QRect( 0, offsetY()+height(), 50, 20 ), "Yes" ); 
}

VarComparison::~VarComparison()
{
}

void VarComparison::dataChanged()
{
	setCaption( dataString("0var1") + " " + dataString("1op") + " " + dataString("2var2") + " ?" );
}

QString VarComparison::oppOp( const QString &op )
{
	if		( op == "==" )	return "!=";
	if		( op == "!=" )	return "==";
	else if ( op == "<" )	return ">=";
	else if ( op == ">=" )	return "<";
	else if ( op == ">" )	return "<=";
	else if ( op == "<=" )	return ">";
	else return "__UNKNOWN_OP__";
}

void VarComparison::generateMicrobe( FlowCode *code )
{
	QString var1 = dataString("0var1");
	QString var2 = dataString("2var2");
	QString test = dataString("1op");
	
	handleIfElse( code, var1+" "+test+" "+var2, var1+" "+oppOp(test)+" "+var2, "stdoutput", "altoutput" );
	
#if 0
	code->addCode( "if "+var1+" "+test+" "+var2+"\n{\n" );
	code->addCodeBranch( outputPart("stdoutput") );
	code->addCode("}");
	if ( outputPart("altoutput") )
	{
		code->addCode("else\n{");
		code->addCodeBranch( outputPart("altoutput") );
		code->addCode("}");
	}
#endif
	
#if 0
	QString newCode;
	
	if ( FlowCode::isLiteral(var2) ) newCode += "movlw " + var2 + " ; Move literal to register w\n";
	else
	{
		code->addVariable(var2);
		newCode += "movf " + var2 + ",0 ; Move " + var2 + " to register w\n";
	}
	
	if ( FlowCode::isLiteral(var1) ) newCode += "sublw " + var1 + " ; Subtract register w from " + var1 + ", placing result in w\n";
	else
	{
		code->addVariable(var1);
		newCode += "subwf " + var1 + ",0 ; Subtract register w from " + var1 + ", placing result in w\n";
	}
	
	
	if		( test == "==" )
	{
		// check: works
		newCode += "btfss STATUS,2 ; Check if zero flag is set\n";
		newCode += gotoCode("altoutput") + " ; Result from calculation was non-zero; hence comparison is false\n";
		newCode += gotoCode("stdoutput") + " ; Ouput was zero; hence comparison is true, so continue from this point\n";
	}
	else if	( test == "!=" )
	{
		// check: works
		newCode += "btfsc STATUS,2 ; Check if zero flag is clear\n";
		newCode += gotoCode("altoutput") + " ; Result from calculation was zero; hence comparison is false\n";
		newCode += gotoCode("stdoutput") + " ; Output was non-zero; hence comparison is true, so continue from this point\n";
	}
	else if	( test == ">=" )
	{
		// check: works
		newCode += "btfss STATUS,0 ; Check if carry flag is set\n";
		newCode += gotoCode("altoutput") + " ; Result from calculation is negative; hence comparison is false\n";
		newCode += gotoCode("stdoutput") + " ; Result from calculation is positive or zero; so continue from this point\n";
	}
	else if	( test == ">" )
	{
		// check: works
		newCode += "btfss STATUS,0 ; Check if carry flag is set\n";
		newCode += gotoCode("altoutput") + " ; Result is negative; hence comparison is false\n";
		newCode += "btfsc STATUS,2 ; Check if zero flag is set\n";
		newCode += gotoCode("altoutput") + " ; Result is zero; hence comparison is false\n";
		newCode += gotoCode("stdoutput") + " ; Comparison is true, so continue from this point\n";
	}
	else if	( test == "<" )
	{
		// check: works
		newCode += "btfsc STATUS,0 ; Check if carry flag is set\n";
		newCode += gotoCode("altoutput");
		newCode += gotoCode("stdoutput");
	}
	else if	( test == "<=" )
	{
		// check: works
		newCode += "btfsc STATUS,2 ; Check if result is zero\n";
		newCode += gotoCode("stdoutput") + " ; Result is zero; hence comparison is true\n";
		newCode += "btfsc STATUS,0 ; Check if carry flag is set\n";
		newCode += gotoCode("altoutput") + " ; Result is positive (not zero, has already tested for this); hence comparison is false\n";
		newCode += gotoCode("stdoutput") + " ; Result is negative, hence comparison is true\n";
	}
	
	code->addCodeBlock( id(), newCode );
#endif
}
