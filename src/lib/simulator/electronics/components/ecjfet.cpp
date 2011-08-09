/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecjfet.h"

#include "variant.h"

#include <QDebug>

ECJFET::ECJFET(Circuit& ownerCircuit, int JFET_type)
		:   Component(ownerCircuit),
            m_pJFET((JFET::JFET_type) JFET_type) {
	m_JFET_type = JFET_type;

	JFETSettings s; // will be created with the default settings

	m_pJFET.setJFETSettings(s);

	Variant *v = new Property("V_Th", Variant::Type::Double);
	v->setCaption(tr("Threshold voltage"));
	v->setUnit("V");
	v->setMinValue(-1e6);
	v->setMaxValue(1e6);
	v->setValue(s.V_Th);
	v->setAdvanced(true);
    addProperty(v);

	Variant *beta = new Property("beta", Variant::Type::Double);
	beta->setCaption(tr("Transcondutance"));
	beta->setUnit(QString("A/V") + QChar(0xb2));
	beta->setMinValue(1e-12);
	beta->setMaxValue(1e0);
	beta->setValue(s.beta);
	beta->setAdvanced(true);
    addProperty(beta);

	Variant *i_s = new Property("I_S", Variant::Type::Double);
	i_s->setCaption(tr("Saturation current"));
	i_s->setUnit("A");
	i_s->setMinValue(1e-20);
	i_s->setMaxValue(1e0);
	i_s->setValue(s.I_S);
	i_s->setAdvanced(true);
    addProperty(i_s);

	Variant *n = new Property("N", Variant::Type::Double);
	n->setCaption(tr("PN emission coefficient"));
	n->setUnit("");
	n->setMinValue(0.0);
	n->setMaxValue(10.0);
	n->setValue(s.N);
	n->setAdvanced(true);
    addProperty(n);

	Variant *n_r = new Property("N_R", Variant::Type::Double);
	n_r->setCaption(tr("Isr emission coefficient"));
	n_r->setUnit("");
	n_r->setMinValue(0.0);
	n_r->setMaxValue(10.0);
	n_r->setValue(s.N_R);
	n_r->setAdvanced(true);
    addProperty(n_r);
}

ECJFET::~ECJFET() {}

void ECJFET::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(oldValue);

    JFETSettings s = m_pJFET.settings();
    if( theProperty.name() == "V_Th" ){
        s.V_Th = newValue.toDouble();
    } else if( theProperty.name() == "beta"){
        s.beta = newValue.toDouble();
    } else if( theProperty.name() == "I_S"){
        s.I_S = newValue.toDouble();
    } else if( theProperty.name() == "N"){
        s.N = newValue.toDouble();
    } else if( theProperty.name() == "N_R"){
        s.N_R = newValue.toDouble();
    } else
        qCritical() << "ECJFET: unknown property: " << theProperty.name();

    m_pJFET.setJFETSettings(s);
}
