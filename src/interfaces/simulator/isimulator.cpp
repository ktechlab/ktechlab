/***************************************************************************
 *    ISimulator interface implementation                                  *
 *       A controller for simulations in KTechLab                          *
 *     Copyright (c) 2010 Zoltan Padrah                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "isimulator.h"

using namespace KTechLab;

ISimulator::ISimulator(IDocumentModel* doc)
{
    Q_UNUSED( doc );
}


ISimulator::~ISimulator(){
}
