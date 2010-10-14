/*
    Copyright (C) 2009-2010 Julian Bäume <julian@svg4all.de>

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

#include "icomponentitem.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <KDebug>
#include <idocumentmodel.h>
#include "node.h"

using namespace KTechLab;

IComponentItem::IComponentItem(QGraphicsItem* parentItem)
    : QGraphicsSvgItem(parentItem),
      m_document(0)
{
    setAcceptHoverEvents(true);
    setFlags(
        ItemIsFocusable | ItemIsSelectable |
        ItemIsMovable
    );
}

IComponentItem::~IComponentItem()
{

}

bool IComponentItem::hasNode(const Node* node) const
{
    if (!node->isValid() || id() != node->parentId())
        return false;

    return this->node(node->id()) != 0;
}

const Node* IComponentItem::node(const QString& id) const
{
    foreach (const QGraphicsItem* item, childItems()){
        //TODO: make this a qgraphicsitem_cast
        const Node* n = dynamic_cast<const Node*>(item);
        if (n && n->id() == id){
            return n;
        }
    }
    return 0;
}
QList<const Node*> IComponentItem::nodes() const
{
    QList<const Node*> list;
    foreach (const QGraphicsItem* item, childItems()){
        const Node* n = qgraphicsitem_cast<const Node*>(item);
        if (n) list.append(n);
    }
    return list;
}

void IComponentItem::setDocumentModel(IDocumentModel* model)
{
    m_document = model;
}

IDocumentModel* IComponentItem::documentModel() const
{
    return m_document;
}

QVariant KTechLab::IComponentItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if (change == QGraphicsItem::ItemScenePositionHasChanged && m_document){
        QPointF p = value.toPointF();
        QVariantMap newData = QGraphicsItem::data(0).toMap();
        newData.insert("x", p.x());
        newData.insert("y", p.y());
        setData(0, newData);
        m_document->updateData( "position", newData);
    }
    return QGraphicsItem::itemChange(change, value);
}

void IComponentItem::updateData(const QString& name, const QVariantMap& data)
{

}
