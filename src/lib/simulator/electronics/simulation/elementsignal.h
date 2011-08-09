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

#include "simulatorexport.h"

/**
@short Provides different signals
@author David Saxton
*/
class SIMULATOR_EXPORT ElementSignal
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
	
	void setStep(Type type, double frequency );
	/**
	 * Advances the timer, returns amplitude (between -1 and 1)
	 */
	double advance(double delta);

protected:
	Type m_type;
	double m_time;
	double m_frequency;
	double m_omega; // Used for sinusoidal signal
};

#endif
