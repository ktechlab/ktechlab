/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ELEMENTSIGNAL_H
#define ELEMENTSIGNAL_H

/**
@short Provides different signals
@author David Saxton
*/
class ElementSignal
{
public:
	enum Type
	{
		st_sinusoidal,
		st_square,
		st_sawtooth,
		st_triangular
	};
	ElementSignal();
	~ElementSignal();
	
	void setStep( double delta, Type type, double frequency );
	/**
	 * Advances the timer, returns amplitude (between -1 and 1)
	 */
	double advance();

protected:

// TODO: Implement phase angle variable so that the user can specify multiple generators
// with precise phase offsets. 

	Type m_type;
	double m_time;  // TODO SHOULD BE DISCARDED IN FAVOR OF GLOBAL CLOCK. 
	double m_frequency;
	double m_delta; // TODO SHOULD BE DISCARDED IN FAVOR OF GLOBAL CLOCK. 
	double m_omega; // Used for sinusoidal signal
// random moron: HUH?  
};

#endif
