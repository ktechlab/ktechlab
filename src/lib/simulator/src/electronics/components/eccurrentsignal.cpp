/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "eccurrentsignal.h"

#include <circuit.h>
#include "currentsignal.h"
#include "ecnode.h"
#include "elementmap.h"
#include "pin.h"
#include "simulator.h"
#include "variant.h"

#include <QDebug>

ECCurrentSignal::ECCurrentSignal(Circuit& ownerCircuit)
	: Component(ownerCircuit)
{
    m_currentSignal = new CurrentSignal(LINEAR_UPDATE_PERIOD, 0.02);
    m_currentSignal->setStep(ElementSignal::st_sinusoidal, 50.);

    ElementMap *map = new ElementMap(m_currentSignal);
    m_elementMapList.append(map);

    m_pinMap.insert("n1", new ECNode(ownerCircuit, map->pin(0)));
    m_pinMap.insert("p1", new ECNode(ownerCircuit, map->pin(1)));

    map->pin(1)->setGroundType(Pin::gt_low);

    Property *freq = new Property("1-frequency", Variant::Type::Double);
	freq->setCaption(tr("Frequency"));
	freq->setUnit("Hz");
	freq->setMinValue(1e-9);
	freq->setMaxValue(1e3);
	freq->setValue(50.0);
    addProperty(freq);

	Property *current = new Property( "1-current", Variant::Type::Double );
	current->setCaption(tr("Current Range"));
	current->setUnit("A");
	current->setMinValue(-1e12);
	current->setMaxValue(1e12);
	current->setValue(0.02);
    addProperty(current);

    ownerCircuit.addComponent(this);
}


ECCurrentSignal::~ECCurrentSignal()
{
    circuit().removeComponent(this);
}

void ECCurrentSignal::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(oldValue);

    if(theProperty.name() == "1-current"){
        double frequency = newValue.toDouble();
        m_currentSignal->setStep(ElementSignal::st_sinusoidal, frequency );
    } else
    if(theProperty.name() == "1-frequency"){
        double current = newValue.toDouble();
        m_currentSignal->setCurrent(current);
    } else {
        qCritical() << "ECCurrentSignal: inexistent property with the name "
            << theProperty.name() << " changed";
    }
}
