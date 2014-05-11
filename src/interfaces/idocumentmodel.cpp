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

#if KDE_ENABLED
#include <KDebug>
#else
#include <QDebug>
#endif

#include <QSet>
#include "private/documentitem.h"
#include <qtextdocument.h>

using namespace KTechLab;

class KTechLab::IDocumentModelPrivate {

public:
    IDocumentModelPrivate(IDocumentModel* m)
        : m_model(m)
    {
        textDocument.setUndoRedoEnabled(true);
    };

    ~IDocumentModelPrivate();
    QString generateUid(const QString& name);
    DocumentItem* itemFromIndex(QModelIndex index) const;
    void addData(QDomElement e, const QVariantMap& data, QVariantMap* dataContainer) const;
    QModelIndex indexFromId(const QString& id);
    QVariantMap components;
    QVariantMap connectors;
    QVariantMap nodes;
    QDomDocument doc;
    QTextDocument textDocument;
    DocumentItem* rootItem;

private:
    IDocumentModel* m_model;
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

QModelIndex IDocumentModelPrivate::indexFromId(const QString& id)
{
    DocumentItem* item = rootItem->childWithId(id);
    if (!item) return QModelIndex();

    QModelIndex rootIndex = m_model->index(0,0);
    return m_model->index(item->row(),0,rootIndex);
}

void IDocumentModelPrivate::addData(QDomElement e,
                                    const QVariantMap& data,
                                    QVariantMap* dataContainer) const
{
    QModelIndex parent = m_model->index(0,0);
    DocumentItem* parentItem = itemFromIndex(parent);
    int rows = m_model->rowCount(parent);
    m_model->beginInsertRows(parent,rows,rows);
    parentItem->node().appendChild(e);
    dataContainer->insert( data.value("id").toString(), data);
    m_model->endInsertRows();
    QModelIndex i = m_model->index(rows,0,parent);
    m_model->setData(i,data);
}

IDocumentModel::IDocumentModel ( QDomDocument doc, QObject* parent )
    : QAbstractItemModel ( parent ),
      d(new IDocumentModelPrivate(this))
{
    d->doc = doc;
    d->textDocument.setPlainText(doc.toString());
    d->textDocument.setModified(false);
    d->rootItem = new DocumentItem(d->doc, 0);

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
                d->components.insert( item.value("id").toString(), item );
            } else if ( tagName == "connector" ) {
                d->connectors.insert( item.value("id").toString(), item );
            } else if ( tagName == "node" ) {
                d->nodes.insert( item.value("id").toString(), item );
            }
        }
        node = node.nextSibling();
    }
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

const QDomNode IDocumentModel::domNode(const QModelIndex& index) const
{
    DocumentItem* item = d->itemFromIndex(index);
    if (item)
        return item->node();

    return QDomNode();
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
    DocumentItem* item = d->itemFromIndex(index);
    if ((role == Qt::EditRole) && (item) && (value.canConvert(QVariant::Map))){
        //TODO: make sure to update the containers in d as well
        QVariantMap data = value.toMap();
        QDomElement e = item->node().toElement();
        foreach(const QString& key, data.keys()){
            e.setAttribute(key, data[key].toString());
        }
        emit QAbstractItemModel::dataChanged(index,index);
        return true;
    }
    return false;
}

bool IDocumentModel::removeRows(int row, int count, const QModelIndex& parent)
{
    QModelIndex rowIndex = index(row,0,parent);
    if (!rowIndex.isValid())
        return false;

    if (count > 1) {
#if KDE_ENABLED
        kWarning() << "Model only supports to remove one row at a time. Removing only row:" << row;
#else
        qWarning() << "Model only supports to remove one row at a time. Removing only row:" << row;
#endif
    }

    DocumentItem* item = d->itemFromIndex(rowIndex);
    item->parent()->removeChild(item->row());

    return true;
}

void IDocumentModel::addComponent(const QVariantMap& component)
{
    QDomElement e = d->doc.createElement("item");
    d->addData(e, component, &d->components );
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
    QDomElement e = d->doc.createElement("connector");
    d->addData(e, connector, &d->connectors);
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
    QDomElement e = d->doc.createElement("node");
    d->addData(e, node, &d->nodes);
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

void IDocumentModel::updateData(const QString& id, const QVariantMap& data)
{
    const QModelIndex& modelIndex = d->indexFromId(id);
    setData(modelIndex, data);
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

QModelIndex IDocumentModel::index(const QVariantMap& item) const
{
    const QString& id = item.value("id").toString();
    if (id.isEmpty())
        return QModelIndex();

    return d->indexFromId(id);
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

void IDocumentModel::revert()
{
    d->doc.setContent(d->textDocument.toPlainText());
    delete d->rootItem;
    d->rootItem = new DocumentItem(d->doc,0);
    QAbstractItemModel::revert();
}

bool IDocumentModel::submit()
{
    d->textDocument.setPlainText(d->doc.toString());
    return QAbstractItemModel::submit();
}


#include "idocumentmodel.moc"
