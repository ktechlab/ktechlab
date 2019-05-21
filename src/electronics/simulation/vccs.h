/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VCCS_H
#define VCCS_H

#include "element.h"

/**
CNodes n0 and n1 are used for the voltage control.
CNodes n2 and n3 are used for the current output.
@short Voltage Controlled Current Source
@author David Saxton
*/
class VCCS : public Element
{
public:
	VCCS( const double gain );
	~VCCS() override;
	
	Type type() const override { return Element_VCCS; }
	void setGain( const double g );

protected:
	void updateCurrents() override;
	void add_initial_dc() override;
	
private:
	double m_g; // Conductance
};

#endif
