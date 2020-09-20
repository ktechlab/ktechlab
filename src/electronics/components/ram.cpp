/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ram.h"
#include "libraryitem.h"
#include "logic.h"
#include "variant.h"

#include <KLocalizedString>
#include <cmath>

Item *RAM::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new RAM((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *RAM::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/ram")), i18n("RAM"), i18n("Integrated Circuits"), "ic2.png", LibraryItem::lit_component, RAM::construct);
}

RAM::RAM(ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, id ? id : "ram")
{
    m_name = i18n("RAM");

    // m_data = nullptr; // should be initialized by the constructor anyway
    m_pCS = nullptr;
    m_pOE = nullptr;
    m_pWE = nullptr;
    m_wordSize = 0;
    m_addressSize = 0;

    createProperty("wordSize", Variant::Type::Int);
    property("wordSize")->setCaption(i18n("Word Size"));
    property("wordSize")->setMinValue(1);
    property("wordSize")->setMaxValue(64);
    property("wordSize")->setValue(2);

    createProperty("addressSize", Variant::Type::Int);
    property("addressSize")->setCaption(i18n("Address Size"));
    property("addressSize")->setMinValue(1);
    property("addressSize")->setMaxValue(24);
    property("addressSize")->setValue(4);

    m_data = createProperty("data", Variant::Type::Raw)->value().toBitArray();
}

RAM::~RAM()
{
}

void RAM::dataChanged()
{
    m_wordSize = dataInt("wordSize");
    m_addressSize = dataInt("addressSize");

    int newSize = int(m_wordSize * std::pow(2., m_addressSize));
    m_data.resize(newSize);

    initPins();
}

void RAM::inStateChanged(bool newState)
{
    Q_UNUSED(newState);

    bool cs = m_pCS->isHigh();
    bool oe = m_pOE->isHigh();
    bool we = m_pWE->isHigh();

    if (!cs || !oe) {
        for (int i = 0; i < m_wordSize; ++i)
            m_dataOut[i]->setHigh(false);
    }

    if (!cs || (!oe && !we))
        return;

    unsigned address = 0;
    for (int i = 0; i < m_addressSize; ++i)
        address += (m_address[i]->isHigh() ? 1 : 0) << i;

    if (we) {
        for (int i = 0; i < m_wordSize; ++i)
            m_data[m_wordSize * address + i] = m_dataIn[i]->isHigh();
    }

    if (oe) {
        for (int i = 0; i < m_wordSize; ++i)
            m_dataOut[i]->setHigh(m_data[m_wordSize * address + i]);
    }
}

void RAM::initPins()
{
    int oldWordSize = m_dataIn.size();
    int oldAddressSize = m_address.size();

    int newWordSize = dataInt("wordSize");
    int newAddressSize = dataInt("addressSize");

    if (newAddressSize == oldAddressSize && newWordSize == oldWordSize)
        return;

    QStringList leftPins; // Pins on left of IC
    leftPins << "CS"
             << "OE"
             << "WE";
    for (int i = 0; i < newAddressSize; ++i)
        leftPins << QString("A%1").arg(QString::number(i));

    QStringList rightPins; // Pins on right of IC
    for (unsigned i = newWordSize; i > 0; --i)
        rightPins << QString("DI%1").arg(QString::number(i - 1));
    for (unsigned i = newWordSize; i > 0; --i)
        rightPins << QString("DO%1").arg(QString::number(i - 1));

    // Make pin lists of consistent sizes
    for (unsigned i = leftPins.size(); i < rightPins.size(); ++i)
        leftPins.append("");
    for (unsigned i = rightPins.size(); i < leftPins.size(); ++i)
        rightPins.prepend("");

    QStringList pins = leftPins + rightPins;

    initDIPSymbol(pins, 72);
    initDIP(pins);

    ECNode *node;

    if (!m_pCS) {
        node = ecNodeWithID("CS");
        m_pCS = createLogicIn(node);
        m_pCS->setCallback(this, (CallbackPtr)(&RAM::inStateChanged));
    }

    if (!m_pOE) {
        node = ecNodeWithID("OE");
        m_pOE = createLogicIn(node);
        m_pOE->setCallback(this, (CallbackPtr)(&RAM::inStateChanged));
    }

    if (!m_pWE) {
        node = ecNodeWithID("WE");
        m_pWE = createLogicIn(node);
        m_pWE->setCallback(this, (CallbackPtr)(&RAM::inStateChanged));
    }

    if (newWordSize > oldWordSize) {
        m_dataIn.resize(newWordSize);
        m_dataOut.resize(newWordSize);

        for (int i = oldWordSize; i < newWordSize; ++i) {
            node = ecNodeWithID(QString("DI%1").arg(QString::number(i)));
            m_dataIn.insert(i, createLogicIn(node));
            m_dataIn[i]->setCallback(this, (CallbackPtr)(&RAM::inStateChanged));

            node = ecNodeWithID(QString("DO%1").arg(QString::number(i)));
            m_dataOut.insert(i, createLogicOut(node, false));
        }
    } else if (newWordSize < oldWordSize) {
        for (int i = newWordSize; i < oldWordSize; ++i) {
            QString id = QString("DO%1").arg(QString::number(i));
            removeDisplayText(id);
            removeElement(m_dataIn[i], false);
            removeNode(id);

            id = QString("DI%1").arg(QString::number(i));
            removeDisplayText(id);
            removeElement(m_dataOut[i], false);
            removeNode(id);
        }

        m_dataIn.resize(newWordSize);
        m_dataOut.resize(newWordSize);
    }

    if (newAddressSize > oldAddressSize) {
        m_address.resize(newAddressSize);

        for (int i = oldAddressSize; i < newAddressSize; ++i) {
            node = ecNodeWithID(QString("A%1").arg(QString::number(i)));
            m_address.insert(i, createLogicIn(node));
            m_address[i]->setCallback(this, (CallbackPtr)(&RAM::inStateChanged));
        }
    } else if (newAddressSize < oldAddressSize) {
        for (int i = newAddressSize; i < oldAddressSize; ++i) {
            QString id = QString("A%1").arg(QString::number(i));
            removeDisplayText(id);
            removeElement(m_address[i], false);
            removeNode(id);
        }

        m_address.resize(newAddressSize);
    }
}
