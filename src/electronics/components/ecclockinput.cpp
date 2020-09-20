/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecclockinput.h"

#include "libraryitem.h"
#include "logic.h"
#include "simulator.h"

#include <KLocalizedString>
#include <QPainter>

#include <QDebug>

#include <cmath>

using namespace std;

// was a constant, this is my guess for an appropriate name.
//#define TIME_INTERVAL 100 // 2015.09.27 - added proper constant to simulator class

static inline uint roundDouble(const double x)
{
    return uint(std::floor(x + 0.5));
}

Item *ECClockInput::construct(ItemDocument *itemDocument, bool newItem, const char *id)
{
    return new ECClockInput((ICNDocument *)itemDocument, newItem, id);
}

LibraryItem *ECClockInput::libraryItem()
{
    return new LibraryItem(QStringList(QString("ec/clock_input")), i18n("Clock Input"), i18n("Logic"), "clockinput.png", LibraryItem::lit_component, ECClockInput::construct);
}

ECClockInput::ECClockInput(ICNDocument *icnDocument, bool newItem, const char *id)
    : Component(icnDocument, newItem, (id) ? id : "clock_input")
{
    m_name = i18n("Clock Input");
    setSize(-16, -8, 32, 16);

    m_lastSetTime = 0;
    m_time = 0;
    m_high_time = 0;
    m_low_time = 0;
    m_period = 0;
    m_bSetStepCallbacks = true;
    m_pSimulator = Simulator::self();

    for (unsigned i = 0; i < LOGIC_UPDATE_PER_STEP; i++) {
        ComponentCallback *ccb = new ComponentCallback(this, (VoidCallbackPtr)(&ECClockInput::stepCallback));
        m_pComponentCallback[i] = new list<ComponentCallback>;
        m_pComponentCallback[i]->push_back(*ccb);
    }

    init1PinRight();
    m_pOut = createLogicOut(m_pPNode[0], false);

    createProperty("low-time", Variant::Type::Double);
    property("low-time")->setUnit("S");
    property("low-time")->setCaption(i18n("Low Time"));
    property("low-time")->setMinValue(1.0 / LOGIC_UPDATE_RATE);
    property("low-time")->setValue(0.5);

    createProperty("high-time", Variant::Type::Double);
    property("high-time")->setUnit("S");
    property("high-time")->setCaption(i18n("High Time"));
    property("high-time")->setMinValue(1.0 / LOGIC_UPDATE_RATE);
    property("high-time")->setValue(0.5);

    addDisplayText("freq", QRect(-16, -24, 32, 14), "", false);
}

ECClockInput::~ECClockInput()
{
    for (unsigned i = 0; i < LOGIC_UPDATE_PER_STEP; i++)
        delete m_pComponentCallback[i];
}

void ECClockInput::dataChanged()
{
    m_high_time = roundDouble(dataDouble("high-time") * LOGIC_UPDATE_RATE);
    m_low_time = roundDouble(dataDouble("low-time") * LOGIC_UPDATE_RATE);
    m_period = m_low_time + m_high_time;

    const double frequency = 1. / (dataDouble("high-time") + dataDouble("low-time"));
    QString display = QString::number(frequency / getMultiplier(frequency), 'g', 3) + getNumberMag(frequency) + "Hz";
    setDisplayText("freq", display);

    bool setStepCallbacks = m_period > LOGIC_UPDATE_PER_STEP;
    if (setStepCallbacks != m_bSetStepCallbacks) {
        m_bSetStepCallbacks = setStepCallbacks;
        if (setStepCallbacks) {
            m_pSimulator->detachComponentCallbacks(*this);
        } else {
            m_pSimulator->attachComponentCallback(this, (VoidCallbackPtr)(&ECClockInput::stepLogic));
        }
    }

    m_bLastStepCallbackOut = false;
    m_lastSetTime = m_pSimulator->time();
    if (m_lastSetTime < 0) {
        qWarning() << Q_FUNC_INFO << " m_lastSetTime = " << m_lastSetTime;
    }
}

void ECClockInput::stepLogic()
{
    m_pOut->setHigh(m_time > m_low_time);

    ++m_time;
    if (m_time > m_period) {
        m_time -= int(m_time / m_period) * m_period;
    }
}

void ECClockInput::stepCallback()
{
    m_pOut->setHigh(m_bLastStepCallbackOut);
    m_bLastStepCallbackOut = !m_bLastStepCallbackOut;
}

void ECClockInput::stepNonLogic()
{
    if (!m_bSetStepCallbacks) {
        return;
    }

    bool addingHigh = !m_bLastStepCallbackOut;

    //     { // for testing
    //         const long long remainingLogicSteps = m_pSimulator->time() % LOGIC_UPDATE_PER_STEP;
    //         if (remainingLogicSteps != 0) {
    //             qWarning() << Q_FUNC_INFO << "remainingLogicSteps should be 0, but got " << remainingLogicSteps;
    //         }
    //     }

    // TODO review this method

    long long lowerTime = m_pSimulator->time();
    long long upperTime = lowerTime + LOGIC_UPDATE_PER_STEP;

    long long upTo = m_lastSetTime;

    while (upTo + (addingHigh ? m_high_time : m_low_time) < upperTime) {
        upTo += addingHigh ? m_high_time : m_low_time;
        addingHigh = !addingHigh;

        long long at = upTo - lowerTime;
        if (at >= 0 && at < LOGIC_UPDATE_PER_STEP)
            m_pSimulator->addStepCallback(at, &m_pComponentCallback[at]->front());
    }

    m_lastSetTime = upTo;
    if (m_lastSetTime < 0) {
        qWarning() << Q_FUNC_INFO << " m_lastSetTime = " << m_lastSetTime;
    }
}

void ECClockInput::drawShape(QPainter &p)
{
    initPainter(p);

    int _x = (int)x() - 10;
    int _y = (int)y() - 8;

    p.drawRect(_x - 6, _y, 32, 16);

    p.drawLine(_x, _y + 8, _x, _y + 4);
    p.drawLine(_x, _y + 4, _x + 4, _y + 4);
    p.drawLine(_x + 4, _y + 4, _x + 4, _y + 12);
    p.drawLine(_x + 4, _y + 12, _x + 8, _y + 12);
    p.drawLine(_x + 8, _y + 12, _x + 8, _y + 4);
    p.drawLine(_x + 8, _y + 4, _x + 12, _y + 4);
    p.drawLine(_x + 12, _y + 4, _x + 12, _y + 12);
    p.drawLine(_x + 12, _y + 12, _x + 16, _y + 12);
    p.drawLine(_x + 16, _y + 12, _x + 16, _y + 4);
    p.drawLine(_x + 16, _y + 4, _x + 20, _y + 4);
    p.drawLine(_x + 20, _y + 4, _x + 20, _y + 8);

    deinitPainter(p);
}
