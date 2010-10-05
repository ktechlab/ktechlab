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

#include <QSet>

using namespace KTechLab;

class KTechLab::IDocumentModelPrivate {

public:
    QString generateUid(const QString& name);
    QVariantMap components;
    QVariantMap connectors;
    QVariantMap nodes;
    QDomDocument doc;

private:
    QSet<QString> m_ids;
    int m_nextIdNum;
};

QString IDocumentModelPrivate::generateUid(const QString& name)
{
    QString cleanName = name;
    cleanName.remove(QRegExp("__[0-9]*")); //Change 'node__13' to 'node', for example
    QString idAttempt = cleanName;

    while (m_ids.contains(idAttempt))
        idAttempt = cleanName + "__" + QString::number(m_nextIdNum++);

    m_ids.insert(idAttempt);
    return idAttempt;
}

IDocumentModel::IDocumentModel ( QDomDocument doc, QObject* parent )
    : QAbstractItemModel ( parent ),
      d(new IDocumentModelPrivate())
{
    d->doc = doc;
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


void IDocumentModel::addComponent(const QVariantMap& component)
{
    if (!component.contains("id"))
        return;

    d->components.insert( component.value("id").toString(), component);
}

QVariantMap IDocumentModel::component(const QString& key) const
{
    return d->components.value(key).toMap();
}

QVariantMap IDocumentModel::components() const
{
    return d->components;
}

void IDocumentModel::addConnector(const QVariantMap& connector)
{
    if ( !connector.contains( "id" ) )
        return;

    d->connectors.insert( connector.value("id").toString(), connector );
}

QVariantMap IDocumentModel::connector(const QString& key) const
{
    return d->connectors.value(key).toMap();
}

QVariantMap IDocumentModel::connectors() const
{
    return d->connectors;
}


void IDocumentModel::addNode(const QVariantMap& node)
{
    if ( node.contains( "id" ) )
        d->nodes.insert( node.value("id").toString(), node );
}

QVariantMap IDocumentModel::node(const QString& id)
{
    if ( d->nodes.contains( id ) )
        return d->nodes.value(id).toMap();

    return QVariantMap();
}

QVariantMap IDocumentModel::nodes() const
{
    return d->nodes;
}


void IDocumentModel::updateData(const QString& name, const QVariantMap& data)
{

}

QString IDocumentModel::generateUid(const QString& name)
{
    return d->generateUid(name);
}

Qt::ItemFlags IDocumentModel::flags(const QModelIndex& index) const
{
    return QAbstractItemModel::flags(index);
}

QVariant IDocumentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex IDocumentModel::index(int row, int column, const QModelIndex& parent) const
{

}

QModelIndex IDocumentModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}

#include "idocumentmodel.moc"