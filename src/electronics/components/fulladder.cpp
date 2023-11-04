/***************************************************************************
 *   Copyright (C) 2004 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "fulladder.h"

#include "libraryitem.h"
#include "logic.h"

#include <KLocalizedString>

void FullAdder_inStateChanged(void *objV, bool state) { // Enable
    FullAdder *objT = static_cast<FullAdder*>(objV);
    objT->inStateChanged(state);
}

Item *FullAdder::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new FullAdder(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *FullAdder::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/adder")), i18n("Adder"), i18n("Integrated Circuits"), "ic1.png", LibraryItem::lit_component, FullAdder::construct);
}

FullAdder::FullAdder(ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, (id) ? id : "adder")
{
    m_name = i18n("Adder");

    ALogic = BLogic = inLogic = nullptr;
    outLogic = SLogic = nullptr;

    QStringList pins = QString("A,B,>,,S,C").split(',', Qt::KeepEmptyParts);
    initDIPSymbol(pins, 48);
    initDIP(pins);

    ECNode *node;

    node = ecNodeWithID("S");
    SLogic = createLogicOut(node, false);

    node = ecNodeWithID("C");
    outLogic = createLogicOut(node, false);

    node = ecNodeWithID("A");
    ALogic = createLogicIn(node);

    node = ecNodeWithID("B");
    BLogic = createLogicIn(node);

    node = ecNodeWithID(">");
    inLogic = createLogicIn(node);

    //ALogic->setCallback(this, (CallbackPtr)(&FullAdder::inStateChanged));
    ALogic->setCallback2(FullAdder_inStateChanged, this);
    //BLogic->setCallback(this, (CallbackPtr)(&FullAdder::inStateChanged));
    BLogic->setCallback2(FullAdder_inStateChanged, this);
    //inLogic->setCallback(this, (CallbackPtr)(&FullAdder::inStateChanged));
    inLogic->setCallback2(FullAdder_inStateChanged, this);
}

FullAdder::~FullAdder()
{
    ALogic->setCallback2(nullptr, nullptr);
    BLogic->setCallback2(nullptr, nullptr);
    inLogic->setCallback2(nullptr, nullptr);
}

void FullAdder::inStateChanged(bool /*state*/)
{
    const bool A = ALogic->isHigh();
    const bool B = BLogic->isHigh();
    const bool in = inLogic->isHigh();

    const bool out = (!A && B && in) || (A && !B && in) || (A && B);
    const bool S = (!A && !B && in) || (!A && B && !in) || (A && !B && !in) || (A && B && in);

    SLogic->setHigh(S);
    outLogic->setHigh(out);
}
