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

#include "iwire.h"

#include "ipin.h"

#if KDE_ENABLED
#include <kdebug.h>
#endif
#include <QDebug>

using namespace KTechLab;

IWire::IWire(IPin* start, IPin* end, QVariantMap& parentInModel) :
    m_parentInModel(parentInModel)
{
    m_start = start;
    m_end = end;
    // TODO implement
}

KTechLab::IWire::~IWire()
{
    m_start = 0;
    m_end = 0;
}

QVariantMap& KTechLab::IWire::parentInModel() const
{
    return m_parentInModel;
}

void KTechLab::IWire::setCurrentIsKnown(bool known)
{
    m_currentKnown = known;
}

bool KTechLab::IWire::currentIsKnown() const
{
    return m_currentKnown;
}

void KTechLab::IWire::setCurrent(double current)
{
    m_current = current;
}

double KTechLab::IWire::current() const
{
    return m_current;
}

KTechLab::IPin* KTechLab::IWire::startPin() const
{
    return m_start;
}

KTechLab::IPin* KTechLab::IWire::endPin() const
{
    return m_end;
}

double KTechLab::IWire::voltage() const
{
    if( m_start->voltage() != m_end->voltage() )
#if KDE_EANBLED
        kWarning() << "different voltages on the ends of a wire: "
            << m_start->voltage() << " and "
            << m_end->voltage() << "\n";
#else
        qWarning() << "different voltages on the ends of a wire: "
            << m_start->voltage() << " and "
            << m_end->voltage() << "\n";

#endif
    return m_start->voltage();
}

void KTechLab::IWire::transferDataToModel()
{
    // TODO implement
}
