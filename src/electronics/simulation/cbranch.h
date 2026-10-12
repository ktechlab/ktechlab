/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#pragma once
typedef unsigned int uint;

class CBranch
{
public:
    CBranch(const uint n = 0): num(n) {}

    /// Current flowing through branch. This is set from the last calculated current.
    double i = 0.0;

    /// CBranch number
    uint num = 0;
};
