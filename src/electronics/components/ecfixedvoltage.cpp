/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecfixedvoltage.h"

#include "circuit.h"
#include "property.h"
#include <voltagepoint.h>
#include <elementmap.h>
#include <ecnode.h>

#include <QDebug>

ECFixedVoltage::ECFixedVoltage(Circuit &ownerCircuit)
        : Component(ownerCircuit) {
    m_voltagePoint = new VoltagePoint();

    m_map = new ElementMap(m_voltagePoint);
    m_elementMapList.append(m_map);

    m_pinMap.insert("p1", new ECNode(ownerCircuit, m_map->pin(0)));

    Property *voltage = new Property("voltage", Variant::Type::Double);
    voltage->setUnit("V");
    voltage->setCaption(tr("Voltage"));
    voltage->setMinValue(-1e15);
    voltage->setMaxValue(1e15);
    voltage->setValue(5.0);
    addProperty(voltage);

    ownerCircuit.addComponent(*this);
}

ECFixedVoltage::~ECFixedVoltage() {
    circuit().removeComponent(*this);
}

void ECFixedVoltage::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(oldValue);

    if(theProperty.name() == "voltage"){
        double voltage = newValue.asDouble();
        m_voltagePoint->setVoltage(voltage);
    } else
        qCritical() << "ECFixedVoltage: unknown property: " << theProperty.name();
}
