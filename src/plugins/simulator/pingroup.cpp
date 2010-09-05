/*
    KTechLab, and IDE for electronics
    Copyright (C) 2010 Zoltan Padrah

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "pingroup.h"
#include <qlinkedlist.h>
#include <qmap.h>
#include <interfaces/simulator/iwire.h>
#include <interfaces/simulator/ipin.h>
#include <kdebug.h>

using namespace KTechLab;

KTechLab::PinGroup::PinGroup(IPin* startPin)
{
    QSet<IPin*> takenPins;
    QLinkedList<IPin*> toBeInspected;
    IPin *currentPin;
    IPin *toBeAddedPin;

    takenPins.clear();
    takenPins.insert(startPin);
    toBeInspected.clear();
    toBeInspected.append(startPin);
    while( ! toBeInspected.isEmpty() ){
        currentPin = toBeInspected.takeFirst();
        foreach(IWire *wire, currentPin->connections() ){
            // get the other end
            toBeAddedPin = 0;
            if( wire->startPin() == currentPin)
                toBeAddedPin = wire->endPin();
            if( wire->endPin() == currentPin)
                toBeAddedPin = wire->startPin();
            if( !toBeAddedPin ){
                kError() << "BUG: found inconsistency in data structures: "
                    << "a wire marked connected to a pin doesn't have that pin as any endpoint\n";
                    return;
            }
            if( !takenPins.contains(toBeAddedPin) ){
                // add the other pin
                takenPins.insert(toBeAddedPin);
                toBeInspected.append(toBeAddedPin);
                m_pins.append(toBeAddedPin);
                m_wires.append(wire);
            }
        }
    }
    // some debug
    kDebug() << "pin group created with " << m_pins.size() << " pins and " <<
                m_wires.size() << " wires \n";
}

KTechLab::PinGroup::~PinGroup()
{
    m_wires.clear();
    m_pins.clear();
}

bool KTechLab::PinGroup::containsPin(IPin* pin) const
{
    return m_pins.contains(pin);
}

const QList< IPin* > PinGroup::pins() const
{
    return m_pins;
}


bool KTechLab::PinGroup::containsWire(IWire* wire) const
{
    return m_wires.contains(wire);
}

void KTechLab::PinGroup::setVoltage(double voltage)
{
    foreach(IPin *pin, m_pins){
        pin->setVoltage(voltage);
    }
}

void KTechLab::PinGroup::calculateInternalCurrents()
{
 // TODO implement
}
