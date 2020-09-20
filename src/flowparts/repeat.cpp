/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "repeat.h"

#include "flowcode.h"
#include "libraryitem.h"

#include <KLocalizedString>

Item *Repeat::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new Repeat((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *Repeat::libraryItem()
{
    return new LibraryItem(QStringList(QString("flow/repeat")), i18n("Repeat"), i18n("Loops"), "repeat.png", LibraryItem::lit_flowpart, Repeat::construct);
}

Repeat::Repeat(ICNDocument *icnDocument, bool newItem, const char *id)
    : FlowContainer(icnDocument, newItem, id ? id : "repeatloop")
{
    m_name = i18n("Repeat");
    createTopContainerNode();
    createBotContainerNode();

    createProperty("0var1", Variant::Type::Combo);
    property("0var1")->setToolbarCaption("repeat until");
    property("0var1")->setEditorCaption(i18n("Variable"));
    property("0var1")->setValue("x");

    createProperty("1op", Variant::Type::Select);
    property("1op")->setToolbarCaption(" ");
    property("1op")->setEditorCaption(i18n("Operation"));
    property("1op")->setAllowed((QStringList("==") << "<"
                                                   << ">"
                                                   << "<="
                                                   << ">="
                                                   << "!="));
    property("1op")->setValue("==");

    createProperty("2var2", Variant::Type::Combo);
    property("2var2")->setToolbarCaption(" ");
    property("2var2")->setEditorCaption(i18n("Value"));
    property("2var2")->setValue("0");
}

Repeat::~Repeat()
{
}

void Repeat::dataChanged()
{
    setCaption(i18n("repeat until %1 %2 %3", dataString("0var1"), dataString("1op"), dataString("2var2")));
}

void Repeat::generateMicrobe(FlowCode *code)
{
    code->addCode("repeat\n{\n");
    code->addCodeBranch(outputPart("int_in"));
    code->addCode("}\n");
    code->addCode("until " + dataString("0var1") + " " + dataString("1op") + " " + dataString("2var2"));
    code->addCodeBranch(outputPart("ext_out"));
}
