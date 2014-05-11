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

#if KDE_ENABLED
#include <KDebug>
#else
#include <QDebug>
#endif

#include <interfaces/component/componentmimedata.h>

using namespace KTechLab;

CircuitModel::CircuitModel ( QDomDocument doc, QObject* parent )
    : IDocumentModel ( doc, parent )
{}

int KTechLab::CircuitModel::columnCount(const QModelIndex& parent) const
{
    if (isItem(parent))
        return 2;

    return KTechLab::IDocumentModel::columnCount(parent);
}

QVariant CircuitModel::data(const QModelIndex& index, int role) const
{
    if (isItem(index.parent())) {
        const QDomElement node = domNode(index).toElement();
        if (index.column() == 0 && role == Qt::DisplayRole) {
            return QVariant(node.attribute("id", tr("Invalid Data")));
        } else if ((index.column() == 1 && role == Qt::DisplayRole) || role == Qt::EditRole) {
            return QVariant(node.attribute("value", tr("Invalid Data")));
        }
    }
    return KTechLab::IDocumentModel::data(index, role);
}

Qt::ItemFlags CircuitModel::flags(const QModelIndex& index) const
{
    if (!isItem(index.parent()))
        return KTechLab::IDocumentModel::flags(index);

    if (index.column() == 0) {
        return KTechLab::IDocumentModel::flags(index) & ~Qt::ItemIsSelectable;
    } else if (index.column() == 1) {
        return KTechLab::IDocumentModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }

    return KTechLab::IDocumentModel::flags(index);
}

bool KTechLab::CircuitModel::isItem(const QModelIndex& parent) const
{
    const QDomNode node = domNode(parent);

    if (!node.toElement().isNull() && node.nodeName() == "item")
        return true;

    return false;
}
