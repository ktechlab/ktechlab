/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef INDUCTOR_H
#define INDUCTOR_H

#include "component.h"
#include "simulatorexport.h"

class Circuit;
class Inductance;

/**
@author David Saxton
*/
class SIMULATOR_EXPORT Inductor : public Component {

public:
    Inductor(Circuit &ownerCircuit);
    ~Inductor();

protected:
    virtual void propertyChanged(Property& theProperty,
                                    QVariant newValue, QVariant oldValue );

    Inductance *m_pInductance;
};

#endif
