/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DIODE_H
#define DIODE_H

#include "nonlinear.h"

class DiodeSettings {

public:
	DiodeSettings();
	void reset();

	double I_S;	///< Diode saturation current
	double N;	///< Emission coefficient
	double V_B;	///< Reverse breakdown
	double SR;	///< Series resistance
};

/**
This simulates a diode. The simulated diode characteristics are:

@li I_s: Diode saturation current
@li V_T: Thermal voltage = kT/4 = 25mV at 20 C
@li n: Emission coefficient, typically between 1 and 2
@li V_RB: Reverse breakdown (large negative voltage)
@li G_RB: Reverse breakdown conductance
@li R_D: Base resistance of diode

@short Simulates the electrical property of diode-ness
@author David Saxton
*/
class Diode : public NonLinear {

public:
	Diode();
	virtual ~Diode();

	virtual void update_dc();
	virtual void add_initial_dc();
	virtual Element::Type type() const {
		return Element_Diode;
	}

	DiodeSettings settings() const {
		return m_diodeSettings;
	}

	void setDiodeSettings(const DiodeSettings & settings);

    /**
     * Returns the current flowing through the diode
     */
	double current() const { return I_old; }

    virtual void updateCurrents();
protected:
	void calc_eq(double *g_new, double *I_new);
	void calcIg(double V, double *I, double *g) const;
	void updateLim();

	double g_old, I_old;

	DiodeSettings m_diodeSettings;

	double v_prev, v_lim;

};

#endif

