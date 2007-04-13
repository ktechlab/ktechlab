/***************************************************************************
 *   Copyright (C) 2003,2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ITEMSELECTOR_H
#define ITEMSELECTOR_H

#include <klistview.h>

#include <qpixmap.h>
#include <qstring.h>

class ProjectItem;
class QStoredDrag;
namespace KateMDI { class ToolView; }

/**
@short Contains info about item for ItemSelector
@author David Saxton
*/
class ILVItem : public QObject, public KListViewItem
{
	public:
		ILVItem( QListView *parent, const QString &id );
		ILVItem( QListViewItem *parent, const QString &id );
		
		void setProjectItem( ProjectItem * projectItem ) { m_pProjectItem = projectItem; }
		ProjectItem * projectItem() const { return m_pProjectItem; }
		
		QString id() const { return m_id; }
	
		QString key( int, bool ) const { return m_id; }
		/**
		 * Set whether the item can be removed from the listview by the user
		 */
		void setRemovable( bool isRemovable ) { b_isRemovable = isRemovable; }
		/**
		 * Whether the item can be removed from the listview by the user
		 */
		bool isRemovable() const { return b_isRemovable; }
	
	protected:
		QString m_id;
		bool b_isRemovable;
		ProjectItem * m_pProjectItem;
};

/**
@short Allows selection of generic items for dragging / clicking
@author David Saxton
*/
class ItemSelector : public KListView
{
	Q_OBJECT
	public:
		ItemSelector( QWidget *parent, const char *name );
		~ItemSelector();
		/**
		 * Adds a listview item to the ListView
		 * @param caption The displayed text
		 * @param id A unique identification for when it is dragged or activated
		 * @param category The category it is in, eg "Integrated Circuits
		 * @param icon The icon to be displayed to the left of the text
		 * @param removable Whether the user can right-click on the item and select Remove
		 */
		void addItem( const QString & caption, const QString & id, const QString & category, const QPixmap & icon = QPixmap(), bool removable = false );
	
	public slots:
		virtual void slotContextMenuRequested( QListViewItem *item, const QPoint &pos, int col );
		virtual void clear();
		void slotRemoveSelectedItem();
	
	signals:
		/**
		 * Emitted when a user selects an item and removes it
		 */
		void itemRemoved( const QString &id );
		void itemDoubleClicked( const QString &id );
		void itemClicked( const QString &id );
	
	protected:
		/**
		 * Sets the caption of the ListView (eg 'Components' or 'Files')
		 */
		void setListCaption( const QString &caption );
		/**
		 * Writes the open status (folded or unfolded) of "parent" items in the view
		 * to the config file.
		 */
		void writeOpenStates();
		/**
		 * Reads the open status (folded or unfolded) of the given item. The default
		 * status for non-existant items is true.
		 */
		bool readOpenState( const QString &id );

	private slots:
		void slotItemClicked( QListViewItem *item );
		void slotItemDoubleClicked( QListViewItem *item );

	private:
		/**
		 * @return a dragobject encoding the currently selected component item.
		 */
		QDragObject * dragObject();
	
		QStringList m_categories;
};


/**
@short Allows selection of electrical components
@author David Saxton
 */
class ComponentSelector : public ItemSelector
{
	Q_OBJECT
	public:
		static ComponentSelector * self( KateMDI::ToolView * parent = 0 );
		static QString toolViewIdentifier() { return "ComponentSelector"; }
	
	private:
		ComponentSelector( KateMDI::ToolView * parent );
		static ComponentSelector * m_pSelf;
};


/**
@short Allows selection of PIC parts (eg 'Pause')
@author David Saxton
 */
class FlowPartSelector : public ItemSelector
{
	Q_OBJECT
	public:
		static FlowPartSelector * self( KateMDI::ToolView * parent = 0 );
		static QString toolViewIdentifier() { return "FlowPartSelector"; }
	
	private:
		FlowPartSelector( KateMDI::ToolView * parent );
		static FlowPartSelector * m_pSelf;
};


/**
@author David Saxton
 */
class MechanicsSelector : public ItemSelector
{
	Q_OBJECT
	public:
		static MechanicsSelector * self( KateMDI::ToolView * parent = 0 );
		static QString toolViewIdentifier() { return "MechanicsSelector"; }
	
	private:
		MechanicsSelector( QWidget *parent = 0 );
		static MechanicsSelector * m_pSelf;
};


#endif
