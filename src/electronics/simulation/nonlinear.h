/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef NONLINEAR_H
#define NONLINEAR_H

#include "element.h"

/**
@short Represents a non-linear circuit element (such as a diode)
@author David Saxton
*/
class NonLinear : public Element
{
	public:
		NonLinear();
	
		virtual bool isNonLinear() { return true; }
		/**
		 * Newton-Raphson iteration: Update equation system.
		 */
		virtual void update_dc() = 0;
		
	protected:
		double diodeCurrent( double v, double I_S, double Vte ) const;
		/**
		 * Conductance of the diode - the derivative of Schockley's
		 * approximation.
		 */
		double diodeConductance( double v, double I_S, double Vte ) const;
		/**
		 * Limits the diode voltage to prevent divergence in the nonlinear
		 * iterations.
		 */
		double diodeVoltage( double v, double V_prev, double Vt, double V_crit ) const;
		void diodeJunction( double v, double I_S, double Vte, double * I, double * g ) const;
		
		double diodeCriticalVoltage( double I_S, double Vte ) const;
};

#endif
