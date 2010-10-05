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

#include <interfaces/iplugincontroller.h>
#include <shell/core.h>
#include <KDebug>
#include <interfaces/component/componentmimedata.h>

using namespace KTechLab;

CircuitModel::CircuitModel ( QDomDocument doc, QObject* parent )
    : IDocumentModel ( doc, parent )
{
    QDomElement root = doc.documentElement();
    QDomNode node = root.firstChild();
    while ( !node.isNull() ) {
        QDomElement element = node.toElement();
        if ( !element.isNull() ) {
            const QString tagName = element.tagName();
            QDomNamedNodeMap attribs = element.attributes();
            QVariantMap item;
            for ( int i=0; i<attribs.count(); ++i ) {
                item[ attribs.item(i).nodeName() ] = attribs.item(i).nodeValue();
            }
            if ( tagName == "item" ) {
                addComponent( item );
            } else if ( tagName == "connector" ) {
                addConnector( item );
            } else if ( tagName == "node" ) {
                addNode( item );
            }
        }
        node = node.nextSibling();
    }
}

QVariantMap CircuitModel::createComponent(const KTechLab::ComponentMimeData* data, QPointF pos)
{
    QVariantMap comp;
    comp.insert("type", data->type());
    comp.insert("x", pos.x());
    comp.insert("y", pos.y());
    comp.insert("id", generateUid(data->name()));
    addComponent(comp);

    return component(comp.value("id").toString());
}

void CircuitModel::addComponent ( const QVariantMap& component )
{
   if ( isValidComponent(component) ) {
        QVariantMap map(component);
/*        map.insert( "fileName",
                    m_circuitPlugin->fileNameForComponent( map.value("type").toString() )
                );*/
        m_components.insert( component.value("id").toString(), map );
    }
}

bool CircuitModel::isValidComponent(const QVariantMap& c)
{
    return c.contains("id");
}

QVariantMap CircuitModel::components() const
{
    return m_components;
}

QVariantMap CircuitModel::component ( const QString& id ) const
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

