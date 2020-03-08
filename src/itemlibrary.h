/***************************************************************************
 *   Copyright (C) 2003-2006 David Saxton <david@bluehaze.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ITEMLIBRARY_H
#define ITEMLIBRARY_H

#include <klocalizedstring.h>

#include <qcolor.h>
#include <qobject.h>
#include <qmap.h>

class Component;
class Document;
class Item;
class ItemDocument;
class ItemLibrary;
class LibraryItem;
inline ItemLibrary* itemLibrary();

typedef QMap< QString, QString > QStringMap;
typedef QMap< QString, QStringMap > QStringMapMap;
typedef QMap< QString, QImage > ImageMap;
typedef QList<LibraryItem*> LibraryItemList;

/**
While the program is running, only one instance of this class is created.
You can get it by calling itemLibrary()
@short Holds the list of CNItems
@author David Saxton
*/
class ItemLibrary : public QObject
{
	Q_OBJECT
	public:
		~ItemLibrary() override;
		/**
		 * Returns a QPixmap of the item icon
		 */
		QPixmap itemIconFull( const QString &id );
		/**
		 * Append the given item into the library
		 */
		void addLibraryItem( LibraryItem *item );
		/**
		 * Returns a list of items in the library
		 */
		LibraryItemList* items() { return &m_items; }
		/**
		 * @return the LibraryItem for the item with the given type (id) const.
		 */
		LibraryItem * libraryItem( QString type ) const;
		/**
		 * Creates a new item with the given id, and returns a pointer to it
		 */
		Item * createItem( const QString &id, ItemDocument * itemDocument, bool newItem, const char *newId = nullptr, bool finishCreation = true );
		/**
		 * Returns an image of the given component. As QPixmap::toImage is
		 * a slow function, this will cache the result and return that for large
		 * images.
		 * @param component A pointer to the Component.
		 * @param maxSize The maximum size (in pixels) before the image is
		 * cached.
		 */
		QImage componentImage( Component * component, const uint maxSize = 36000 );
		/**
		 * Does similar to that above, but will not be able to return a description
		 * if there is none saved on file (instead of the above, which falls back to
		 * calling item->description()).
		 * @param type the id of the item.
		 * @param language the language code, e.g. "es".
		 */
		QString description( QString type, const QString & language ) const;
		/**
		 * @return if we have a description for the item in language.
		 */
		bool haveDescription( QString type, const QString & language ) const;
		/**
		 * Gives the type item the description.
		 * @param type the type of item this description is for.
		 * @param language the language code, e.g. "es".
		 * @return whether the descriptions file could be saved.
		 */
		bool setDescription( QString type, const QString & description, const QString & language );
		/** 
		 * @return the directory containing the item descriptions. By default,
		 * this is something like "/usr/share/apps/ktechlab/contexthelp/". But
		 * can be changed by calling setDescriptionsDirectory.
		 */
		QString itemDescriptionsDirectory() const;
		/** 
		 * Stores the item descriptions directory in the users config.
		 */
		void setItemDescriptionsDirectory( QString dir );
		/**
		 * @return the item description file for the given language.
		 */
		QString itemDescriptionsFile( const QString & language ) const;
		/**
		 * @return the string used for an empty item description - something like
		 * "The help for English does not yet exist..". This is created by inserting
		 * the current language name into m_emptyItemDescription;
	 	*/
		QString emptyItemDescription( const QString & language ) const;
		/**
		 * @return the list of language-codes that have item descriptions.
		 */
		QStringList descriptionLanguages() const { return m_itemDescriptions.keys(); }
	
	protected:
		/**
		 * Saves the item descriptions to the file specified in the config.
		 * @return whether successful (e.g. if the file could be opened for
		 * writing).
		 */
		bool saveDescriptions( const QString & language );
		void loadItemDescriptions();
		void addComponents();
		void addFlowParts();
		void addMechanics();
		void addDrawParts();
	
		ItemLibrary();
	
		LibraryItemList m_items;
		ImageMap m_imageMap;
		QStringMapMap m_itemDescriptions; // (Language, type) <--> description
		static KLocalizedString m_emptyItemDescription; // Description template for when a description does not yet exist
	
		friend ItemLibrary * itemLibrary();
};

inline ItemLibrary* itemLibrary()
{
	// are we really sure we aren't calling new over and over again? 
	static ItemLibrary *_itemLibrary = new ItemLibrary();
	return _itemLibrary;
}

#endif
