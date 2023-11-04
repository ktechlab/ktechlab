/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "pushswitch.h"
#include "canvasitemparts.h"
#include "libraryitem.h"
#include "switch.h"

#include <KLocalizedString>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
//#include <q3pointarray.h>

// BEGIN class ECPTBSwitch
Item *ECPTBSwitch::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECPTBSwitch(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *ECPTBSwitch::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/ptb_switch")), i18n("Push-to-Break"), i18n("Switches"), "ptb.png", LibraryItem::lit_component, ECPTBSwitch::construct);
}

ECPTBSwitch::ECPTBSwitch(ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, (id) ? id : "ptb_switch")
{
    m_name = i18n("Push to Break");
    setSize(-16, -16, 32, 24);

    addButton("button", QRect(-16, 8, 32, 20), "");

    createProperty("button_text", Variant::Type::String);
    property("button_text")->setCaption(i18n("Button Text"));

    Variant *v = createProperty("bounce", Variant::Type::Bool);
    v->setCaption(i18n("Bounce"));
    v->setAdvanced(true);
    v->setValue(false);

    v = createProperty("bounce_period", Variant::Type::Double);
    v->setCaption(i18n("Bounce Period"));
    v->setAdvanced(true);
    v->setUnit("s");
    v->setValue(5e-3);

    init1PinLeft(0);
    init1PinRight(0);

    m_switch = createSwitch(m_pPNode[0], m_pNNode[0], false);
    pressed = false;
}

ECPTBSwitch::~ECPTBSwitch()
{
}

void ECPTBSwitch::dataChanged()
{
    button("button")->setText(dataString("button_text"));

    bool bounce = dataBool("bounce");
    int bouncePeriod_ms = int(dataDouble("bounce_period") * 1e3);
    m_switch->setBounce(bounce, bouncePeriod_ms);
}

void ECPTBSwitch::drawShape(QPainter &p)
{
    initPainter(p);

    int _x = int(x()) - 16;
    int _y = int(y()) - 8;
    const int radius = 2;
    const int _height = height() - 8;

    int dy = pressed ? 6 : 4;

    p.drawLine(_x + width() / 4, _y + dy, _x + (3 * width()) / 4, _y + dy);                           // Top horizontal line
    p.drawLine(_x, _y + (_height / 2) - radius + dy, _x + width(), _y + (_height / 2) - radius + dy); // Bottom horizontal line
    p.drawLine(_x + width() / 2, _y + dy, _x + width() / 2, _y + (_height / 2) - radius + dy);        // Vertical line

    p.drawEllipse(_x, _y + (_height / 2) - radius, 2 * radius, 2 * radius);                            // Left circle
    p.drawEllipse(_x + width() - 2 * radius + 1, _y + (_height / 2) - radius, 2 * radius, 2 * radius); // Right circle

    deinitPainter(p);
}

void ECPTBSwitch::buttonStateChanged(const QString &id, bool state)
{
    if (id != "button")
        return;
    m_switch->setState(state ? Switch::Open : Switch::Closed);
    pressed = state;
}
// END class ECPTBSwitch

// BEGIN class ECPTMSwitch
Item *ECPTMSwitch::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECPTMSwitch(static_cast<ICNDocument *>(itemDocument), newItem, id);
}

LibraryItem *ECPTMSwitch::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/ptm_switch")), i18n("Push-to-Make"), i18n("Switches"), "ptm.png", LibraryItem::lit_component, ECPTMSwitch::construct);
}

ECPTMSwitch::ECPTMSwitch(ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, (id) ? id : "ptm_switch")
{
    m_name = i18n("Push to Make");
    setSize(-16, -16, 32, 24);

    addButton("button", QRect(-16, 8, 32, 20), "");

    createProperty("button_text", Variant::Type::String);
    property("button_text")->setCaption(i18n("Button Text"));

    Variant *v = createProperty("bounce", Variant::Type::Bool);
    v->setCaption("Bounce");
    v->setAdvanced(true);
    v->setValue(false);

    v = createProperty("bounce_period", Variant::Type::Double);
    v->setCaption("Bounce Period");
    v->setAdvanced(true);
    v->setUnit("s");
    v->setValue(5e-3);

    init1PinLeft(0);
    init1PinRight(0);

    m_switch = createSwitch(m_pPNode[0], m_pNNode[0], true);
    pressed = false;
}

ECPTMSwitch::~ECPTMSwitch()
{
}

void ECPTMSwitch::dataChanged()
{
    button("button")->setText(dataString("button_text"));

    bool bounce = dataBool("bounce");
    int bouncePeriod_ms = int(dataDouble("bounce_period") * 1e3);
    m_switch->setBounce(bounce, bouncePeriod_ms);
}

void ECPTMSwitch::drawShape(QPainter &p)
{
    initPainter(p);

    int _x = int(x()) - 16;
    int _y = int(y()) - 8;
    const int radius = 2;
    const int _height = height() - 8;

    int dy = pressed ? 1 : 3;

    p.drawLine(_x + width() / 4, _y - dy, _x + (3 * width()) / 4, _y - dy);                           // Top horizontal line
    p.drawLine(_x, _y + (_height / 2) - radius - dy, _x + width(), _y + (_height / 2) - radius - dy); // Bottom horizontal line
    p.drawLine(_x + width() / 2, _y - dy, _x + width() / 2, _y + (_height / 2) - radius - dy);        // Vertical line

    p.drawEllipse(_x, _y + (_height / 2) - radius, 2 * radius, 2 * radius);                            // Left circle
    p.drawEllipse(_x + width() - 2 * radius + 1, _y + (_height / 2) - radius, 2 * radius, 2 * radius); // Right circle

    deinitPainter(p);
}

void ECPTMSwitch::buttonStateChanged(const QString &id, bool state)
{
    if (id != "button")
        return;
    m_switch->setState(state ? Switch::Closed : Switch::Open);
    pressed = state;
}
// END class ECPTMSwitch
