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

using namespace KTechLab;

KTechLab::PinGroup::PinGroup(QList< IPin* > pins, QList< IWire* > wires, IPin* startPin)
{
 // TODO implement
}

KTechLab::PinGroup::~PinGroup()
{
 // TODO implement
}

bool KTechLab::PinGroup::containsPin(IPin* pin) const
{
 // TODO implement
}

const QList< IPin* > PinGroup::pins() const
{
 // TODO implement
}


bool KTechLab::PinGroup::containsWire(IWire* wire) const
{
 // TODO implement
}

void KTechLab::PinGroup::setVoltage(double voltage)
{
 // TODO implement
}

void KTechLab::PinGroup::calculateInternalCurrents()
{
 // TODO implement
}
