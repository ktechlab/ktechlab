/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "callsub.h"

#include "flowcode.h"
#include "libraryitem.h"
#include "icndocument.h"

#include <KLocalizedString>

Item *CallSub::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new CallSub(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *CallSub::libraryItem()
{
    return new LibraryItem(QStringList(QString("flow/callsub")), i18n("Sub Call"), i18n("Common"), "subcall.png", LibraryItem::lit_flowpart, CallSub::construct);
}

CallSub::CallSub(ICNDocument *icnDocument, bool newItem, const char *id)
    : FlowPart(icnDocument, newItem, id ? id : "callsub")
{
    m_name = i18n("Sub Call");
    initCallSymbol();
    createStdInput();
    createStdOutput();

    createProperty("sub", Variant::Type::Combo);
    property("sub")->setCaption(i18n("Subroutine"));
    property("sub")->setValue("MySub");
}

CallSub::~CallSub()
{
}

void CallSub::dataChanged()
{
    setCaption(i18n("Call %1", dataString("sub")));
}

void CallSub::generateMicrobe(FlowCode *code)
{
    code->addCode("call " + dataString("sub"));
    code->addCodeBranch(outputPart("stdoutput"));
}
