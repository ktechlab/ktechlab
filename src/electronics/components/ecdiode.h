/***************************************************************************
 *   Copyright (C) 2003,2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECDIODE_H
#define ECDIODE_H

#include "component.h"
#include "diode.h"

/**
@short Simple diode
@author David Saxton
*/
class ECDiode : public Component
{
public:
	ECDiode();
	~ECDiode();

    void setSaturationCurrent(double I_S); // I_S
    void setEmissionCoefficient(double N); // N
    void setBreakdownVoltage(double V_B); // V_B

    double saturationCurrent() const;
    double emissionCoefficient() const;
    double breakdownVoltage() const;
	
protected:
	Diode m_diode;
};

#endif
