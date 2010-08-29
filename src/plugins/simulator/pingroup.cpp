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

KTechLab::PinGroup::PinGroup(QList< IPin* > pins, QList< IWire* > wires, IPin* startPin)
{
    // build auxiliaty maps
    QMap<IPin*, IWire*> pinToWire;
    foreach(IWire* wire, wires){
        pinToWire.insertMulti(wire->startPin(), wire);
        pinToWire.insertMulti(wire->endPin(), wire);
    }

    QMap<IPin*, IPin*> edges;
    edges.clear();
    foreach(IWire *wire, wires){
        edges.insertMulti(wire->startPin(), wire->endPin());
    }

    m_pins.clear();
    m_wires.clear();

    // this is a breath-first traversing of a graph
    m_pins.append(startPin);
    QLinkedList<IPin*> toBeInspected;
    toBeInspected.clear();
    toBeInspected.append(startPin);
    while( ! toBeInspected.isEmpty() ){
        IPin * currentPin = toBeInspected.takeFirst();
        QList<IPin*> currentAdiacent = edges.values(currentPin);
        foreach(IPin * otherPin, currentAdiacent){
            toBeInspected.append(otherPin);
            m_pins.append(otherPin);
        }
        /* get the wires. because we want all the wires, if any end of a wire is
        inside of the selected pins, the wire should be taken */
        QList<IWire*> linkedWires = pinToWire.values(currentPin);
        foreach(IWire *wire, linkedWires){
            m_wires.append(wire);
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
