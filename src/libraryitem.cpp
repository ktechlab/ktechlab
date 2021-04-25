/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "libraryitem.h"

#include <QImage>
#include <QStandardPaths>

LibraryItem::LibraryItem(QStringList idList, const QString &name, const QString &category, QIcon icon, Type type, createItemPtr _createItem)
{
    m_idList = idList;
    m_name = name;
    m_category = category;
    m_icon = icon;
    m_type = type;
    createItem = _createItem;
}

LibraryItem::LibraryItem(QStringList idList, const QString &name, const QString &category, const QString &iconName, Type type, createItemPtr _createItem)
{
    m_idList = idList;
    m_name = name;
    m_category = category;
    m_icon = QIcon(QStandardPaths::locate(QStandardPaths::AppDataLocation, "icons/" + iconName));
    m_type = type;
    createItem = _createItem;
}

LibraryItem::LibraryItem(QStringList idList, const QString &name, const QString &category, Type type, createItemPtr _createItem)
{
    m_idList = idList;
    m_name = name;
    m_category = category;
    m_type = type;
    createItem = _createItem;
}

LibraryItem::~LibraryItem()
{
}

QString LibraryItem::activeID() const
{
    return m_idList.isEmpty() ? "" : m_idList[0];
}
