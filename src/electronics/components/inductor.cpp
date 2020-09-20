/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "inductor.h"
#include "inductance.h"
#include "libraryitem.h"

#include <KLocalizedString>
#include <QPainter>

Item *Inductor::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new Inductor((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *Inductor::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/inductor")), i18n("Inductor"), i18n("Passive"), "inductor.png", LibraryItem::lit_component, Inductor::construct);
}

Inductor::Inductor(ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, id ? id : "inductor")
{
    m_name = i18n("Inductor");
    setSize(-16, -8, 32, 16);

    init1PinLeft();
    init1PinRight();

    m_pInductance = createInductance(m_pNNode[0], m_pPNode[0], 0.001);

    createProperty("Inductance", Variant::Type::Double);
    property("Inductance")->setCaption(i18n("Inductance"));
    property("Inductance")->setUnit("H");
    property("Inductance")->setMinValue(1e-12);
    property("Inductance")->setMaxValue(1e12);
    property("Inductance")->setValue(1e-3);

    addDisplayText("inductance", QRect(-8, -24, 16, 16), "", false);
}

Inductor::~Inductor()
{
}

void Inductor::dataChanged()
{
    double inductance = dataDouble("Inductance");

    QString display = QString::number(inductance / getMultiplier(inductance), 'g', 3) + getNumberMag(inductance) + "H";
    setDisplayText("inductance", display);

    m_pInductance->setInductance(inductance);
}

void Inductor::drawShape(QPainter &p)
{
    initPainter(p);
    int _y = int(y());
    int _x = int(x());

    p.drawArc(_x - 16, _y - 5, 11, 11, 0, 180 * 16);
    p.drawArc(_x - 5, _y - 5, 11, 11, 0, 180 * 16);
    p.drawArc(_x + 6, _y - 5, 11, 11, 0, 180 * 16);

    deinitPainter(p);
}
