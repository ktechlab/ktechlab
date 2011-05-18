/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef INDUCTANCE_H
#define INDUCTANCE_H

#include "reactive.h"

/**

@author David Saxton
*/
class Inductance : public Reactive
{
	public:
		enum Method
		{
			m_none, // None
			m_euler, // Backward Euler
			m_trap // Trapezoidal (currently unimplemented)
		};
		Inductance(const double inductance, const double delta);
		virtual ~Inductance();
	
		virtual Type type() const { return Element_Inductance; }
		/**
		 * Set the stepping use for numerical integration of inductance, and the
		 * interval between successive updates.
		 */
		void setMethod( Method m );
		virtual void time_step();
		virtual void add_initial_dc();
		void setInductance( double i );

        virtual void updateCurrents();
	protected:
		virtual bool updateStatus();
	
	private:
		double m_inductance; // Inductance
		Method m_method; // Method of integration
		
		double scaled_inductance;
};

#endif
