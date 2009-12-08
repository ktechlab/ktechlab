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
#include "ktlcircuitplugin.h"

#include <interfaces/iplugincontroller.h>
#include <shell/core.h>
#include <KDebug>

using namespace KTechLab;

CircuitModel::CircuitModel ( QObject* parent )
    : IDocumentModel ( parent )
{
    QStringList constraints;
    constraints << QString("'%1' in [X-KDevelop-SupportedMimeTypes]").arg("application/x-circuit");
    QList<KDevelop::IPlugin*> plugins = KDevelop::Core::self()->pluginController()->allPluginsForExtension( "org.kdevelop.idocument", constraints );
    if (plugins.isEmpty()) {
        kError() << "No plugin found to load KTechLab Documents";
    } else {
        m_circuitPlugin = qobject_cast<KTLCircuitPlugin*>( plugins.first() );
    }
}

void CircuitModel::addComponent ( const QVariantMap& component )
{
    if ( component.contains( "id" ) ) {
        QVariantMap map(component);
        map.insert( "fileName",
                    m_circuitPlugin->fileNameForComponent( map.value("type").toString() )
                );
        m_components.insert( component.value("id").toString(), map );
    }
}

QVariantMap CircuitModel::components() const
{
    return m_components;
}

QVariantMap CircuitModel::component ( const QString& id )
{
    return m_components.value(id).toMap();
}

void CircuitModel::addConnector ( const QVariantMap& connector )
{
    if ( connector.contains( "id" ) )
        m_connectors.insert( connector.value("id").toString(), connector );
}


QVariantMap CircuitModel::connectors() const
{
    return m_connectors;
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

