/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecvoltagesource.h"

#include "voltagesource.h"

#include <QDebug>

ECCell::ECCell() : Component()
{
    Property *v = new Property("voltage", Variant::Type::Double);
    v->setUnit("V");
    v->setCaption(tr("Voltage"));
    v->setMinValue(-1e12);
    v->setMaxValue(1e12);
    v->setValue(5.0);
    addProperty(v);
}

ECCell::~ECCell() {
}

void ECCell::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    if(theProperty.name() != "voltage"){
        qCritical() << "property changed different than voltage?!";
        return;
    }
    Q_UNUSED(oldValue);

    double voltage = newValue.asDouble();
    m_voltageSource.setVoltage(voltage);
}
