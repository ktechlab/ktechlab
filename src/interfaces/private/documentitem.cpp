/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) <year>  <name of author>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

#include "documentitem.h"

#include <QDebug>

DocumentItem::DocumentItem(QDomNode& node, int row, DocumentItem* parent)
    :   domNode(node),
        parentItem(parent),
        rowNumber(row)
{
}

DocumentItem::~DocumentItem()
{
    qDeleteAll< QHash<int,DocumentItem*> >(childItems);
}

QDomNode DocumentItem::node() const
{
    return domNode;
}

DocumentItem* DocumentItem::child(int i)
{
    if (i >= 0 && i < domNode.childNodes().count()) {
        QDomNode childNode = domNode.childNodes().item(i);
        DocumentItem *childItem = new DocumentItem(childNode, i, this);
        childItems[i] = childItem;
        return childItem;
    }
    return 0;
}

DocumentItem* DocumentItem::childWithId(const QString& id)
{
    for(int i = 0; i<node().childNodes().size(); ++i){
        QDomNamedNodeMap attribs = child(i)->node().attributes();
        if (attribs.contains("id") && attribs.namedItem("id").nodeValue() == id)
            return child(i);

        DocumentItem* c = child(i)->childWithId(id);
        if (c) return c;
    }
    return 0;
}

DocumentItem* DocumentItem::parent()
{
    return parentItem;
}

int DocumentItem::row()
{
    return rowNumber;
}
