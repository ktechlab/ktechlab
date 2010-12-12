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

#include "ecnode.h"
#include "variant.h"
#include <elementmap.h>
#include <resistance.h>

Resistor::Resistor()
		: Component() {
    m_resistance = new Resistance();
    m_elemMap = new ElementMap(m_resistance);
    m_elementMapList.append(m_elemMap);

    // these will be exernal pins
    m_pinMap.insert("n1", new ECNode(m_elemMap->pin(0)));
    m_pinMap.insert("p1", new ECNode(m_elemMap->pin(1)));

    Property * r = new Property("resistance", Variant::Type::Double);
	r->setCaption(tr("Resistance"));
	r->setUnit(QChar(0x3a9));
	r->setValue(1e4);
	r->setMinValue(1e-6);
    addProperty(r);

}

Resistor::~Resistor() {
    // TODO move pin + elementmap destructors into component's destructor
    ECNode *n1 = m_pinMap.value("n1");
    ECNode *p1 = m_pinMap.value("p1");
    m_pinMap.clear();
    delete m_elemMap;
    delete n1;
    delete p1;
    delete m_resistance;
}

void Resistor::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_ASSERT(theProperty.name() == "resistance");
    Q_ASSERT(newValue.asDouble() > 0);

    Q_UNUSED(oldValue);

    m_resistance->setResistance(newValue.asDouble());
}
