/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "keypad.h"

#include "flowcode.h"
#include "libraryitem.h"

#include <KLocalizedString>

Item *Keypad::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new Keypad((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *Keypad::libraryItem()
{
    return new LibraryItem(QStringList(QString("flow/keypad")), i18n("Keypad"), i18n("Functions"), "keypad.png", LibraryItem::lit_flowpart, Keypad::construct);
}

Keypad::Keypad(ICNDocument *icnDocument, bool newItem, const char *id)
    : FlowPart(icnDocument, newItem, id ? id : "keypad")
{
    m_name = i18n("Keypad");
    initProcessSymbol();
    createStdInput();
    createStdOutput();

    createProperty("variable", Variant::Type::VarName);
    property("variable")->setValue("x");
    property("variable")->setCaption(i18n("Variable"));

    Variant *v = createProperty("keypad", Variant::Type::KeyPad);
    v->setCaption(i18n("Pin map"));
}

Keypad::~Keypad()
{
}

void Keypad::dataChanged()
{
    setCaption(i18n("Read %1 to %2", dataString("keypad"), dataString("variable")));
}

void Keypad::generateMicrobe(FlowCode *code)
{
    code->addCode(QString("%1 = %2").arg(dataString("variable")).arg(dataString("keypad")));
    code->addCodeBranch(outputPart("stdoutput"));
}
