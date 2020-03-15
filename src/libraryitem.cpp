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

#include <kiconloader.h>
#include <kicontheme.h>

#include <qimage.h>
#include <QStandardPaths>

LibraryItem::LibraryItem( QStringList idList, const QString &name, const QString &category, QPixmap icon, Type type, createItemPtr _createItem )
{
	m_idList = idList;
	m_name = name;
	m_category = category;
	m_icon_full = icon;
	m_type = type;
	createItem = _createItem;
	createIcon16();
}


LibraryItem::LibraryItem( QStringList idList, const QString &name, const QString &category, const QString &iconName, Type type, createItemPtr _createItem )
{
	m_idList = idList;
	m_name = name;
	m_category = category;
	m_icon_full.load( QStandardPaths::locate(QStandardPaths::AppDataLocation, "icons/"+iconName ) );
	m_type = type;
	createItem = _createItem;
	createIcon16();
}


LibraryItem::LibraryItem( QStringList idList, const QString &name, const QString &category, Type type, createItemPtr _createItem )
{
	m_idList = idList;
	m_name = name;
	m_category = category;
	m_type = type;
	createItem = _createItem;
	createIcon16();
}


LibraryItem::~LibraryItem()
{
}


void LibraryItem::createIcon16()
{
	if ( m_icon_full.isNull() )
		m_icon_full = KIconLoader::global()->loadIcon( "null", KIconLoader::Small );
	
// 	const int size = KIcon::SizeSmallMedium;
// 	const int size = 22;
	const int size = 16;
	
	if ( m_icon_full.width() == size && m_icon_full.height() == size )
	{
		m_icon_16 = m_icon_full;
		return;
	}
	
	QImage im = m_icon_full.toImage();
	//im = im.smoothScale( size, size, Qt::ScaleMin ); // 2018.11.30
    im = im.scaled( QSize( size, size ), Qt::KeepAspectRatio, Qt::SmoothTransformation );
	m_icon_16.convertFromImage(im);
}

QString LibraryItem::activeID( ) const
{
	return m_idList.isEmpty() ? "" : m_idList[0];
}
