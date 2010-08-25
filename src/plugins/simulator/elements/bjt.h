/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef BJT_H
#define BJT_H

#include "nonlinear.h"

class BJTState {

public:
	BJTState();
	void reset();

	BJTState operator-(const BJTState & s) const;

	double A[3][3];
	double I[3];
};


class BJTSettings {

public:
	BJTSettings();

	double I_S; ///< saturation current
	double N_F; ///< forward emission coefficient
	double N_R; ///< reverse emission coefficient
	double B_F; ///< forward beta
	double B_R; ///< reverse beta
};

/**
@author David Saxton
*/
class BJT : public NonLinear {

public:
	BJT(bool isNPN);
	virtual ~BJT();

	virtual Type type() const {
		return Element_BJT;
	}

	virtual void update_dc();
	virtual void add_initial_dc();
	BJTSettings settings() const {
		return m_bjtSettings;
	}

	void setBJTSettings(const BJTSettings & settings);

protected:
	virtual void updateCurrents();
	/**
	 * Calculates the new BJTState from the voltages on the nodes.
	 */
	void calc_eq();
	void calcIg(double V_BE, double V_BC,
	            double *I_BE, double *I_BC,
	            double *I_T,
	            double *g_BE, double *g_BC,
	            double *g_IF, double *g_IR) const;
	void updateLim();

	BJTState m_os;
	BJTState m_ns;
	int m_pol;
	double V_BE_prev, V_BC_prev;
	double V_BE_lim, V_BC_lim;
	double I_BE, I_BC, I_T; // so we don't have to recompute for current reads.

	BJTSettings m_bjtSettings;
};

#endif
