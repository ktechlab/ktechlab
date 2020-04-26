/***************************************************************************
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ITEMEDITOR_H
#define ITEMEDITOR_H

#include <QWidget>
#include <QPointer>

class ComponentModelWidget;
class CNItem;
class CNItemGroup;
class CNItemGroup;
class ICNDocument;
class Item;
class ItemEditor;
class ItemGroup;
class OrientationWidget;
class PropertyEditor;
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
		static ItemEditor * self( KateMDI::ToolView * parent = nullptr );
		~ItemEditor() override;
		static QString toolViewIdentifier() { return "ItemEditor"; }
	
	public slots:
		/**
		 * Update the Properties Editor
		 */
		void slotUpdate( ItemGroup * itemGroup );
		/**
		 * Updates various widgets (orientation and component-model ).
		 */
		void slotUpdate( Item * item );
		/**
		 * Clear the properties editor and orientation widget
		 */
		void slotClear();
		void slotMultipleSelected();
		/**
		 * Updates the merge / reset data parts (e.g. enabling or disabling the
		 * "Merge" button)
		 */
		void itemGroupUpdated( ItemGroup * itemGroup );
	
	protected:
		void updateNameLabel( Item * item );
		
		PropertyEditor * m_pPropertyEditor;
		QLabel * m_pNameLabel;
		OrientationWidget * m_pOrientationWidget;
		ComponentModelWidget * m_pComponentModelWidget;
		
	private:
		static ItemEditor * m_pSelf;
		ItemEditor( KateMDI::ToolView * parent );
};


#endif
