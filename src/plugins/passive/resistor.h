/***************************************************************************
 *   Copyright (C) 2005 by John Myers <electronerd@electronerdia.net>      *
 *   Copyright (C) 2009 by Julian Bäume <julian@svg4all.de>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RESISTOR_H
#define RESISTOR_H

#include "interfaces/component/icomponent.h"

namespace KTechLab
{

/**
 * A standard resistor
 * \author Julian Bäume
 */
class Resistor : public IComponent
{
public:
    Resistor( );
    virtual ~Resistor();

    static ComponentMetaData metaData();

    virtual void dataChanged();

};
}
#endif //RESISTOR_H

