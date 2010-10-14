/*
    Copyright (C) 2010 Julian Bäume <julian@svg4all.de>

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

#include "idocumentitem.h"
#include <QString>

using namespace KTechLab;

class IDocumentItem::IDocumentItemPrivate {
public:
    QString id;
    QString name;
    QString type;
};

IDocumentItem::IDocumentItem()
    : d(new IDocumentItemPrivate())
{

}
IDocumentItem::~IDocumentItem()
{
    delete d;
}
QString KTechLab::IDocumentItem::id() const
{
    return d->id;
}
QString KTechLab::IDocumentItem::name() const
{
    return d->name;
}
QString KTechLab::IDocumentItem::type() const
{
    return d->type;
}
QVariantMap IDocumentItem::data() const
{
    QVariantMap map;
    if (!d->id.isEmpty())
        map.insert("id", d->id);
    if (!d->name.isEmpty())
        map.insert("name", d->name);
    if (!d->type.isEmpty())
        map.insert("type", d->type);
    return map;
}
void KTechLab::IDocumentItem::setId(const QString & id)
{
    d->id = id;
}
void KTechLab::IDocumentItem::setName(const QString & name)
{
    d->name = name;
}
void KTechLab::IDocumentItem::setType(const QString & type)
{
    d->type = type;
}
