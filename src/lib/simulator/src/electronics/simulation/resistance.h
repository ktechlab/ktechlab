/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RESISTANCE_H
#define RESISTANCE_H

#include "element.h"
#include "simulatorexport.h"

/**
@short Resistance
@author David saxton
*/
class SIMULATOR_EXPORT Resistance : public Element {

public:
    Resistance(const double resistance = 1);
    virtual ~Resistance();

    virtual Type type() const { return Element_Resistance; }

    void setConductance(const double g);
    void setResistance(const double r);

    double resistance() { return 1 / m_g; }
    double conductance() { return m_g; }

    virtual void updateCurrents();

protected:
    virtual void add_initial_dc();

private:
    double m_g; // Conductance
};

#endif
