/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "resistordip.h"
#include "libraryitem.h"
#include "node.h"
#include "resistance.h"

#include <KLocalizedString>
#include <QPainter>

Item *ResistorDIP::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ResistorDIP(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *ResistorDIP::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/resistordip")), i18n("Resistor DIP"), i18n("Passive"), "resistordip.png", LibraryItem::lit_component, ResistorDIP::construct);
}

ResistorDIP::ResistorDIP(ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, id ? id : "multiplexer")
{
    m_name = i18n("Resistor DIP");

    m_resistorCount = 0;
    for (int i = 0; i < maxCount; ++i)
        m_resistance[i] = nullptr;

    createProperty("resistance", Variant::Type::Double);
    property("resistance")->setCaption(i18n("Resistance"));
    property("resistance")->setUnit(QChar(0x3a9));
    property("resistance")->setValue(1e4);
    property("resistance")->setMinValue(1e-6);

    createProperty("count", Variant::Type::Int);
    property("count")->setCaption(i18n("Count"));
    property("count")->setMinValue(2);
    property("count")->setMaxValue(maxCount);
    property("count")->setValue(8);
}

ResistorDIP::~ResistorDIP()
{
}

void ResistorDIP::dataChanged()
{
    initPins();
    const double resistance = dataDouble("resistance");
    for (int i = 0; i < m_resistorCount; ++i)
        m_resistance[i]->setResistance(resistance);

    const QString display = QString::number(resistance / getMultiplier(resistance), 'g', 3) + getNumberMag(resistance) + QChar(0x3a9);
    addDisplayText("res", QRect(offsetX(), offsetY() - 16, 32, 12), display);
}

void ResistorDIP::initPins()
{
    const int count = dataInt("count");
    const double resistance = dataDouble("resistance");

    if (count == m_resistorCount)
        return;

    if (count < m_resistorCount) {
        for (int i = count; i < m_resistorCount; ++i) {
            removeElement(m_resistance[i], false);
            m_resistance[i] = nullptr;
            removeNode("n" + QString::number(i));
            removeNode("p" + QString::number(i));
        }
    } else {
        for (int i = m_resistorCount; i < count; ++i) {
            const QString nid = "n" + QString::number(i);
            const QString pid = "p" + QString::number(i);
            m_resistance[i] = createResistance(createPin(-24, 0, 0, nid), createPin(24, 0, 180, pid), resistance);
        }
    }
    m_resistorCount = count;

    setSize(-16, -count * 8, 32, count * 16, true);
    updateDIPNodePositions();
}

void ResistorDIP::updateDIPNodePositions()
{
    for (int i = 0; i < m_resistorCount; ++i) {
        m_nodeMap["n" + QString::number(i)].y = offsetY() + 8 + 16 * i;
        m_nodeMap["p" + QString::number(i)].y = offsetY() + 8 + 16 * i;
    }
    updateAttachedPositioning();
}

void ResistorDIP::drawShape(QPainter &p)
{
    int _x = int(x() + offsetX());
    int _y = int(y() + offsetY());

    initPainter(p);
    for (int i = 0; i < m_resistorCount; ++i)
        p.drawRect(_x, _y + 16 * i + 2, 32, 12);
    deinitPainter(p);
}
