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
#include "private/documentitem.h"
#include <KDebug>
#include <qtextdocument.h>

using namespace KTechLab;

class KTechLab::IDocumentModelPrivate {

public:
    IDocumentModelPrivate() {
        textDocument.setUndoRedoEnabled(true);
    };

    ~IDocumentModelPrivate();
    QString generateUid(const QString& name);
    DocumentItem* itemFromIndex(QModelIndex index) const;
    QVariantMap components;
    QVariantMap connectors;
    QVariantMap nodes;
    QDomDocument doc;
    QTextDocument textDocument;
    DocumentItem* rootItem;

private:
    QSet<QString> m_ids;
    int m_nextIdNum;
};

IDocumentModelPrivate::~IDocumentModelPrivate()
{
    delete rootItem;
}

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

DocumentItem* IDocumentModelPrivate::itemFromIndex(QModelIndex index) const
{
    if ( !index.isValid() ) {
        return rootItem;
    } else {
        return static_cast<DocumentItem*>(index.internalPointer());
    }
}

IDocumentModel::IDocumentModel ( QDomDocument doc, QObject* parent )
    : QAbstractItemModel ( parent ),
      d(new IDocumentModelPrivate())
{
    d->doc = doc;
    d->textDocument.setPlainText(doc.toString());
    d->textDocument.setModified(false);
    d->rootItem = new DocumentItem(d->doc, 0);
}

IDocumentModel::~IDocumentModel()
{
    delete d;
}

QVariant IDocumentModel::data(const QModelIndex& index, int role) const
{
    DocumentItem* item = d->itemFromIndex(index);
    if (role == IDocumentModel::XMLDataRole) {
        QByteArray res;
        QTextStream stream(&res);
        item->node().save(stream, 1);
        return res;
    }
    return QVariant();
}

int IDocumentModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}

int IDocumentModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return 0;

    DocumentItem* parentItem = d->itemFromIndex(parent);

    return parentItem->node().childNodes().count();
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

QTextDocument* IDocumentModel::textDocument() const
{
    return &d->textDocument;
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
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DocumentItem *parentItem;
    if (!parent.isValid())
        parentItem = d->rootItem;
    else
        parentItem = static_cast<DocumentItem*>(parent.internalPointer());

    DocumentItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex IDocumentModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    DocumentItem *childItem = static_cast<DocumentItem*>(child.internalPointer());
    DocumentItem *parentItem = childItem->parent();

    if (!parentItem || parentItem == d->rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

#include "idocumentmodel.moc"