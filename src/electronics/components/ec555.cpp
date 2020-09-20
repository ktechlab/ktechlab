/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ec555.h"

#include "ecnode.h"
#include "libraryitem.h"
#include "pin.h"
#include "resistance.h"

#include <KLocalizedString>
#include <QPainter>

Item *EC555::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new EC555((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *EC555::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/555")), i18n("555"), i18n("Integrated Circuits"), "ic1.png", LibraryItem::lit_component, EC555::construct);
}

EC555::EC555(ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, (id) ? id : "555")
{
    m_name = i18n("555");
    // 	m_pins = QStringList::split( ',', "Gnd,Trg,Out,Res,CV,Th,Dis,Vcc" );
    // 	m_pins = QStringList::split( ',', "Dis,Th,Trg,Gnd,CV,Out,Res,Vcc" );

    old_com1 = false;
    old_com2 = false;
    old_q = false;

    setSize(-32, -32, 64, 64);

    // Pins down left

    // Pin 7
    discharge = createPin(-40, -16, 0, "Dis")->pin();
    addDisplayText("dis", QRect(-32, -24, 24, 16), "Dis");

    // Pin 6
    threshold = createPin(-40, 0, 0, "Th")->pin();
    addDisplayText("th", QRect(-32, -8, 24, 16), "Th");

    // Pin 2
    trigger = createPin(-40, 16, 0, "Trg")->pin();
    addDisplayText("trg", QRect(-32, 8, 24, 16), "Trg");

    // Top two

    // Pin 8
    vcc = createPin(-16, -40, 90, "Vcc")->pin();
    addDisplayText("vcc", QRect(-24, -32, 16, 8), "+");

    // Pin 4
    reset = createPin(16, -40, 90, "Res")->pin();
    addDisplayText("res", QRect(8, -28, 16, 16), "Res");

    // Bottom two

    // Pin 1
    ground = createPin(-16, 40, 270, "Gnd")->pin();
    addDisplayText("gnd", QRect(-24, 20, 16, 8), "-");

    // Pin 5
    control = createPin(16, 40, 270, "CV")->pin();
    addDisplayText("cv", QRect(8, 12, 16, 16), "CV");

    // Output on right

    // Pin 3
    output = createPin(40, 0, 180, "Out")->pin();
    addDisplayText("out", QRect(8, -8, 16, 16), "Out");

    m_r1 = createResistance(vcc, control, 5e3);
    m_r23 = createResistance(control, ground, 1e4);
    m_po_sink = createResistance(output, ground, 0.);
    m_po_source = createResistance(output, vcc, 0.);
    m_po_source->setConductance(0.);
    m_r_discharge = createResistance(discharge, ground, 0.);
}

EC555::~EC555()
{
}

// TODO: This is simulation code not UI code, so it shouldn't be here.
// Would it be better to simulate the appropriate elements, ie comparator, voltage divider,
// and flip-flop instead of all this hand-wavy logic?

void EC555::stepNonLogic()
{
    double v_threshold = threshold->voltage();
    double v_control = control->voltage();
    double v_ground = ground->voltage();
    double v_trigger = trigger->voltage();
    double v_reset = reset->voltage();
    double v_vcc = vcc->voltage();
    double v_r = (v_control + v_ground) / 2;

    bool com1 = (v_threshold == v_control) ? old_com1 : (v_threshold < v_control);
    bool com2 = (v_r == v_trigger) ? old_com2 : (v_r > v_trigger);
    bool reset = v_reset >= (v_control - v_ground) / 2 + v_ground;

    old_com1 = com1;
    old_com2 = com2;

    bool r = !(reset && com1);
    bool s = com2;
    bool q = old_q;

    if (v_vcc - v_ground >= 2.5) {
        if (s && !r) {
            q = true;
        } else if (r && !s) {
            q = false;
        }
    } else {
        q = false;
    }

    old_q = q;
    m_r_discharge->setConductance(0.);

    if (q) {
        m_po_source->setResistance(10.);
        m_po_sink->setConductance(0.);
    } else {
        m_po_source->setConductance(0.);
        m_po_sink->setResistance(10.);

        if (v_ground + 0.7 <= v_vcc) {
            m_r_discharge->setResistance(10.);
        }
    }
}
