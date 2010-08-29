/*
    KTechLab, an IDE for electronics
    Copyright (C) 2010 Zoltan Padrah

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "elementset.h"

#include "math/matrix.h"
#include "math/quickmatrix.h"
#include <interfaces/simulator/ielement.h>
#include "pingroup.h"

using namespace KTechLab;

KTechLab::ElementSet::ElementSet(IElement *start,
                   QList<IElement*> elements,
                   QList<PinGroup*> pinGroups)
    : IElementSet()
{
    // TODO implement

    buildElementList();
    allocateMatrixes();
    assignNodeAndSourceIds();
}

void ElementSet::buildElementList()
{
    // TODO implement
}

void ElementSet::allocateMatrixes()
{
    // TODO implement
}

void ElementSet::assignNodeAndSourceIds()
{
    // TODO implement
}


KTechLab::ElementSet::~ElementSet()
{
    // TODO implement
}

void KTechLab::ElementSet::solveEquations()
{
    // TODO implement
}

double& KTechLab::ElementSet::A_g(const unsigned int i, const unsigned int j)
{
    // TODO implement
}

double& KTechLab::ElementSet::A_b(const unsigned int i, const unsigned int j)
{
    // TODO implement
}

double& KTechLab::ElementSet::A_c(const unsigned int i, const unsigned int j)
{
    // TODO implement
}

double& KTechLab::ElementSet::A_d(const unsigned int i, const unsigned int j)
{
    // TODO implement
}

double& KTechLab::ElementSet::b_i(const unsigned int i)
{
    // TODO implement
}

double& KTechLab::ElementSet::b_v(const unsigned int i)
{
    // TODO implement
}

double KTechLab::ElementSet::x_j(const unsigned int i)
{
    // TODO implement
}

double KTechLab::ElementSet::x_v(const unsigned int i)
{
    // TODO implement
}
