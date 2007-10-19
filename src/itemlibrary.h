/***************************************************************************
 *   Copyright (C) 2003 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ITEMLIBRARY_H
#define ITEMLIBRARY_H

#include <qcolor.h>
#include <qmap.h>

class Document;
class Item;
class ItemDocument;
class ItemLibrary;
class LibraryItem;
inline ItemLibrary* itemLibrary();

typedef QMap< QString, QImage > ImageMap;
typedef QValueList<LibraryItem*> LibraryItemList;

/**
While the program is running, only one instance of this class is created.
You can get it by calling itemLibrary()
@short Holds the list of CNItems
@author David Saxton
*/
class ItemLibrary
{
public:
	~ItemLibrary();
	/**
	 * Returns a QPixmap of the item icon
	 */
	QPixmap itemIconFull( const QString &id);
	/**
	 * Append the given item into the library
	 */
	void addLibraryItem( LibraryItem *item);
	/**
	 * Returns a list of items in the library
	 */
	LibraryItemList* items() { return &m_items; }
	/**
	 * Creates a new item with the given id, and returns a pointer to it
	 */
	Item *createItem( const QString &id, ItemDocument *itemDocument, bool newItem, const char *newId = 0, bool finishCreation = true);
	/**
	 * Returns an image of the given cnitem. As QPixmap::convertToImage is
	 * a *very* slow function, this will cache the result and return that for
	 * large images.
	 * @param cnItem A pointer to the CNItem
	 * @param maxSize The maximum size (in pixels) before the image is cached
	 */
	QImage itemImage( Item *item, const uint maxSize = 36000);
	
protected:
	void addComponents();
	void addFlowParts();
	void addMechanics();
	void addDrawParts();
	
	ItemLibrary();
	
	LibraryItemList m_items;
	ImageMap m_imageMap;
	
	friend ItemLibrary* itemLibrary();
};

inline ItemLibrary* itemLibrary()
{
	static ItemLibrary *_itemLibrary = new ItemLibrary();
	return _itemLibrary;
}

#endif
