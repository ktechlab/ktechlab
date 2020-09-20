/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecjfet.h"
#include "jfet.h"
#include "libraryitem.h"

#include <KLocalizedString>
#include <QPainter>

Item *ECJFET::constructNJFET(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECJFET(JFET::nJFET, (ICNDocument *)itemDocument, newItem, id ? id : "njfet");
}

Item *ECJFET::constructPJFET(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECJFET(JFET::pJFET, (ICNDocument *)itemDocument, newItem, id ? id : "pjfet");
}

LibraryItem *ECJFET::libraryItemNJFET()
{
    return new LibraryItem(QStringList(QString("ec/njfet")),
                           // 	i18n("n JFET"),
                           i18n("n-JFET"),
                           i18n("Discrete"),
                           "njfet.png",
                           LibraryItem::lit_component,
                           ECJFET::constructNJFET);
}

LibraryItem *ECJFET::libraryItemPJFET()
{
    return new LibraryItem(QStringList(QString("ec/pjfet")),
                           // 	i18n("p JFET"),
                           i18n("p-JFET"),
                           i18n("Discrete"),
                           "pjfet.png",
                           LibraryItem::lit_component,
                           ECJFET::constructPJFET);
}

ECJFET::ECJFET(int JFET_type, ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, id)
{
    m_JFET_type = JFET_type;
    if (JFET_type == JFET::nJFET)
        m_name = i18n("N-Channel JFET");
    else
        m_name = i18n("P-Channel JFET");

    setSize(-8, -8, 16, 16);
    m_pJFET = createJFET(createPin(8, -16, 90, "D"), createPin(-16, 0, 0, "G"), createPin(8, 16, 270, "S"), JFET_type);

    JFETSettings s; // will be created with the default settings

    Variant *v = createProperty("V_Th", Variant::Type::Double);
    v->setCaption(i18n("Threshold voltage"));
    v->setUnit("V");
    v->setMinValue(-1e6);
    v->setMaxValue(1e6);
    v->setValue(s.V_Th);
    v->setAdvanced(true);

    v = createProperty("beta", Variant::Type::Double);
    v->setCaption(i18n("Transcondutance"));
    v->setUnit(QString("A/V") + QChar(0xb2));
    v->setMinValue(1e-12);
    v->setMaxValue(1e0);
    v->setValue(s.beta);
    v->setAdvanced(true);

    v = createProperty("I_S", Variant::Type::Double);
    v->setCaption(i18n("Saturation current"));
    v->setUnit("A");
    v->setMinValue(1e-20);
    v->setMaxValue(1e0);
    v->setValue(s.I_S);
    v->setAdvanced(true);

    v = createProperty("N", Variant::Type::Double);
    v->setCaption(i18n("PN emission coefficient"));
    v->setUnit("");
    v->setMinValue(0.0);
    v->setMaxValue(10.0);
    v->setValue(s.N);
    v->setAdvanced(true);

    v = createProperty("N_R", Variant::Type::Double);
    v->setCaption(i18n("Isr emission coefficient"));
    v->setUnit("");
    v->setMinValue(0.0);
    v->setMaxValue(10.0);
    v->setValue(s.N_R);
    v->setAdvanced(true);
}

ECJFET::~ECJFET()
{
}

void ECJFET::dataChanged()
{
    JFETSettings s;
    s.V_Th = dataDouble("V_Th");
    s.beta = dataDouble("beta");
    s.I_S = dataDouble("I_S");
    s.N = dataDouble("N");
    s.N_R = dataDouble("N_R");

    m_pJFET->setJFETSettings(s);
}

void ECJFET::drawShape(QPainter &p)
{
    const int _x = int(x());
    const int _y = int(y());

    initPainter(p);

    // back lines
    p.drawLine(_x - 8, _y, _x + 2, _y);
    p.drawLine(_x + 2, _y - 8, _x + 2, _y + 8);

    // top corner
    p.drawLine(_x + 2, _y - 5, _x + 8, _y - 5);
    p.drawLine(_x + 8, _y - 5, _x + 8, _y - 8);

    // bottom corner
    p.drawLine(_x + 2, _y + 5, _x + 8, _y + 5);
    p.drawLine(_x + 8, _y + 5, _x + 8, _y + 8);

    QPolygon pa(3);
    if (m_JFET_type == JFET::nJFET) {
        // right pointing arrow
        pa[0] = QPoint(1, 0);
        pa[1] = QPoint(-4, -3);
        pa[2] = QPoint(-4, +3);
    } else {
        // left pointing arrow
        pa[0] = QPoint(-8, 0);
        pa[1] = QPoint(-3, -3);
        pa[2] = QPoint(-3, +3);
    }
    pa.translate(_x, _y);
    p.setBrush(p.pen().color());
    p.drawPolygon(pa);

    deinitPainter(p);
}
