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
#include <limits>
typedef unsigned int uint;

class CNode
{
public:
    /// Represents a single node or "net" in a circuit.
    CNode(const uint n = 0): num(n) {}

    /// Voltage on node. This is set from the last calculated voltage.
    double v = 0.0;

    /// CNode number / element set index. If this == CNode::GROUND
    /// Obviously, you should ignore n and v if this is true
    uint num = 0;

    // Number assigned to ground or reference node.
    // This is out of bounds so any attempts to use it can be detected.
    static const uint GROUND = std::numeric_limits<uint>::max();
};

