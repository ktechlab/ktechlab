/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CCVS_H
#define CCVS_H

#include "element.h"
#include "simulatorexport.h"

/**
CNodes n0 and n1 are used for the current control.
CNodes n2 and n3 are used for the voltage output.
Branches b0 and b1 are for control and output
@short Current Controlled Voltage Source
@author David Saxton
*/
class SIMULATOR_EXPORT CCVS : public Element
{
public:
	CCVS( const double gain );
	virtual ~CCVS();
	
	virtual Type type() const { return Element_CCVS; }
	void setGain( const double g );

    virtual void updateCurrents();
protected:
	virtual void add_initial_dc();
	
private:
	double m_g; // Conductance
};

#endif
