/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MOSFET_H
#define MOSFET_H

#include "matrix.h"
#include "nonlinear.h"

class MOSFETState
{
	public:
		MOSFETState();
		void reset();
		
		MOSFETState operator-( const MOSFETState & s ) const;
	
		double A[4][4];
		double I[4];
};


class MOSFETSettings
{
	public:
		MOSFETSettings();
		
		double I_S;		///< bulk junction saturation current
		double N;		///< bulk junction emission coefficient
		double K_P;		///< transconductance coeffecient
		double W;		///< channel width
		double L;		///< channel length
		
		double beta() const { return K_P * W / L; }
		
#if 0
		double phi;		///< surface potential
		double T_OX;	///< oxide thickness
		double P_b;		///< bulk junction potential
		double M_J;		///< bulk junction bottom grading coefficient
		double F_C;		///< bulk junction forward-bbias depletion capacitance coefficient
		double M_JSW;	///< bulk junction periphery grading coefficient
		double U_0;		///< surface mobility
		int N_RD;		///< number of equivalent drain squares
		int N_RS;		///< number of equivalent source squares
#endif
};


/**
@author David Saxton
 */
class MOSFET : public NonLinear
{
	public:
		enum MOSFET_type { neMOSFET, peMOSFET/*, ndMOSFET, pdMOSFET*/ };
		
		MOSFET( MOSFET_type type );
		~MOSFET() override;
	
		Type type() const override { return Element_MOSFET; }
		void update_dc() override;
		void add_initial_dc() override;
		MOSFETSettings settings() const { return m_mosfetSettings; }
		void setMOSFETSettings( const MOSFETSettings & settings );
	
	protected:
		void calcIg( double V_BS, double V_BD, double V_DS, double V_GS, double V_GD,
							 double * I_BS, double * I_BD, double * I_DS,
							 double * g_BS, double * g_BD, double * g_DS,
							 double * g_M ) const;
		
		void updateLim();
		void updateCurrents() override;
		/**
		 * Calculates the new MOSFETState from the voltages on the nodes.
		 */
		void calc_eq();
		
		MOSFETState m_os;
		MOSFETState m_ns;
		int m_pol;
		double V_lim;
		double V_GS_prev, V_GD_prev, V_BD_prev, V_DS_prev, V_BS_prev;
		MOSFETSettings m_mosfetSettings;
		
		static const uint PinD, PinG, PinS, PinB;
};

#endif
