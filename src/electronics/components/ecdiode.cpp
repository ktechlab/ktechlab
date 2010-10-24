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



ECDiode::ECDiode()
{
    /*
	DiodeSettings ds; // it will have the default properties that we use

	createProperty("I_S", Variant::Type::Double);
	property("I_S")->setCaption("Saturation Current");
	property("I_S")->setUnit("A");
	property("I_S")->setMinValue(1e-20);
	property("I_S")->setMaxValue(1e-0);
	property("I_S")->setValue(ds.I_S);
	property("I_S")->setAdvanced(true);

	createProperty("N", Variant::Type::Double);
	property("N")->setCaption(i18n("Emission Coefficient"));
	property("N")->setMinValue(1.0);
	property("N")->setMaxValue(1e1);
	property("N")->setValue(ds.N);
	property("N")->setAdvanced(true);

	createProperty("V_B", Variant::Type::Double);
	property("V_B")->setCaption(i18n("Breakdown Voltage"));
	property("V_B")->setUnit("V");
	property("V_B")->setMinAbsValue(1e-5);
	property("V_B")->setMaxValue(1e10);
	property("V_B")->setValue(ds.V_B);
	property("V_B")->setAdvanced(true);
    */

// 	createProperty( "R", Variant::Type::Double );
// 	property("R")->setCaption( i18n("Series Resistance") );
// 	property("R")->setUnit( QChar(0x3a9) );
// 	property("R")->setMinValue(1e-5);
// 	property("R")->setMaxValue(1e0);
// 	property("R")->setValue( ds.R );
// 	property("R")->setAdvanced(true);
}

ECDiode::~ECDiode() {}


void ECDiode::setSaturationCurrent(double I_S)
{
    DiodeSettings ds = m_diode.settings();
    ds.I_S = I_S;
    m_diode.setDiodeSettings(ds);
}

void ECDiode::setEmissionCoefficient(double N)
{
    DiodeSettings ds = m_diode.settings();
    ds.N = N;
    m_diode.setDiodeSettings(ds);
}

void ECDiode::setBreakdownVoltage(double V_B)
{
    DiodeSettings ds = m_diode.settings();
    ds.V_B = V_B;
    m_diode.setDiodeSettings(ds);
}

double ECDiode::saturationCurrent() const
{
    return m_diode.settings().I_S;
}

double ECDiode::emissionCoefficient() const
{
    return m_diode.settings().N;
}

double ECDiode::breakdownVoltage() const
{
    return m_diode.settings().V_B;
}
