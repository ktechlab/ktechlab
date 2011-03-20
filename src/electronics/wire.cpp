/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <cmath>
#include <cassert>
// #include <kdebug.h>
#include <qdebug.h>

#include "pin.h"
#include "wire.h"
#include "simulator.h"

Wire::Wire(Pin *startPin, Pin *endPin) :
    m_current(NAN), m_bCurrentIsKnown(false)
{
    assert(startPin && endPin);

    m_pStartPin = startPin;
    m_pEndPin = endPin;

    m_pStartPin->addWire(this);
    m_pEndPin->addWire(this);
}

Wire::~Wire()
{
// if this fails, we've already been deleted. Because the memory allocation subsystem isn't fine-grained, we
// don't immediately segfault when the caller tries to call us, but rather further down when we try to de-refference
// the pins which we zeroed out below. This assertion should be left in place until the underlying bug is fixed.
assert(m_pStartPin && m_pEndPin);

    m_pStartPin->removeWire(this);
    m_pEndPin->removeWire(this);

    // make sure we're conspicuously invalid to help debugging, cuz
    // there are many dangling refferences to this class. =(
    m_pStartPin = m_pEndPin = 0;
}

double Wire::currentFor(const Pin *aPin) const {
    /*
        Current flows from the start pin to the end pin.
    */
    if(aPin == m_pStartPin) return -m_current;
    if(aPin == m_pEndPin ) return   m_current;

    qWarning() << "BUG: requesting current for a pin not associated "
        "with the connector";
    return 0;
}

double Wire::voltage() const
{
    double temp;
    if((temp = m_pStartPin->voltage() - m_pEndPin->voltage()) && Simulator::self()->isSimulating()) {
        // kdError() << k_funcinfo << "Wire voltage error: " << temp << endl;
        qCritical() << "Wire voltage error: " << temp << endl;
    }

    return m_pStartPin->voltage();
}

void Wire::setCurrentKnown( bool known )
{
    m_bCurrentIsKnown = known;
    if (!known) m_current = NAN;
}

/*!
    \fn Wire::method_1(const Pin aPin) const
 */
Pin *Wire::otherPin(const Pin *aPin) const
{
    if(aPin == m_pStartPin) return m_pEndPin;
    if(aPin == m_pEndPin) return m_pStartPin;
    // got here? bug!
    qCritical() << "BUG: tried to get other pin for a non-associated wire" ;
    return NULL;
}

void Wire::setCurrentFor(const Pin* pin, double current)
{
    /*  currents are flwoing into the pin;
        in the wire, the current is from start to end */
    if(pin == m_pStartPin)
        m_current = -current;
    else
        if( pin == m_pEndPin)
            m_current = current;
        else {
            qCritical() << "BUG: trying to set current on a non-associated end of a wire";
            return;
        }
    m_bCurrentIsKnown = true;
}

#include "wire.moc"
