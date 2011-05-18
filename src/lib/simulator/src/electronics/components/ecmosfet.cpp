/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecmosfet.h"

#include "variant.h"

#include <QDebug>


ECMOSFET::ECMOSFET(Circuit& ownerCircuit, int MOSFET_type)
		:   Component(ownerCircuit),
            m_pMOSFET((MOSFET::MOSFET_type)MOSFET_type) {

	m_MOSFET_type = MOSFET_type;

	m_bHaveBodyPin = false;

	Variant *v = new Property("bodyPin", Variant::Type::Bool);
	v->setCaption(tr("mosfet body/bulk pin", "Body Pin"));
	v->setValue(false);
    addProperty(v);

	// create a "test" MOSFETSettings, for init. values
	MOSFETSettings s;
	
	// add the properties of MOSFET
	Variant *v1 = new Property("mosfetW", Variant::Type::Double);
	v1->setCaption(tr("mosfet channel width", "Channel width"));
	v1->setValue( s.W );
	v1->setAdvanced(true);
	v1->setUnit( "m" );
    addProperty(v1);
	
	Variant *v2 = new Property("mosfetL", Variant::Type::Double);
	v2->setCaption(tr("mosfet channel length", "Channel length"));
	v2->setValue( s.L );
	v2->setAdvanced(true);
	v2->setUnit( "m" );
    addProperty(v2);

	
#if 0
    // TODO add other properties to the MOSTFET
	MOSFETSettings s; // will be created with the default settings
	v = createProperty("I_S", Variant::Type::Double);
	v->setCaption(i18n("Saturation Current"));
	v->setUnit("A");
	v->setMinValue(1e-20);
	v->setMaxValue(1e-0);
	v->setValue(s.I_S);
	v->setAdvanced(true);

	v = createProperty("N_F", Variant::Type::Double);
	v->setCaption(i18n("Forward Coefficient"));
	v->setMinValue(1e0);
	v->setMaxValue(1e1);
	v->setValue(s.N_F);
	v->setAdvanced(true);

	v = createProperty("N_R", Variant::Type::Double);
	v->setCaption(i18n("Reverse Coefficient"));
	v->setMinValue(1e0);
	v->setMaxValue(1e1);
	v->setValue(s.N_R);
	v->setAdvanced(true);

	v = createProperty("B_F", Variant::Type::Double);
	v->setCaption(i18n("Forward Beta"));
	v->setMinValue(1e-1);
	v->setMaxValue(1e3);
	v->setValue(s.B_F);
	v->setAdvanced(true);

	v = createProperty("B_R", Variant::Type::Double);
	v->setCaption(i18n("Reverse Beta"));
	v->setMinValue(1e-1);
	v->setMaxValue(1e3);
	v->setValue(s.B_R);
	v->setAdvanced(true);
#endif
}

ECMOSFET::~ECMOSFET() {}

void ECMOSFET::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(oldValue);

    if( theProperty.name() == "bodyPin"){
        bool haveBodyPin = newValue.asBool();
        if (haveBodyPin == m_bHaveBodyPin)
            return;

        // body pin state changed
        if (m_bHaveBodyPin) {
            // TODO Creating a body pin
//             removeElement(&m_pMOSFET, false);
// 
//             setup4pinElement(m_pMOSFET, ecNodeWithID("d")->pin(),
//                 ecNodeWithID("g")->pin(), ecNodeWithID("s")->pin(),
//                 createPin(16, 0, 180, "b")->pin());

        } else {
            // TODO Removing a body pin
            // removeNode("b");
            // removeElement(&m_pMOSFET, false);

        }
    }

    MOSFETSettings s = m_pMOSFET.settings();
#if 0
    s.I_S = dataDouble("I_S");
    s.N_F = dataDouble("N_F");
    s.N_R = dataDouble("N_R");
    s.B_F = dataDouble("B_F");
    s.B_R = dataDouble("B_R");
#endif
    if( theProperty.name() == "mosfetL") {
        s.L = newValue.asDouble();
    } else if(theProperty.name() == "mosfetW"){
        s.W = newValue.asDouble();
    } else
        qCritical() << "ECMOSFET: unknown property: " << theProperty.name();

    m_pMOSFET.setMOSFETSettings(s);
}
