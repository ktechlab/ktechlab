/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ITEMINTERFACE_H
#define ITEMINTERFACE_H

#include <QPointer>
#include <QMap>

#include "itemgroup.h"

class CNItemGroup;
class DoubleSpinBox;
class Item;
class ItemDocument;
class ItemGroup;
class ItemInterface;
class KTechlab;
class MechanicsGroup;
class Variant;

class ColorCombo;
class KComboBox;
class KToolBar;
class KUrlRequester;
class QCheckBox;
class LineEdit;
class QSpinBox;


typedef QMap<QString, Variant*> VariantDataMap;
typedef QMap<QString, KComboBox*> KComboBoxMap;
typedef QMap<QString, LineEdit*> LineEditMap;
typedef QMap<QString, DoubleSpinBox*> DoubleSpinBoxMap;
typedef QMap<QString, QSpinBox*> IntSpinBoxMap;
typedef QMap<QString, ColorCombo*> ColorComboMap;
typedef QMap<QString, KUrlRequester*> KUrlReqMap;
typedef QMap<QString, QCheckBox*> QCheckBoxMap;

/**
This acts as an interface between the ItemDocument's and the associated
Items's, and the various objects that like to know about them
(e.g. ContextHelp, ItemEditor, ItemEditTB)
@author David Saxton
*/
class ItemInterface : public QObject
{
	Q_OBJECT
	public:
		~ItemInterface() override;
		static ItemInterface * self();
		/**
		 * The current item group in use (or null if none).
		 */
		ItemGroup * itemGroup() const { return p_itemGroup; }
		/**
		 * Sets the orientation of all flowparts in the group.
		 */
		void setFlowPartOrientation( unsigned orientation );
		/**
		 * Sets the orientation of all components in the group.
		 */
		void setComponentOrientation( int angleDegrees, bool flipped );
		/**
		 * Updates actions based on the items currently selected (e.g. rotate,
		 * flip, etc)
		 */
		void updateItemActions();
		/**
		 * Returns a configuration widget for the component for insertion into te
		 * ItemEditTB.
		 */
		virtual QWidget * configWidget();
	
	public slots:
		/**
		 * If cnItemsAreSameType() returns true, then set the
		 * data with the given id for all the CNItems in the group.
		 * Else, it only sets the data for the activeCNItem()
		 */
		void slotSetData( const QString &id, QVariant value );
		/**
		 * Essentially the same as slotSetData.
		 */
		void setProperty( Variant * v );
		/**
		 * Called when the ItemEditTB is cleared. This clears all widget maps.
		 */
		void itemEditTBCleared();
		void tbDataChanged();
		void slotItemDocumentChanged( ItemDocument *view );
		void slotUpdateItemInterface();
		void slotClearAll();
		void slotMultipleSelected();
		void clearItemEditorToolBar();
	
	protected:
		/**
		 * Connects the specified widget to either tbDataChanged or advDataChanged
		 * as specified by mi.
		 */
		void connectMapWidget( QWidget *widget, const char *_signal);
	
		// Widget maps.
		LineEditMap m_stringLineEditMap;
		KComboBoxMap m_stringComboBoxMap;
		KUrlReqMap m_stringURLReqMap;
		DoubleSpinBoxMap m_doubleSpinBoxMap;
		IntSpinBoxMap m_intSpinBoxMap;
		ColorComboMap m_colorComboMap;
		QCheckBoxMap m_boolCheckMap;
		
		// Use by item editor toolbar
		QPointer<KToolBar> m_pActiveItemEditorToolBar;
		int m_toolBarWidgetID;
		
		
	protected slots:
		void slotGetActionTicket();

	private:
		ItemInterface();
		static ItemInterface * m_pSelf;
	
		QPointer<ItemDocument> p_cvb;
		QPointer<ItemGroup> p_itemGroup;
		QPointer<Item> p_lastItem;
		int m_currentActionTicket;
        bool m_isInTbDataChanged;
};

#endif
