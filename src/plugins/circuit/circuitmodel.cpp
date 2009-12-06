/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#include "circuitmodel.h"

using namespace KTechLab;

CircuitModel::CircuitModel ( QObject* parent )
    : IDocumentModel ( parent )
{

}

void CircuitModel::addComponent ( const QVariantMap& component )
{
    if ( component.contains( "id" ) )
        m_components.insert( component.value("id").toString(), component );
}

QVariantMap CircuitModel::components() const
{
    return m_components;
}

QVariant CircuitModel::data ( const QModelIndex& index, int role ) const
{
    return QVariant();
}

int CircuitModel::columnCount ( const QModelIndex& parent ) const
{
    return 1;
}

int CircuitModel::rowCount ( const QModelIndex& parent ) const
{
    if ( !parent.isValid() )
        return m_components.size();

    return 0;
}

bool CircuitModel::setData ( const QModelIndex& index, const QVariant& value, int role )
{
    return QAbstractItemModel::setData ( index, value, role );
}

