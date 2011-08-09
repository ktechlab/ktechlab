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
#include "simulatorexport.h"

/**
@short Represents a non-linear circuit element (such as a diode)
@author David Saxton
*/
class SIMULATOR_EXPORT NonLinear : public Element
{
    public:
        NonLinear();
        
        virtual bool isNonLinear() const { return true; }
        /**
        * Newton-Raphson iteration: Update equation system.
        */
        virtual void update_dc() = 0;
        
    protected:
        /**
        * The diode current.
        */
        double diodeCurrent( double v, double I_S, double N ) const;
        /**
        * Conductance of the diode - the derivative of Schockley's
        * approximation.
        */
        double diodeConductance( double v, double I_S, double N ) const;
        /**
        * Limits the diode voltage to prevent divergence in the nonlinear
        * iterations.
        */
        double diodeVoltage( double v, double V_prev, double N, double V_lim ) const;
        /**
        * Current and conductance for a diode junction.
        */
        void diodeJunction( double v, double I_S, double N, double * I, double * g ) const;
        /**
        * Current and conductance for a MOS diode junction.
        */
        void mosDiodeJunction( double V, double I_S, double N, double * I, double * g ) const;
        /**
        * Limits the drain-source voltage to prevent divergence in the
        * nonlinear iterations.
        */
        double fetVoltageDS( double V, double V_prev ) const;
        /**
        * Limits the forward voltage to prevent divergence in the nonlinear
        * iterations.
        */
        double fetVoltage( double V, double V_prev, double Vth ) const;
        
        double diodeLimitedVoltage( double I_S, double N ) const;
};

#endif