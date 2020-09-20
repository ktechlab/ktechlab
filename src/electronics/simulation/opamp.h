/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OPAMP_H
#define OPAMP_H

#include <element.h>

/**
node 0: non-inverting input
node 1: output
node 2: inverting input
@author David Saxton
*/
class OpAmp : public Element
{
public:
    OpAmp();
    ~OpAmp() override;

    Type type() const override
    {
        return Element_OpAmp;
    }

protected:
    void updateCurrents() override;
    void add_initial_dc() override;
};

#endif
