/***************************************************************************
 *    Capacitance model                                                    *
 *       used to simulate capacitances in KTechLab                         *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "capacitance.h"

KTechLab::Capacitance::Capacitance() :
    IElement(NULL, NULL, 1, 1)
{
    // TODO implement
}

KTechLab::Capacitance::Capacitance(QVariantMap* parentInModel, KTechLab::IElementSet* elementSet):
    IElement(parentInModel, elementSet, 1, 1)
{
    // TODO implement
}

void KTechLab::Capacitance::fillMatrixCoefficients()
{
    // TODO implement
}
