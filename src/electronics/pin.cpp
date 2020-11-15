/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "pin.h"

#include <QDebug>

#include <cassert>

#include <ktechlab_debug.h>

Pin::Pin(ECNode *parent)
{
    assert(parent);
    m_pECNode = parent;
    m_voltage = 0.;
    m_current = 0.;
    m_eqId = -2;
    m_bCurrentIsKnown = false;
    m_groundType = Pin::gt_never;
}

Pin::~Pin()
{
    WireList::iterator end = m_inputWireList.end();
    for (WireList::iterator it = m_inputWireList.begin(); it != end; ++it)
        delete (Wire *)(*it);

    end = m_outputWireList.end();
    for (WireList::iterator it = m_outputWireList.begin(); it != end; ++it)
        delete (Wire *)(*it);
}

PinList Pin::localConnectedPins() const
{
    // 	qCDebug(KTL_LOG) << "Input wires: "<<m_inputWireList.size()<<"   Output wires: " << m_outputWireList.size() << "   Switch connected: " << m_switchConnectedPins.size() << endl;

    PinList pins;

    WireList::const_iterator end = m_inputWireList.end();
    for (WireList::const_iterator it = m_inputWireList.begin(); it != end; ++it) {
        if (*it)
            pins << (*it)->startPin();
    }

    end = m_outputWireList.end();
    for (WireList::const_iterator it = m_outputWireList.begin(); it != end; ++it) {
        if (*it)
            pins << (*it)->endPin();
    }

    pins += m_switchConnectedPins;

    return pins;
}

void Pin::setSwitchConnected(Pin *pin, bool isConnected)
{
    if (!pin)
        return;

    if (isConnected) {
        if (!m_switchConnectedPins.contains(pin))
            m_switchConnectedPins.append(pin);
    } else
        m_switchConnectedPins.removeAll(pin);
}

void Pin::setSwitchCurrentsUnknown()
{
    if (!m_switchList.empty()) {
        m_switchList.removeAt(0l);
    } else {
        qCDebug(KTL_LOG) << "Pin::setSwitchCurrentsUnknown - WARN - unexpected empty switch list";
    }
    m_unknownSwitchCurrents = m_switchList;
}

void Pin::addCircuitDependentPin(Pin *pin)
{
    if (pin && !m_circuitDependentPins.contains(pin))
        m_circuitDependentPins.append(pin);
}

void Pin::addGroundDependentPin(Pin *pin)
{
    if (pin && !m_groundDependentPins.contains(pin))
        m_groundDependentPins.append(pin);
}

void Pin::removeDependentPins()
{
    m_circuitDependentPins.clear();
    m_groundDependentPins.clear();
}

void Pin::addElement(Element *e)
{
    if (!e || m_elementList.contains(e))
        return;
    m_elementList.append(e);
}

void Pin::removeElement(Element *e)
{
    m_elementList.removeAll(e);
}

void Pin::addSwitch(Switch *sw)
{
    if (!sw || m_switchList.contains(sw))
        return;
    m_switchList << sw;
}

void Pin::removeSwitch(Switch *sw)
{
    m_switchList.removeAll(sw);
}

void Pin::addInputWire(Wire *wire)
{
    if (wire && !m_inputWireList.contains(wire))
        m_inputWireList << wire;
}

void Pin::addOutputWire(Wire *wire)
{
    if (wire && !m_outputWireList.contains(wire))
        m_outputWireList << wire;
}

bool Pin::calculateCurrentFromWires()
{
    m_inputWireList.removeAll((Wire *)nullptr);
    m_outputWireList.removeAll((Wire *)nullptr);

    const WireList inputs = m_inputWireList;
    const WireList outputs = m_outputWireList;

    m_current = 0.0;

    WireList::const_iterator end = inputs.end();
    for (WireList::const_iterator it = inputs.begin(); it != end; ++it) {
        if (!(*it)->currentIsKnown())
            return false;

        m_current -= (*it)->current();
    }

    end = outputs.end();
    for (WireList::const_iterator it = outputs.begin(); it != end; ++it) {
        if (!(*it)->currentIsKnown())
            return false;

        m_current += (*it)->current();
    }

    m_bCurrentIsKnown = true;
    return true;
}
