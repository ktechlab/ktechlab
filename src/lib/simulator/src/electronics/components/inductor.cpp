/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "inductor.h"

#include "circuit.h"
#include "ecnode.h"
#include "elementmap.h"
#include "inductance.h"
#include "simulator.h"

#include <QDebug>

Inductor::Inductor(Circuit& ownerCircuit)
		: Component(ownerCircuit)
    {
    m_pInductance = new Inductance(0.001, LINEAR_UPDATE_PERIOD);

    ElementMap *m_elemMap = new ElementMap(m_pInductance);
    m_elementMapList.append(m_elemMap);

    // pins
    m_pinMap.insert("n1", new ECNode(ownerCircuit, m_elemMap->pin(0)));
    m_pinMap.insert("p1", new ECNode(ownerCircuit, m_elemMap->pin(1)));

    Property *cap = new Property("Inductance", Variant::Type::Double);
	cap->setCaption(tr("Inductance"));
	cap->setUnit("H");
	cap->setMinValue(1e-12);
	cap->setMaxValue(1e12);
	cap->setValue(1e-3);
    addProperty(cap);

    ownerCircuit.addComponent(this);
}

Inductor::~Inductor() {
    circuit().removeComponent(this);
}


void Inductor::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    if(theProperty.name() != "Inductance"){
        qCritical() << "inductor has different property than Inductance?"
        << "and that one also changes?";
        return;
    }
    Q_UNUSED(oldValue);
    double inductance = newValue.asDouble();
    m_pInductance->setInductance(inductance);
    // reset the charge on the capacitance
    m_pInductance->add_initial_dc();
}
