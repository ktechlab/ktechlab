/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECCURRENTSOURCE_H
#define ECCURRENTSOURCE_H

#include "component.h"
#include "currentsource.h"
#include "simulatorexport.h"

class Circuit;
class CurrentSource;

/**
@short Fixed current source
@author David Saxton
*/
class SIMULATOR_EXPORT ECCurrentSource : public Component
{
public:
	ECCurrentSource(Circuit &ownerCircuit);
	~ECCurrentSource();

private:
    virtual void propertyChanged(Property& theProperty, QVariant newValue,
                                 QVariant oldValue);

	CurrentSource *m_currentSource;
    ElementMap *m_elemMap;

};

#endif
