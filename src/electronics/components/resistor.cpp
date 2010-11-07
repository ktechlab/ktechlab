/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "resistor.h"

#include "variant.h"

Resistor::Resistor()
		: Component() {
    Property * r = new Property("resistance", Variant::Type::Double);
	r->setCaption(tr("Resistance"));
	r->setUnit(QChar(0x3a9));
	r->setValue(1e4);
	r->setMinValue(1e-6);
    addProperty(r);

}

Resistor::~Resistor() {
}
void Resistor::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_ASSERT(theProperty.name() == "resistance");
    Q_ASSERT(newValue.asDouble() > 0);

    Q_UNUSED(oldValue);

    m_resistance.setResistance(newValue.asDouble());
}
