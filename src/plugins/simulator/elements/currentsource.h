/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CURRENTSOURCE_H
#define CURRENTSOURCE_H

#include "element.h"

/**
cnode n0 has current flowing otu of it, cnode n1 has current flowing into it
@author David Saxton
@short Current Source
*/
class CurrentSource : public Element
{
public:
	CurrentSource( const double current );
	virtual ~CurrentSource();
	
	virtual Type type() const { return Element_CurrentSource; }
	void setCurrent( const double i );

protected:
	virtual void updateCurrents();
	virtual void add_initial_dc();
	
private:
	double m_i; // Current
};

#endif
