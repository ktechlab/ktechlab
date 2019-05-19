/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef REACTIVE_H
#define REACTIVE_H

#include "element.h"

/**
@short Represents a reactive element (such as a capacitor)
@author David Saxton
*/
class Reactive : public Element
{
public:
	Reactive( const double delta );
	virtual ~Reactive();

	virtual bool isReactive() const override { return true; }
	/**
	 * Call this function to set the time period (in seconds)
	 */
	void setDelta( double delta );
	/**
	 * Called on every time step for the element to update itself
	 */
	virtual void time_step() = 0;

protected:
	virtual bool updateStatus() override;

	double m_delta; // Delta time interval
};

#endif
