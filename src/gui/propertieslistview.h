/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROPERTIESLISTVIEW_H
#define PROPERTIESLISTVIEW_H

#include <klistview.h>
#include <qmap.h>

class CNItemGroup;
class ItemGroup;
class PIEditor;
class PLVItem;
class QPushButton;

typedef QMap< QString, PLVItem * > PLVItemMap;

/**
@author Daniel Clarke
@author David Saxton
*/
class PropertiesListView : public KListView
{
Q_OBJECT
public:
	PropertiesListView( QWidget *parent = 0, const char *name = 0 );
	~PropertiesListView();
	
public slots:
	/**
	 * Creates a new set of property items and inserts them into the widget.
	 */
	void slotCreate( ItemGroup * itemGroup );
	/**
	 * Updates the set of property items (enables/disables according to whether
	 * the items have differing values, etc). This will be called by slotCreate,
	 * and any time new items are selected / unselected.
	 */
	void slotUpdate( ItemGroup * itemGroup );
	/**
	 * Removes all property items from the widget.
	 */
	void slotClear();
	void slotSelectionChanged(QListViewItem *item);
	/**
	 * Whenthe selected items have different values, then the property editor
	 * for the disagreeing property will have a value taken from one of the
	 * items, but will also be disabled. This will enable all disabled property
	 * editors and set the items to the value contained.
	 */
	void slotMergeProperties();
	void slotSetDefaults();
	
protected slots:
	void headerSizeChanged(int section, int oldSize, int newSize);
	void slotDataChanged(const QString &id, QVariant data);
	void destroyEditor();
	
protected:
	virtual void wheelEvent( QWheelEvent *e );
	
	PIEditor *m_editor;
	QPushButton *m_diffBt;
	PLVItem *p_lastItem;
	PLVItemMap m_plvItemMap;
};

#endif
