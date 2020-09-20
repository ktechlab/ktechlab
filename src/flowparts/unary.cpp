/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "unary.h"

#include "flowcode.h"
#include "libraryitem.h"

#include <KLocalizedString>

Item *Unary::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new Unary((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *Unary::libraryItem()
{
    return new LibraryItem(QStringList(QString("flow/unary")), i18n("Unary"), i18n("Variables"), "unary.png", LibraryItem::lit_flowpart, Unary::construct);
}

Unary::Unary(ICNDocument *icnDocument, bool newItem, const char *id)
    : FlowPart(icnDocument, newItem, id ? id : "unary")
{
    m_name = i18n("Unary");
    initProcessSymbol();
    createStdInput();
    createStdOutput();

    createProperty("0-var", Variant::Type::VarName);
    property("0-var")->setValue("x");
    property("0-var")->setCaption(i18n("Variable"));

    createProperty("1-op", Variant::Type::Select);
    property("1-op")->setCaption(i18n("Operation"));
    QStringMap allowed;
    allowed["Rotate Left"] = i18n("Rotate Left");
    allowed["Rotate Right"] = i18n("Rotate Right");
    allowed["Increment"] = i18n("Increment");
    allowed["Decrement"] = i18n("Decrement");
    property("1-op")->setAllowed(allowed);
    property("1-op")->setValue("Rotate Left");
}

Unary::~Unary()
{
}

void Unary::dataChanged()
{
    setCaption(dataString("0-var") + " " + dataString("1-op"));
}

void Unary::generateMicrobe(FlowCode *code)
{
    const QString var = dataString("0-var");
    const QString op = dataString("1-op");

    if (op == "Rotate Left")
        code->addCode("rotateleft " + var);
    else if (op == "Rotate Right")
        code->addCode("rotateright " + var);
    else if (op == "Increment")
        code->addCode("increment " + var);
    else if (op == "Decrement")
        code->addCode("decrement " + var);
    //	else; // Hmm...
    code->addCodeBranch(outputPart("stdoutput"));

#if 0
	QString rot = dataString("1-rot");
	
	if ( FlowCode::isLiteral(var) ) return;
	
	QString newCode;
	
	code->addVariable(var);
	if ( rot == "Left" ) newCode += "rlf " + var + ",1 ; Unary " + var + " left through Carry, place result back in " + var + "\n";
	else newCode += "rrf " + var + ",1 ; Unary " + var + " right through Carry, place result back in " + var + "\n";

	newCode += gotoCode("stdoutput");
	code->addCodeBlock( id(), newCode );
#endif
}
