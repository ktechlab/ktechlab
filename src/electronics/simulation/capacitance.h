/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CAPACITANCE_H
#define CAPACITANCE_H

#include "reactive.h"

/**
@author David Saxton
@short Capacitance
*/
class Capacitance : public Reactive
{
public:
	enum Method
	{
		m_none, // None
		m_euler, // Backward Euler
		m_trap // Trapezoidal (currently unimplemented)
	};
	Capacitance( const double capacitance, const double delta );
	virtual ~Capacitance();
	
	virtual Type type() const { return Element_Capacitance; }
	/**
	 * Set the stepping use for numerical integration of capacitance,
	 * and the interval between successive updates
	 */
	void setMethod( Method m );
	virtual void time_step();
	virtual void add_initial_dc();
	void setCapacitance( const double c );

    virtual void updateCurrents();
protected:
	virtual bool updateStatus();
	
private:
	double m_cap; // Capacitance
	Method m_method; // Method of integration
	
	double m_scaled_cap; // capacitance scaled to time base of latest m_delta
	double i_eq_old;
};

#endif
