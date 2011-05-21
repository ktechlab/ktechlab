/*
    Fake library to provide component items for unknown components
    Copyright (C) 2011  Julian Bäume <julian@svg4all.de>

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

#include <KDebug>
#include "fakecomponentitemfactory.h"
#include <componentitem.h>

using namespace KTechLab;

FakeComponentItemFactory::FakeComponentItemFactory()
{
    KTechLab::ComponentMetaData data;
    data.name = "ec/unknown";
    addSupportedComponent(data);
}

ComponentItem* FakeComponentItemFactory::createItem(const QVariantMap& data, Theme* theme)
{
    QVariantMap d = data;
    d.insert("type","ec/unknown");
    return KTechLab::GenericComponentItemFactory::createItem(d, theme);
}
