/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *									   *
 *   Peak/RMS added (c)19/06/2007 by Jason Lucas			   *
 ***************************************************************************/

#include "ecvoltagesignal.h"

#include "circuit.h"
#include "ecnode.h"
#include "elementmap.h"
#include "simulator.h"
#include "variant.h"
#include "voltagesignal.h"

#include <QDebug>

ECVoltageSignal::ECVoltageSignal(Circuit& ownerCircuit)
	: Component(ownerCircuit)

{
    m_voltageSignal = new VoltageSignal(LINEAR_UPDATE_PERIOD, 5);
	m_voltageSignal->setStep(ElementSignal::st_sinusoidal, 50.);

    ElementMap *m_elemMap = new ElementMap(m_voltageSignal);
    m_elementMapList.append(m_elemMap);

    // these pins might become ground
    m_elemMap->pin(0)->setGroundType(Pin::gt_medium);
    m_elemMap->pin(1)->setGroundType(Pin::gt_medium);

    // pins
    m_pinMap.insert("n1", new ECNode(ownerCircuit, m_elemMap->pin(0)));
    m_pinMap.insert("p1", new ECNode(ownerCircuit, m_elemMap->pin(1)));

    Property *freq = new Property( "frequency", Variant::Type::Double );
	freq->setCaption( tr("Frequency") );
	freq->setUnit("Hz");
	freq->setMinValue(1e-9);
	freq->setMaxValue(1e3);
	freq->setValue(50.0);
    addProperty(freq);

	Property *voltage = new Property("voltage", Variant::Type::Double);
	voltage->setCaption( tr("Voltage Range"));
	voltage->setUnit("V");
	voltage->setMinValue(-1e12);
	voltage->setMaxValue(1e12);
	voltage->setValue(5.0);
    addProperty(voltage);


	Property * peak_rms = new Property("peak-rms", Variant::Type::Select);
	peak_rms->setCaption(tr("Output"));
	QStringMap allowed;
	allowed["Peak"] = tr("Peak");
	allowed["RMS"] = tr("RMS");
	peak_rms->setAllowed(allowed);
	peak_rms->setValue("Peak");
    addProperty(peak_rms);

    ownerCircuit.addComponent(this);
}

ECVoltageSignal::~ECVoltageSignal() {
    circuit().removeComponent(this);
}

void ECVoltageSignal::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(oldValue);
    if(theProperty.name() == "voltage"){
        const double voltage = newValue.toDouble();
        m_voltageSignal->setVoltage(voltage);
    } else if(theProperty.name() == "frequency"){
        m_voltageSignal->setStep(ElementSignal::st_sinusoidal, newValue.toDouble() );
    } else if(theProperty.name() == "peak-rms"){
        const double voltage = m_voltageSignal->voltage();
        if((newValue.toString() == "Peak") && (oldValue.toString() == "RMS")){
            // RMS -> peak
            m_voltageSignal->setVoltage( voltage * M_SQRT1_2 );
        } else
            if((newValue.toString() == "RMS") && (oldValue.toString() == "Peak")){
                // peak -> RMS
                m_voltageSignal->setVoltage(voltage * M_SQRT2);
            } else
                qCritical() << "ECVoltageSignal: unknown configuration of Peak/RSM: old: "
                    << oldValue.toString() << " ; new: " << newValue.toString();
    } else
        qCritical() << "ECVoltageSignal: unknown property: " << theProperty.name();
}
