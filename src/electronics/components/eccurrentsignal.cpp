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

#include "simulator.h"
#include "variant.h"

ECCurrentSignal::ECCurrentSignal()
	: Component(),
	m_currentSignal(LINEAR_UPDATE_PERIOD, 0)	
{
	// m_pNNode[0]->pin().setGroundType(Pin::gt_low);
	m_currentSignal.setStep(ElementSignal::st_sinusoidal, 50.);

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

}


ECCurrentSignal::~ECCurrentSignal()
{
}

void ECCurrentSignal::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    if(theProperty.name() == "1-current"){
        double frequency = newValue.asDouble();
        m_currentSignal.setStep(ElementSignal::st_sinusoidal, frequency );
    }
    if(theProperty.name() == "1-frequency"){
        double current = newValue.asDouble();
        m_currentSignal.setCurrent(current);
    }
}
