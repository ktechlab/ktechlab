/**************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "nonlinear.h"

#include <cmath>
using namespace std;

NonLinear::NonLinear()
	: Element()
{
}

// the simulator will break if you fail to clamp your voltage with this function.
double NonLinear::diodeVoltage(double V, double V_prev, double N, double V_lim) const
{
	double Vt = V_T * N;

	if(V > V_lim && fabs(V - V_prev) > 2 * Vt) {
		if(V_prev > 0) {
			double arg = (V - V_prev) / Vt;
			if(arg > 0) return V_prev + Vt * (2 + log(arg - 2));
			else	    return V_prev - Vt * (2 + log(2 - arg));
		} else		    return (V_prev < 0) ? (Vt * log (V / Vt)) : V_lim;
	} else {
		if(V < 0) {
			double arg = (V_prev > 0) ? (-1 - V_prev) : (2 * V_prev - 1);
			if (V < arg)
				return arg;
		}
	}
	return V;
}

double NonLinear::fetVoltageDS(double V, double V_prev) const
{
	if(V_prev >= 3.5) {
		if(V > V_prev) return std::min(V, 3 * V_prev + 2);
		else if(V < 3.5) return std::max(V, 2.5); // I think this is the pinch-off voltage.

		return V;
	}
	
	if(V > V_prev) return std::min(V, 4.0);
	
	return std::max(V, -0.5);
}

// obtain the diode limit voltage.
double NonLinear::diodeLimitedVoltage(double I_S, double N) const
{
	double Vt = V_T * N;
	return Vt * log(Vt / M_SQRT2 / I_S);
}

/// combined function for current and conductance.
void NonLinear::diodeJunction(double V, double I_S, double N, double *I, double *g) const
{
	double Vt = V_T * N;	

	if (V < -3 * Vt) {
		double a = 3 * Vt / (V * M_E);
		a  = a * a * a;
		*I = -I_S * (1 + a);
		*g =  I_S * 3 * a / V;
	} else {
		double e = exp(V * ELEMENTARY_CHARGE / (N * E_T));
		*I = I_S * (e - 1);
		*g = I_S * ELEMENTARY_CHARGE / (N * E_T) * e;
	}
}

double NonLinear::fetVoltage(double V, double V_prev, double Vth) const
{
	double V_tst_hi = 2 * fabs(V_prev - Vth) + 2.0;
	double V_tst_lo = V_tst_hi / 2;
	double V_tox = Vth + 3.5;
	double delta_V = V - V_prev;

	if(V_prev >= Vth) {
		// on
		if(V_prev >= V_tox) {
			if(delta_V <= 0) {
				// going off
				if(V >= V_tox) {
					if(-delta_V > V_tst_lo)
						return V_prev - V_tst_lo;

					return V;
				}

				return std::max(V, Vth + 2);
			}

			// staying on
			if(delta_V >= V_tst_hi)
				return V_prev + V_tst_hi;

			return V;
		}

		// middle region
		if(delta_V <= 0) {
			// decreasing
			return std::max(V, Vth - 0.5);
		}

		// increasing
		return std::min(V, Vth + 4);
	}

	//  off
	if(delta_V <= 0) {
		// staying off
		if(-delta_V > V_tst_hi)
			return V_prev - V_tst_hi;

		return V;
	}

	// going on
	if(V <= Vth + 0.5) {
		if(delta_V > V_tst_lo)
			return V_prev + V_tst_lo;

		return V;
	}

	return Vth + 0.5;
}

