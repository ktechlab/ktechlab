/***************************************************************************
 *   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>                     *
 *   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>            *
 *   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>                     *
 *   Copyright (C) 2006 by David Saxton david@bluehaze.org                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <qvariant.h>
#include <qpointer.h>
#include <qtablewidget.h>

// #include <q3dict.h>
//#include <q3listview.h> // 2018.08.13 - ported to QTableWidget


#include "propertyeditoritem.h"

class ItemGroup;
class Variant;
class PropertySubEditor;
class KPushButton;
class QStyledItemDelegate;

//! A list view to edit any type of properties
class PropertyEditor : public QTableWidget // K3ListView
{
	Q_OBJECT

	public:
		/*! Creates an empty PropertyEditor with \a parent as parent widget.
		*/
		PropertyEditor( QWidget * parent = 0, const char * name = 0 );
		virtual ~PropertyEditor();

		/*! Reset the list, ie clears all items in the list.
		   if \a editorOnly is true, then only the current editor will be cleared, not the whole list.
		*/
		void reset();

		/**
		 * Updates the list of Property editors from the items selected in
		 * \a itemGroup.
		 */
		void create( ItemGroup * itemGroup );

		virtual QSize sizeHint() const;
		/**
		 * @internal used by PropertySubEditor and PropertyEditor.
		 */
		bool handleKeyPress( QKeyEvent* ev );
		/**
		 * Updates the default button for the current editor.
		 */
		void updateDefaultsButton();

	public slots:
		/**
		 * On focus:
		 * \li previously focused editor is activated
		 * \li first visible item is activated if no item was active
		*/
		virtual void setFocus();

	protected slots:
		/**
		 * This slot resets the value of an item, using Property::oldValue().
		 * It is called when pressing the "Revert to defaults" button
		 */
		void resetItem();
		/**
		 * This slot updates the positions of current editor and revert button.
		 * It is called when double-clicking list's header. 
		 */
		void moveEditor();
		/**
		 * Fills the list with an item for each property in the buffer.
		 * You shouldn't need to call this, as it is automatically called in create().
		 */
		void fill();
		/**
		 * This slot updates editor and revert buttons position and size when
		 * the columns are resized.
		 */
		void slotColumnSizeChanged( int section, int oldS, int newS);
		void slotColumnSizeChanged( int section);
		/**
		 * This slot is called when the user clicks the list view. It takes care
		 * of deleting current editor and creating a new editor for the newly
		 * selected item.
		 */
		void slotClicked(const QModelIndex& index);
		/**
		 * Used to fix selection when unselectable item was activated.
		 */
		void slotCurrentChanged(QTableWidgetItem *);

		void slotExpanded(QTableWidgetItem *item);
		void slotCollapsed(QTableWidgetItem *item);

	protected:
		/**
		 * Creates an editor for the list item \a i in the rect \a geometry, and
		 * displays revert button if property is modified (ie
		 * PropertyEditorItem::modified() == true). The editor type depends on
		 * Property::type() of the item's property.
		*/
		void createEditor(const QModelIndex& index);//, const QRect &geometry);
		/**
		 * Reimplemented from K3ListView to update editor and revert button
		 * position.
		 */
		void resizeEvent(QResizeEvent *ev);

		void showDefaultsButton( bool show );

		int baseRowHeight() const { return m_baseRowHeight; }

		PropertyEditorItem *selectedItem() ;

		QPointer<ItemGroup> m_pItemGroup;
		QPointer<PropertySubEditor> m_currentEditor;
		PropertyEditorItem *m_editItem;
		PropertyEditorItem *m_topItem; //The top item is used to control the drawing of every branches.
		KPushButton *m_defaults; // "Revert to defaults" button
		//PropertyEditorItem::Dict m_items; // 2018.08.13 - unused
		int m_baseRowHeight;
		//! Used in setFocus() to prevent scrolling to previously selected item on mouse click
		bool justClickedItem;

        QStyledItemDelegate *m_colPropertyDelegate;
        QStyledItemDelegate *m_colValueDelegate;

		friend class PropertyEditorItem;
		friend class PropertySubEditor;
};

#endif
