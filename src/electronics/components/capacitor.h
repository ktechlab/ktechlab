/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "component.h"
#include "capacitance.h"

class ECNode;

/**
@short Capacitor
Simple capacitor
@author David Saxton
*/
class Capacitor : public Component
{
public:
	Capacitor();
	~Capacitor();

    /**
     * \return the capacitance of the component
     */
    double capacitance() const;
    /**
     * set the value of the capaciance
     */
    void setCapacitance(double capacitance);

protected:
    virtual void propertyChanged(Property& theProperty,
                                 QVariant newValue, QVariant oldValue );

private:
	// void dataChanged();

	Capacitance m_capacitance;
};

#endif
