/*
    KTechLab, and IDE for electronics
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

#include "ielementset.h"

KTechLab::IElementSet::IElementSet()
{
    m_numNodes = 0;
    m_numVoltageSources = 0;
    m_elements.clear();
}


KTechLab::IElementSet::~IElementSet()
{

}

int KTechLab::IElementSet::nodeCount() const
{
    return m_numNodes;
}

int KTechLab::IElementSet::voltageSourceCount() const
{
    return m_numVoltageSources;
}

QList< KTechLab::IElement* > KTechLab::IElementSet::elements() const
{
    return m_elements;
}

