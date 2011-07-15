/***************************************************************************
 *   Copyright (C) 2003,2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecdiode.h"

#include "circuit.h"
#include "diode.h"
#include "ecnode.h"
#include "elementmap.h"
#include "variant.h"

#include <QDebug>

ECDiode::ECDiode(Circuit& ownerCircuit) :
            Component(ownerCircuit)
{
    m_diode = new Diode();

    ElementMap *map = new ElementMap(m_diode);
    m_elementMapList.append(map);

    m_pinMap.insert("n1", new ECNode(ownerCircuit, map->pin(0)));
    m_pinMap.insert("p1", new ECNode(ownerCircuit, map->pin(1)));

	DiodeSettings ds; // it will have the default properties that we use

	Property * i_s =  new Property("I_S", Variant::Type::Double);
	i_s->setCaption(tr("Saturation Current"));
	i_s->setUnit("A");
	i_s->setMinValue(1e-20);
	i_s->setMaxValue(1e-0);
	i_s->setValue(ds.I_S);
	i_s->setAdvanced(true);
    addProperty(i_s);

    Property *n = new Property("N", Variant::Type::Double);
	n->setCaption(tr("Emission Coefficient"));
	n->setMinValue(1.0);
	n->setMaxValue(1e1);
	n->setValue(ds.N);
	n->setAdvanced(true);
    addProperty(n);

    Property *v_b = new Property("V_B", Variant::Type::Double);
	v_b->setCaption(tr("Breakdown Voltage"));
	v_b->setUnit("V");
	v_b->setMinAbsValue(1e-5);
	v_b->setMaxValue(1e10);
	v_b->setValue(ds.V_B);
	v_b->setAdvanced(true);
    addProperty(v_b);

// 	createProperty( "R", Variant::Type::Double );
// 	property("R")->setCaption( i18n("Series Resistance") );
// 	property("R")->setUnit( QChar(0x3a9) );
// 	property("R")->setMinValue(1e-5);
// 	property("R")->setMaxValue(1e0);
// 	property("R")->setValue( ds.R );
// 	property("R")->setAdvanced(true);

    ownerCircuit.addComponent(this);
}

ECDiode::~ECDiode() {
    circuit().removeComponent(this);
}

void ECDiode::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(oldValue);

    DiodeSettings ds = m_diode->settings();
    if(theProperty.name() == "I_S"){
        ds.I_S = newValue.toDouble();
    } else if(theProperty.name() == "N"){
        ds.N = newValue.toDouble();
    } else if(theProperty.name() == "V_B"){
        ds.V_B = newValue.toDouble();
    } else
        qCritical() << "ECDiode: uknown property has changed: " << theProperty.name();
    m_diode->setDiodeSettings(ds);
}
