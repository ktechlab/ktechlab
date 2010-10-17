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

#ifndef DOCUMENTITEM_H
#define DOCUMENTITEM_H

#include <QDomNode>
#include <QHash>

class DocumentItem
{
public:
    DocumentItem(QDomNode &node, int row, DocumentItem *parent = 0);
    ~DocumentItem();
    DocumentItem* child(int i);
    DocumentItem* childWithId(const QString& id);
    DocumentItem* parent();
    QDomNode node() const;
    int row();

private:
    QDomNode domNode;
    QHash<int,DocumentItem*> childItems;
    DocumentItem* parentItem;
    int rowNumber;
};

#endif // DOCUMENTITEM_H
