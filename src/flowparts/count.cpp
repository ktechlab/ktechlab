/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "count.h"

#include "flowcode.h"
#include "libraryitem.h"

#include <KLocalizedString>

Item *Count::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new Count((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *Count::libraryItem()
{
    return new LibraryItem(QStringList(QString("flow/count")), i18n("Count"), i18n("Functions"), "ppcount.png", LibraryItem::lit_flowpart, Count::construct);
}

Count::Count(ICNDocument *icnDocument, bool newItem, const char *id)
    : FlowPart(icnDocument, newItem, id ? id : "count")
{
    m_name = i18n("Count");
    initProcessSymbol();
    createStdInput();
    createStdOutput();

    createProperty("0-trigger", Variant::Type::Select);
    property("0-trigger")->setAllowed((QStringList("rising") << "falling"));
    property("0-triger")->setValue("rising");
    property("0-trigger")->setCaption(i18n("Trigger"));

    createProperty("1-length", Variant::Type::Double);
    property("1-length")->setUnit("sec");
    property("1-length")->setValue(10.0);
    property("1-length")->setCaption("Interval");
}

Count::~Count()
{
}

void Count::dataChanged()
{
    double count = dataDouble("1-length");
    setCaption(i18n("Count %1 for %2 sec", dataString("0-trigger"), QString::number(count / getMultiplier(count), 'g', 3) + getNumberMag(count)));
}

void Count::generateMicrobe(FlowCode *code)
{
    const double count_ms = dataDouble("1-length") * 1e3;
    code->addCode("count " + dataString("0-trigger") + " for " + QString::number(count_ms) + "ms");
    code->addCodeBranch(outputPart("stdoutput"));
}
