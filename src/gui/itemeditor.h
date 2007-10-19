/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ITEMEDITOR_H
#define ITEMEDITOR_H

#include <qwidget.h>
#include <qguardedptr.h>

class ItemEditor;

class CNItem;
class CNItemGroup;
class Item;
class CNItemGroup;
class ICNDocument;
class ItemGroup;
class OrientationWidget;
class PropertiesListView;
class QPushButton;
class QLabel;

namespace KateMDI { class ToolView; }

/**
@author Daniel Clarke
@author David Saxton
*/
class ItemEditor : public QWidget
{
	Q_OBJECT
	public:
		static ItemEditor * self( KateMDI::ToolView * parent = 0);
		~ItemEditor();
		static QString toolViewIdentifier() { return "ItemEditor"; }
	
	public slots:
		/**
		 * Update the Properties Editor
		 */
		void slotUpdate( ItemGroup *itemGroup);
		/**
		 * Update the orientation widget
		 */
		void slotUpdate( CNItem *item);
		/**
		 * Clear the properties editor and orientation widget
		 */
		void slotClear();
		void slotMultipleSelected();
		/**
		 * Updates the merge / reset data parts (e.g. enabling or disabling the
		 * "Defaults" button)
		 */
		void updateMergeDefaults( ItemGroup *itemGroup);
	
	protected:
		void updateNameLabel( Item *item);
		PropertiesListView * propList;
		static ItemEditor * m_pSelf;
	
	private slots:
		void mergeProperties();
		
	private:
		ItemEditor( KateMDI::ToolView * parent);
	
		QLabel *m_nameLbl;
		QPushButton *m_defaultsBtn;
		QPushButton *m_mergeBtn;
		OrientationWidget *m_orientationWidget;
};


#endif
