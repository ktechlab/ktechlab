/*
    KTechLab, and IDE for electronics
    IPin class, used in simulations, as model for a PinItem or Junction
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

#include "ipin.h"

#if KDE_ENABLED
#include <kdebug.h>
#endif
#include <QDebug>

using namespace KTechLab;

IPin::IPin(const QVariantMap& parentInModel, QString pinName, IElement *parent):
    QObject(), m_parentInModel(parentInModel)
{
    m_name = pinName;
    // TODO implement
    m_currentIn = 0;
    m_voltage = 0;
    m_parent = parent;
    m_wires.clear();
}


KTechLab::IPin::~IPin()
{
    m_currentIn = 0;
    m_voltage = 0;
    m_parent = NULL;
    m_wires.clear();
}

void KTechLab::IPin::setVoltage(double voltage)
{
    m_voltage = voltage;
}

double KTechLab::IPin::voltage() const
{
    return m_voltage;
}

void KTechLab::IPin::setCurrentIn(double current)
{
    m_currentIn = current;
}

double KTechLab::IPin::currentIn() const
{
    return m_currentIn;
}

const KTechLab::IElement* KTechLab::IPin::parent() const
{
    return m_parent;
}

const QVariantMap& KTechLab::IPin::parentInModel() const
{
    return m_parentInModel;
}

QList< KTechLab::IWire* > KTechLab::IPin::connections() const
{
    return m_wires;
}

void KTechLab::IPin::connectWire(IWire* wire)
{
    if( !m_wires.contains(wire) )
        m_wires.append(wire);
}

void KTechLab::IPin::disconnectWire(IWire* wire)
{
    if( m_wires.contains(wire)){
        m_wires.removeAt(
            m_wires.indexOf(wire) );
    } else {
#if KDE_ENABLED
        kError() << "tried to remove not connected wire " << wire << "\n";
#else
        qCritical() << "tried to remove not connected wire " << wire << "\n";
#endif
    }

}

void KTechLab::IPin::transferStatusToModel()
{
    // TODO implement
    // m_parentInModel->
    qWarning() << "IPin::transferStatusToModel: Not implemented";
}
