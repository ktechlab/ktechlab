/***************************************************************************
 *   Copyright (C) 2003,2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecdiode.h"
#include "diode.h"
#include "ecnode.h"
#include "libraryitem.h"

#include <KLocalizedString>
#include <QPainter>

Item *ECDiode::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECDiode(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *ECDiode::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/diode")), i18n("Diode"), i18n("Discrete"), "diode.png", LibraryItem::lit_component, ECDiode::construct);
}

ECDiode::ECDiode(ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, id ? id : "diode")
{
    m_name = i18n("Diode");

    setSize(-8, -8, 16, 16);

    init1PinLeft();
    init1PinRight();

    m_diode = createDiode(m_pNNode[0], m_pPNode[0]);

    DiodeSettings ds; // it will have the default properties that we use

    createProperty("I_S", Variant::Type::Double);
    property("I_S")->setCaption("Saturation Current");
    property("I_S")->setUnit("A");
    property("I_S")->setMinValue(1e-20);
    property("I_S")->setMaxValue(1e-0);
    property("I_S")->setValue(ds.I_S);
    property("I_S")->setAdvanced(true);

    createProperty("N", Variant::Type::Double);
    property("N")->setCaption(i18n("Emission Coefficient"));
    property("N")->setMinValue(1e0);
    property("N")->setMaxValue(1e1);
    property("N")->setValue(ds.N);
    property("N")->setAdvanced(true);

    createProperty("V_B", Variant::Type::Double);
    property("V_B")->setCaption(i18n("Breakdown Voltage"));
    property("V_B")->setUnit("V");
    property("V_B")->setMinAbsValue(1e-5);
    property("V_B")->setMaxValue(1e10);
    property("V_B")->setValue(ds.V_B);
    property("V_B")->setAdvanced(true);

    // 	createProperty( "R", Variant::Type::Double );
    // 	property("R")->setCaption( i18n("Series Resistance") );
    // 	property("R")->setUnit( QChar(0x3a9) );
    // 	property("R")->setMinValue(1e-5);
    // 	property("R")->setMaxValue(1e0);
    // 	property("R")->setValue( ds.R );
    // 	property("R")->setAdvanced(true);
}

ECDiode::~ECDiode()
{
}

void ECDiode::dataChanged()
{
    DiodeSettings ds;

    ds.I_S = dataDouble("I_S");
    ds.V_B = dataDouble("V_B");
    ds.N = dataDouble("N");
    // 	ds.R = dataDouble("R");

    m_diode->setDiodeSettings(ds);
}

void ECDiode::drawShape(QPainter &p)
{
    initPainter(p);

    int _x = int(x());
    int _y = int(y());

    QPolygon pa(3);
    pa[0] = QPoint(8, 0);
    pa[1] = QPoint(-8, -8);
    pa[2] = QPoint(-8, 8);
    pa.translate(_x, _y);
    p.drawPolygon(pa);
    p.drawPolyline(pa);

    p.drawLine(_x + 8, _y - 8, _x + 8, _y + 8);

    deinitPainter(p);
}
