/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CURRENTSIGNAL_H
#define CURRENTSIGNAL_H

#include "reactive.h"
#include "elementsignal.h"

/**
@short CurrentSignal
@author David saxton
*/
class CurrentSignal : public Reactive, public ElementSignal
{
public:
	CurrentSignal(  double delta, double current );
	virtual ~CurrentSignal();
	
	virtual Element::Type type() const { return Element_CurrentSignal; }
	void setCurrent( double current );
	double current() { return m_current; }
	virtual void time_step();

    virtual void updateCurrents();
protected:
	virtual void add_initial_dc();
	void addCurrents();
	
	double m_current; // Current
	double m_oldCurrent; // Old calculated current
	double m_newCurrent; // New calculated current
};

#endif
