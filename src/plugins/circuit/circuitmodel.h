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

#ifndef CIRCUITMODEL_H
#define CIRCUITMODEL_H

#include <interfaces/idocumentmodel.h>

namespace KTechLab
{
class KTLCircuitPlugin;

class CircuitModel : public IDocumentModel
{
public:
    CircuitModel ( QObject* parent = 0 );

    virtual void addComponent ( const QVariantMap& component );
    virtual QVariantMap components() const;
    virtual QVariantMap component( const QString &id );
    virtual void addConnector ( const QVariantMap& connector );
    virtual QVariantMap connectors() const;
    virtual QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    virtual int columnCount ( const QModelIndex& parent = QModelIndex() ) const;
    virtual int rowCount ( const QModelIndex& parent = QModelIndex() ) const;
    virtual bool setData ( const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

private:
    QVariantMap m_components;
    QVariantMap m_connectors;
    KTLCircuitPlugin *m_circuitPlugin;
};

}

#endif // CIRCUITMODEL_H
