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

/**
Voltage source between nodes c2 and c3
Controlling voltage between nodes c0 and c1
@short Voltage Controlled Voltage  Source
@author David Saxton
*/
class VCVS : public Element
{
public:
	VCVS( const double gain );
	~VCVS() override;
	
	Type type() const override { return Element_VCVS; }
	void setGain( const double g );

protected:
	void updateCurrents() override;
	void add_initial_dc() override;
	
private:
	double m_g; // Conductance
};

#endif
