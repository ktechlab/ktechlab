/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECCELL_H
#define ECCELL_H

#include "component.h"

class ElementMap;
class VoltageSource;

/**
@short Electrical cell
Simple electrical cell that simulates a PD and internal resistance
@author David Saxton
*/
class ECCell : public Component
{
public:
	ECCell();
	~ECCell();

protected:
    virtual void propertyChanged(Property& theProperty, QVariant newValue, QVariant oldValue);

private:
    VoltageSource *m_voltageSource;
    ElementMap *m_sourceMap;
};

#endif
