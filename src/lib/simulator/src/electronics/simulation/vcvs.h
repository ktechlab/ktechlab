/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VCVS_H
#define VCVS_H

#include "element.h"
#include "simulatorexport.h"

/**
Voltage source between nodes c2 and c3
Controlling voltage between nodes c0 and c1
@short Voltage Controlled Voltage  Source
@author David Saxton
*/
class SIMULATOR_EXPORT VCVS : public Element
{
public:
	VCVS( const double gain );
	virtual ~VCVS();
	
	virtual Type type() const { return Element_VCVS; }
	void setGain( const double g );

    virtual void updateCurrents();
protected:
	virtual void add_initial_dc();
	
private:
	double m_g; // Conductance
};

#endif
