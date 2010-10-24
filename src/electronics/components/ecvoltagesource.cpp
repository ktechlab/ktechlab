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

ECCell::ECCell() : Component()
{

    /*
    m_pNNode[0]->pin().setGroundType(Pin::gt_medium);

    createProperty("voltage", Variant::Type::Double);
    property("voltage")->setUnit("V");
    property("voltage")->setCaption(i18n("Voltage"));
    property("voltage")->setMinValue(-1e12);
    property("voltage")->setMaxValue(1e12);
    property("voltage")->setValue(5.0);

    addDisplayText("voltage", QRect(-16, -24, 32, 16), "");
    */
}

ECCell::~ECCell() {
}

/*
void ECCell::dataChanged() {
    const double voltage = dataDouble("voltage");

    m_voltageSource.setVoltage(voltage);
    QString display = QString::number(voltage / getMultiplier(voltage), 'g', 3) + getNumberMag(voltage) + "V";
    setDisplayText("voltage", display);
}
*/