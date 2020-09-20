/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "reactive.h"

Reactive::Reactive(const double delta)
    : Element()
{
    m_delta = delta;
}

Reactive::~Reactive()
{
}

void Reactive::setDelta(double delta)
{
    m_delta = delta;
    updateStatus();
}

bool Reactive::updateStatus()
{
    return Element::updateStatus();
}
