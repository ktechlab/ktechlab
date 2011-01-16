/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// #include <klocale.h>
// #include <qpainter.h>

#include "simulator.h"
#include "capacitor.h"
// #include "ecnode.h"
// #include "libraryitem.h"

#include <QDebug>

Capacitor::Capacitor(Circuit &ownerCircuit) : Component(ownerCircuit),
		m_capacitance(0.001, LINEAR_UPDATE_PERIOD)
{
    Property *cap = new Property("Capacitance", Variant::Type::Double);
    cap->setCaption(tr("Capacitance"));
    cap->setUnit("F");
    cap->setMinValue(1e-12);
    cap->setMaxValue(1e12);
    cap->setValue(1e-3);
    addProperty(cap);
        /*
	createProperty("Capacitance", Variant::Type::Double);
	property("Capacitance")->setCaption(i18n("Capacitance"));
	property("Capacitance")->setUnit("F");
	property("Capacitance")->setMinValue(1e-12);
	property("Capacitance")->setMaxValue(1e12);
	property("Capacitance")->setValue(1e-3);

	addDisplayText("capacitance", QRect(-8, -24, 16, 16), "", false);
    */
}

Capacitor::~Capacitor() {}

void Capacitor::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    if(theProperty.name() != "Capacitance"){
        qCritical() << "capacitor has different property than capacitance?"
            << "and that one also changes?";
        return;
    }
    Q_UNUSED(oldValue);
    double capacitance = newValue.asDouble();
    setCapacitance(capacitance);
}

/*
void Capacitor::dataChanged() {
	double capacitance = dataDouble("Capacitance");

	QString display = QString::number(capacitance / getMultiplier(capacitance), 'g', 3) + getNumberMag(capacitance) + "F";
	setDisplayText("capacitance", display);

	m_capacitance.setCapacitance(capacitance);
}
*/

double Capacitor::capacitance() const
{
    return m_capacitance.capacitance();
}

void Capacitor::setCapacitance(double capacitance)
{
    if( capacitance < 0){
        qCritical() << "capacitance value cannot be negative!";
        capacitance = 1e-12;
    }
    m_capacitance.setCapacitance(capacitance);
}
