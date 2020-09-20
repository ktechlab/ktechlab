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

#include <KLocalizedString>

Item *ReadPort::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ReadPort((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *ReadPort::libraryItem()
{
    return new LibraryItem(QStringList(QString("flow/readport")), i18n("Read from Port"), i18n("I\\/O"), "portread.png", LibraryItem::lit_flowpart, ReadPort::construct);
}

ReadPort::ReadPort(ICNDocument *icnDocument, bool newItem, const char *id)
    : FlowPart(icnDocument, newItem, id ? id : "readport")
{
    m_name = i18n("Read from Port");
    initIOSymbol();
    createStdInput();
    createStdOutput();

    createProperty("0-port", Variant::Type::Port);
    property("0-port")->setToolbarCaption(i18n("Read"));
    property("0-port")->setEditorCaption(i18n("Port"));
    property("0-port")->setValue("PORTA");

    createProperty("1-var", Variant::Type::VarName);
    property("1-var")->setToolbarCaption(i18nc("read from port to x", "to"));
    property("1-var")->setEditorCaption(i18n("Variable"));
    property("1-var")->setValue("x");
}

ReadPort::~ReadPort()
{
}

void ReadPort::dataChanged()
{
    setCaption(i18n("Read %1 to %2", dataString("0-port"), dataString("1-var")));
}

void ReadPort::generateMicrobe(FlowCode *code)
{
    code->addCode(dataString("1-var") + " = " + dataString("0-port"));
    code->addCodeBranch(outputPart("stdoutput"));
}
