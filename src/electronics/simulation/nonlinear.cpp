/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "matrix.h"
#include "nonlinear.h"

#include <cmath>
using namespace std;

const double KTL_MAX_DOUBLE = 1.7976931348623157e+308; ///< 7fefffff ffffffff
const int KTL_MAX_EXPONENT = int( log( KTL_MAX_DOUBLE ) );

NonLinear::NonLinear()
	: Element()
{
}

#ifndef MIN
# define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

// The function computes the exponential pn-junction current.
double NonLinear::diodeCurrent( double v, double I_S, double Vte ) const
{
	return I_S * (exp( MIN( v / Vte, KTL_MAX_EXPONENT ) ) - 1);
}

double NonLinear::diodeConductance( double v, double I_S, double Vte ) const
{
	return I_S * exp( MIN( v / Vte, KTL_MAX_EXPONENT ) ) / Vte;
}

double NonLinear::diodeVoltage( double V, double V_prev, double V_T, double Vcrit ) const
{
	if ( V > Vcrit && fabs( V - V_prev ) > 2 * V_T ) {
		if(V_prev > 0) {
			double arg = (V - V_prev) / V_T;
			if(arg > 0) V = V_prev + V_T * (2 + log( arg - 2 ));
			else V = V_prev - V_T * (2 + log( 2 - arg ));
		} else V = (V_prev < 0) ? (V_T * log (V / V_T)) : Vcrit;
	} else {
		if(V < 0) {
			double arg = (V_prev > 0) ? (-1 - V_prev) : (2 * V_prev - 1);
			if (V < arg) V = arg;
		}
	}
	return V;
}

double NonLinear::diodeCriticalVoltage( double I_S, double V_Te ) const
{
	return V_Te * log( V_Te / M_SQRT2 / I_S );
}

void NonLinear::diodeJunction(double V, double I_S, double V_Te, double *I, double *g ) const
{
	if (V < -3 * V_Te) {
		double a = 3 * V_Te / (V * M_E);
		a = a * a * a;
		*I = -I_S * (1 + a);
		*g = +I_S * 3 * a / V;
	} else {
		double e = exp(MIN(V / V_Te, KTL_MAX_EXPONENT));
		*I = I_S * (e - 1);
		*g = I_S * e / V_Te;
	}
}

