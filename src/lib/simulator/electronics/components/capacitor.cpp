/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "capacitor.h"

#include "circuit.h"
#include "capacitance.h"
#include "ecnode.h"
#include "elementmap.h"
#include "simulator.h"

#include <QDebug>

Capacitor::Capacitor(Circuit &ownerCircuit) : Component(ownerCircuit)
{
    // model
    m_capacitance = new Capacitance(0.001, LINEAR_UPDATE_PERIOD);
    ElementMap *m_elemMap = new ElementMap(m_capacitance);
    m_elementMapList.append(m_elemMap);

    // pins
    m_pinMap.insert("n1", new ECNode(ownerCircuit, m_elemMap->pin(0)));
    m_pinMap.insert("p1", new ECNode(ownerCircuit, m_elemMap->pin(1)));

    Property *cap = new Property("Capacitance", Variant::Type::Double);
    cap->setCaption(tr("Capacitance"));
    cap->setUnit("F");
    cap->setMinValue(1e-12);
    cap->setMaxValue(1e12);
    cap->setValue(1e-3);
    addProperty(cap);

    ownerCircuit.addComponent(this);
}

Capacitor::~Capacitor() {
    circuit().removeComponent(this);
}

void Capacitor::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    if(theProperty.name() != "Capacitance"){
        qCritical() << "capacitor has different property than capacitance?"
            << "and that one also changes?";
        return;
    }
    Q_UNUSED(oldValue);
    double capacitance = newValue.toDouble();
    m_capacitance->setCapacitance(capacitance);
    // reset the charge on the capacitance
    m_capacitance->add_initial_dc();
}
