/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef LIBRARYITEM_H
#define LIBRARYITEM_H

#include "item.h"

class QStringList;

/**
This holds details of an item - id, name, category it is displayed in in its
respective item selector, icon, function pointers to creating the item, etc.
Normally each item will only pass one id, but some items have had their IDs
changed during the history of ktl, so passing a stringlist will take the first
ID as the "active" id, and the rest as IDs that will also be recognized, but
never displayed to the user.
@short Details of an Item
@author David Saxton
*/
class LibraryItem
{
	public:
		~LibraryItem();
	
		enum Type
		{
			lit_flowpart,
			lit_component,
			lit_mechanical,
			lit_drawpart,
			lit_subcircuit,
			lit_other
		};
		LibraryItem( QStringList idList, const QString &name, const QString &category, QPixmap icon, Type type, createItemPtr createItem );
		LibraryItem( QStringList idList, const QString &name, const QString &category, const QString &iconName, Type type, createItemPtr createItem );
		LibraryItem( QStringList idList, const QString &name, const QString &category, Type type, createItemPtr createItem );
	
		QString activeID() const;
		QStringList allIDs() const { return m_idList; }
		QString name() const { return m_name; }
		QString category() const { return m_category; }
		QPixmap iconFull() const { return m_icon_full; }
		QPixmap icon16() const { return m_icon_16; }
		createItemPtr createItemFnPtr() const { return createItem; }
		int type() const { return m_type; }
	
	protected:
		void createIcon16();
	
	private:
		QStringList m_idList;
		QString m_name;
		QString m_category;
		QPixmap m_icon_full;
		QPixmap m_icon_16;
		createItemPtr createItem;
		int m_type;
};
typedef QList<LibraryItem*> LibraryItemList;

#endif
