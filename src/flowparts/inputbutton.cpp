/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "inputbutton.h"
#include "flowcode.h"
#include "libraryitem.h"
#include "icndocument.h"

#include <KLocalizedString>

Item *InputButton::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new InputButton(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *InputButton::libraryItem()
{
    return new LibraryItem(QStringList(QString("flow/inputbutton")), i18n("InputButton"), i18n("Functions"), "ppinputbutton.png", LibraryItem::lit_flowpart, InputButton::construct);
}

InputButton::InputButton(ICNDocument *icnDocument, bool newItem, const char *id)
    : FlowPart(icnDocument, newItem, id ? id : "inputbutton")
{
    m_name = i18n("InputButton");
    initProcessSymbol();
    createStdInput();
    createStdOutput();

    createProperty("0-trigger", Variant::Type::Select);
    property("0-trigger")->setCaption(i18n("Trigger"));
    property("0-trigger")->setAllowed((QStringList("rising") << "falling"));
    property("0-trigger")->setValue("rising");

    createProperty("1-pin", Variant::Type::Pin);
    property("1-pin")->setCaption(i18n("Pin"));
    property("1-pin")->setValue("RA0");
}

InputButton::~InputButton()
{
}

void InputButton::dataChanged()
{
    setCaption(i18n("Continue on %1 %2", dataString("0-trigger"), dataString("1-pin")));
}

void InputButton::generateMicrobe(FlowCode *code)
{
    code->addCodeBranch(outputPart("stdoutput"));
}
