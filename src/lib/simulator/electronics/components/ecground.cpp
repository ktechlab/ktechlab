/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecground.h"

ECGround::ECGround(Circuit& ownerCircuit)
        : Component(ownerCircuit) {
    // TODO add pin
    // init1PinRight();
    // m_pPNode[0]->pin().setGroundType(Pin::gt_always);
}

ECGround::~ECGround() {}
