/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// #include <cmath>
#include <kdebug.h>
// #include <qpainter.h>
//Added by qt3to4:
// #include <Q3PointArray>

// #include "circuitdocument.h"
#include "component.h"
#include "pin.h"
#include "simulator.h"

//BEGIN class Component

Component::Component()
{

}


Component::~Component() {
	removeElements();
}

void Component::removeElements() {
	m_elementMapList.clear();
}


//END class Component

#include "component.moc"

