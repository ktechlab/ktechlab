/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "readport.h"

#include "flowcode.h"
#include "libraryitem.h"
#include "icndocument.h"

#include <KLocalizedString>

#include <QLatin1StringView>

using Qt::StringLiterals::operator""_L1;

Item *ReadPort::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ReadPort(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *ReadPort::libraryItem()
{
    return new LibraryItem(QStringList(QLatin1StringView("flow/readport")), i18n("Read from Port"), i18n("I\\/O"),
                           QLatin1StringView("portread.png"), LibraryItem::lit_flowpart, ReadPort::construct);
}

ReadPort::ReadPort(ICNDocument *icnDocument, bool newItem, const char *id)
    : FlowPart(icnDocument, newItem, id ? QLatin1StringView(id) : QLatin1StringView("readport"))
{
    m_name = i18n("Read from Port");
    initIOSymbol();
    createStdInput();
    createStdOutput();

    createProperty("0-port"_L1, Variant::Type::Port);
    property("0-port"_L1)->setToolbarCaption(i18n("Read"));
    property("0-port"_L1)->setEditorCaption(i18n("Port"));
    property("0-port"_L1)->setValue("PORTA"_L1);

    createProperty("1-var"_L1, Variant::Type::VarName);
    property("1-var"_L1)->setToolbarCaption(i18nc("read from port to x", "to"));
    property("1-var"_L1)->setEditorCaption(i18n("Variable"));
    property("1-var"_L1)->setValue("x"_L1);
}

ReadPort::~ReadPort()
{
}

void ReadPort::dataChanged()
{
    setCaption(i18n("Read %1 to %2", dataString("0-port"_L1), dataString("1-var"_L1)));
}

void ReadPort::generateMicrobe(FlowCode *code)
{
    code->addCode(dataString("1-var"_L1) + " = "_L1 + dataString("0-port"_L1));
    code->addCodeBranch(outputPart("stdoutput"_L1));
}
