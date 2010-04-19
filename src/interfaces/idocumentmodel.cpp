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

#include "idocumentmodel.h"
#include "component/icomponent.h"

using namespace KTechLab;

class KTechLab::IDocumentModelPrivate {

public:
    QVariantMap components;
    QVariantMap connectors;
};

IDocumentModel::IDocumentModel ( QObject* parent )
    : QAbstractTableModel ( parent ),
      d(new IDocumentModelPrivate())
{

}

IDocumentModel::~IDocumentModel()
{
    delete d;
}

QVariant IDocumentModel::data(const QModelIndex& index, int role) const
{
    return QVariant();
}

int IDocumentModel::columnCount(const QModelIndex& parent) const
{
    if ( !parent.isValid() )
        return d->components.size();

    return 0;
}

int IDocumentModel::rowCount(const QModelIndex& parent) const
{
    if ( !parent.isValid() )
        return d->components.size();

    return 0;
}

bool IDocumentModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    return QAbstractItemModel::setData(index, value, role);
}


bool IDocumentModel::addComponent(const QVariantMap& component)
{
    if (!component.contains("id"))
        return false;

    d->components.insert( component.value("id").toString(), component);
    return true;
}

QVariantMap IDocumentModel::component(const QString& key) const
{
    return d->components.value(key).toMap();
}

QVariantMap IDocumentModel::components() const
{
    return d->components;
}

bool IDocumentModel::addConnector(const QVariantMap& connector)
{
    if ( !connector.contains( "id" ) )
        return false;

    d->connectors.insert( connector.value("id").toString(), connector );
    return true;
}

QVariantMap IDocumentModel::connector(const QString& key) const
{
    return d->connectors.value(key).toMap();
}

QVariantMap IDocumentModel::connectors() const
{
    return d->connectors;
}

#include "idocumentmodel.moc"