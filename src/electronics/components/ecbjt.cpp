/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecbjt.h"

#include "variant.h"


ECBJT::ECBJT(bool isNPN)
		: Component(),
            m_bIsNPN(isNPN), m_pBJT(m_bIsNPN) {


	BJTSettings s; // will be created with the default settings

    Variant * v = 0;

	v = new Variant("I_S", Variant::Type::Double);
	v->setCaption(tr("Saturation Current"));
	v->setUnit("A");
	v->setMinValue(1e-20);
	v->setMaxValue(1e-0);
	v->setValue(s.I_S);
	v->setAdvanced(true);
    addProperty(v);

	v = new Variant("N_F", Variant::Type::Double);
	v->setCaption(tr("Forward Coefficient"));
	v->setMinValue(1e0);
	v->setMaxValue(1e1);
	v->setValue(s.N_F);
	v->setAdvanced(true);
    addProperty(v);

	v = new Variant("N_R", Variant::Type::Double);
	v->setCaption(tr("Reverse Coefficient"));
	v->setMinValue(1e0);
	v->setMaxValue(1e1);
	v->setValue(s.N_R);
	v->setAdvanced(true);
    addProperty(v);

	v = new Variant("B_F", Variant::Type::Double);
	v->setCaption(tr("Forward Beta"));
	v->setMinValue(1e-1);
	v->setMaxValue(1e3);
	v->setValue(s.B_F);
	v->setAdvanced(true);
    addProperty(v);

	v = new Variant("B_R", Variant::Type::Double);
	v->setCaption(tr("Reverse Beta"));
	v->setMinValue(1e-1);
	v->setMaxValue(1e3);
	v->setValue(s.B_R);
	v->setAdvanced(true);
    addProperty(v);
}

ECBJT::~ECBJT() {}

void ECBJT::propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue)
{
    Q_UNUSED(theProperty);
    Q_UNUSED(newValue);
    Q_UNUSED(oldValue);

    BJTSettings s;
    s.I_S = property("I_S").asDouble();
    s.N_F = property("N_F").asDouble();
    s.N_R = property("N_R").asDouble();
    s.B_F = property("B_F").asDouble();
    s.B_R = property("B_R").asDouble();

    m_pBJT.setBJTSettings(s);
}
