/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuitview.h"
#include "colorcombo.h"
#include "contexthelp.h"
#include "cnitem.h"
#include "cnitemgroup.h"
#include "doublespinbox.h"
#include "itemdocument.h"
#include "itemeditor.h"
#include "iteminterface.h"
#include "itemview.h"
#include "ktechlab.h"

#include <kcombobox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kurlrequester.h>
#include <ktoolbar.h>
#include <qapplication.h>
#include <qlabel.h>
#include <qcheckbox.h>

#include <cassert>

ItemInterface * ItemInterface::m_pSelf = 0;

ItemInterface * ItemInterface::self( KTechlab * ktechlab)
{
	if( !m_pSelf)
	{
		assert(ktechlab);
		m_pSelf = new ItemInterface(ktechlab);
	}
	return m_pSelf;
}


ItemInterface::ItemInterface( KTechlab * ktechlab)
	: QObject(ktechlab),
	p_ktechlab(ktechlab)
{
	m_pActiveItemEditorToolBar = 0;
	p_cvb = 0;
	p_itemGroup = 0;
	p_lastItem = 0;
	m_currentActionTicket = -1;
	m_toolBarWidgetID = -1;
}


ItemInterface::~ItemInterface()
{
}


void ItemInterface::slotGetActionTicket()
{
	m_currentActionTicket = p_cvb ? p_cvb->getActionTicket() : -1;
}


void ItemInterface::slotItemDocumentChanged( ItemDocument * doc)
{
	slotClearAll();
	if( ItemDocument * itemDocument = dynamic_cast<ItemDocument*>((Document*)p_cvb))
	{
		disconnect( itemDocument, SIGNAL(itemSelected(Item*)), this, SLOT(slotUpdateItemInterface()));
		disconnect( itemDocument, SIGNAL(itemUnselected(Item*)), this, SLOT(slotUpdateItemInterface()));
	}
	
	p_itemGroup = 0;
	p_cvb = doc;
	
	slotGetActionTicket();
	
	if(!p_cvb)
		return;
	
	connect( p_cvb, SIGNAL(itemSelected(Item*)), this, SLOT(slotUpdateItemInterface()));
	connect( p_cvb, SIGNAL(itemUnselected(Item*)), this, SLOT(slotUpdateItemInterface()));
	
	p_itemGroup = p_cvb->selectList();
	
	slotUpdateItemInterface();
}


void ItemInterface::clearItemEditorToolBar()
{
	if( m_pActiveItemEditorToolBar && m_toolBarWidgetID != -1)
		m_pActiveItemEditorToolBar->removeItem(m_toolBarWidgetID);
	m_toolBarWidgetID = -1;
	itemEditTBCleared();
}


void ItemInterface::slotClearAll()
{
	ContextHelp::self()->slotClear();
	ItemEditor::self()->slotClear();
	clearItemEditorToolBar();
	p_lastItem = 0;
}


void ItemInterface::slotMultipleSelected()
{
	ContextHelp::self()->slotMultipleSelected();
	ItemEditor::self()->slotMultipleSelected();
	clearItemEditorToolBar();
	p_lastItem = 0;
}


void ItemInterface::slotUpdateItemInterface()
{
	if(!p_itemGroup)
		return;
	
	slotGetActionTicket();
	updateItemActions();
	
	if(!p_itemGroup->itemsAreSameType())
	{
		slotMultipleSelected();
		return;
	}
	if( p_lastItem && p_itemGroup->activeItem())
	{
		ItemEditor::self()->updateMergeDefaults(p_itemGroup);
		return;
	}
	
	p_lastItem = p_itemGroup->activeItem();
	if(!p_lastItem)
	{
		slotClearAll();
		return;
	}
	
	ContextHelp::self()->slotUpdate(p_lastItem);
	ItemEditor::self()->slotUpdate(p_itemGroup);
	if( CNItem * cnItem = dynamic_cast<CNItem*>((Item*)p_lastItem))
		ItemEditor::self()->slotUpdate(cnItem);
	
	// Update item editor toolbar
	if( ItemView * itemView = dynamic_cast<ItemView*>(p_cvb->activeView()))
	{
		if( m_pActiveItemEditorToolBar = dynamic_cast<KToolBar*>(p_ktechlab->factory()->container("itemEditorTB",itemView)))
		{
			m_pActiveItemEditorToolBar->setFullSize( true);
			QWidget * widget = configWidget();
			m_toolBarWidgetID = 1;
			m_pActiveItemEditorToolBar->insertWidget( m_toolBarWidgetID, 0, widget);
		}
	}
}


void ItemInterface::updateItemActions()
{
	ItemView * itemView = ((ItemDocument*)p_cvb) ? dynamic_cast<ItemView*>(p_cvb->activeView()) : 0;
	if( !itemView)
		return;
	
	bool itemsSelected = p_itemGroup && p_itemGroup->itemCount();
	
	itemView->action("edit_raise")->setEnabled(itemsSelected);
	itemView->action("edit_lower")->setEnabled(itemsSelected);
	p_ktechlab->action("edit_cut")->setEnabled(itemsSelected);
	p_ktechlab->action("edit_copy")->setEnabled(itemsSelected);
	
	CNItemGroup * cnItemGroup = dynamic_cast<CNItemGroup*>((ItemGroup*)p_itemGroup);
	CircuitView * circuitView = dynamic_cast<CircuitView*>(itemView);
	
	if( cnItemGroup && circuitView)
	{
		circuitView->action("edit_flip")->setEnabled(cnItemGroup->canFlip());
		bool canRotate = cnItemGroup->canRotate();
		circuitView->action("edit_rotate_ccw")->setEnabled(canRotate);
		circuitView->action("edit_rotate_cw")->setEnabled(canRotate);
	}
}


void ItemInterface::setFlowPartOrientation( unsigned orientation)
{
	CNItemGroup *cnItemGroup = dynamic_cast<CNItemGroup*>((ItemGroup*)p_itemGroup);
	if(!cnItemGroup)
		return;
	
	cnItemGroup->setFlowPartOrientation( orientation);
}


void ItemInterface::setComponentOrientation( int angleDegrees, bool flipped)
{
	CNItemGroup *cnItemGroup = dynamic_cast<CNItemGroup*>((ItemGroup*)p_itemGroup);
	if(!cnItemGroup)
		return;
	
	cnItemGroup->setComponentOrientation( angleDegrees, flipped);
}


void ItemInterface::itemEditTBCleared()
{
	m_stringLineEditMap.clear();
	m_stringComboBoxMap.clear();
	m_stringURLReqMap.clear();
	m_intSpinBoxMap.clear();
	m_doubleSpinBoxMap.clear();
	m_colorComboMap.clear();
	m_boolCheckMap.clear();
}


// The bool specifies whether advanced data should be shown
QWidget * ItemInterface::configWidget()
{
	if( !p_itemGroup || !p_itemGroup->activeItem() || !m_pActiveItemEditorToolBar)
		return 0;
	
	VariantDataMap *variantMap = p_itemGroup->activeItem()->variantMap();
	
	QWidget * parent = m_pActiveItemEditorToolBar;
	
	// Create new widget with the toolbar or dialog as the parent
	QWidget * configWidget = new QWidget( parent, "tbConfigWidget");
	configWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding, 1, 1));
	
	QHBoxLayout * configLayout = new QHBoxLayout( configWidget);
// 	configLayout->setAutoAdd( true);
	configLayout->setSpacing( 6);
	
// 	configLayout->addItem( new QSpacerItem( 0, 0,  QSizePolicy::Expanding, QSizePolicy::Fixed));
	
	const VariantDataMap::iterator vaEnd = variantMap->end();
	for( VariantDataMap::iterator vait = variantMap->begin(); vait != vaEnd; ++vait)
	{
		if( vait.data()->isHidden() || vait.data()->isAdvanced())
			continue;
		
		const Variant::Type::Value type = vait.data()->type();
		
		// common to all types apart from bool
		QString toolbarCaption = vait.data()->toolbarCaption();
		if( type != Variant::Type::Bool && !toolbarCaption.isEmpty())
			configLayout->addWidget( new QLabel( toolbarCaption, configWidget));
		
		QWidget * editWidget = 0; // Should be set to the created widget
		
		switch( type)
		{
			case Variant::Type::Port:
			case Variant::Type::Pin:
			case Variant::Type::VarName:
			case Variant::Type::Combo:
			case Variant::Type::Select:
			case Variant::Type::KeyPad:
			case Variant::Type::SevenSegment:
			{
				QString value = vait.data()->value().toString();
				if( !value.isEmpty() && !vait.data()->allowed().contains(value))
					vait.data()->appendAllowed(value);
				
				const QStringList allowed = vait.data()->allowed();
				
				KComboBox * box = new KComboBox(configWidget);
				
				box->insertStringList(allowed);
				box->setCurrentItem(value);
				
				if( type == Variant::Type::VarName || type == Variant::Type::Combo)
					box->setEditable( true);
				
				m_stringComboBoxMap[vait.key()] = box;
				connectMapWidget( box, SIGNAL(textChanged(const QString &)));
				connectMapWidget( box, SIGNAL(activated(const QString &)));
					
				editWidget = box;
				break;
			}
			case Variant::Type::FileName:
			{
				QString value = vait.data()->value().toString();
				if( !vait.data()->allowed().contains(value))
					vait.data()->appendAllowed(value);
				
				const QStringList allowed = vait.data()->allowed();
				
				KURLComboRequester * urlreq = new KURLComboRequester( configWidget);
				urlreq->setFilter( vait.data()->filter());
				connectMapWidget( urlreq, SIGNAL(urlSelected(const QString &)));
				m_stringURLReqMap[vait.key()] = urlreq;
				
				KComboBox * box = urlreq->comboBox();
				box->insertStringList(allowed);
				box->setEditable( true);
				
				// Note this has to be called after inserting the allowed list
				urlreq->setURL( vait.data()->value().toString());
				
				// Generally we only want a file name once the user has finished typing out the full file name.
				connectMapWidget( box, SIGNAL(returnPressed(const QString &)));
				connectMapWidget( box, SIGNAL(activated(const QString &)));
				
				editWidget = urlreq;
				break;
			}
			case Variant::Type::String:
			{
				KLineEdit * edit = new KLineEdit( configWidget);
				
				edit->setText( vait.data()->value().toString());
				connectMapWidget(edit,SIGNAL(textChanged(const QString &)));
				m_stringLineEditMap[vait.key()] = edit;
				editWidget = edit;
				break;
			}
			case Variant::Type::Int:
			{
				KIntSpinBox *spin = new KIntSpinBox( (int)vait.data()->minValue(), (int)vait.data()->maxValue(), 1, vait.data()->value().toInt(), 10, configWidget);
				
				connectMapWidget( spin, SIGNAL(valueChanged(int)));
				m_intSpinBoxMap[vait.key()] = spin;
				editWidget = spin;
				break;
			}
			case Variant::Type::Double:
			{
				DoubleSpinBox *spin = new DoubleSpinBox( vait.data()->minValue(), vait.data()->maxValue(), vait.data()->minAbsValue(), vait.data()->value().toDouble(), vait.data()->unit(), configWidget);
				
				connectMapWidget( spin, SIGNAL(valueChanged(double)));
				m_doubleSpinBoxMap[vait.key()] = spin;
				editWidget = spin;
				break;
			}
			case Variant::Type::Color:
			{
				QColor value = vait.data()->value().toColor();
				
				ColorCombo * colorBox = new ColorCombo( (ColorCombo::ColorScheme)vait.data()->colorScheme(), configWidget);
				
				colorBox->setColor( value);
				connectMapWidget( colorBox, SIGNAL(activated(const QColor &)));
				m_colorComboMap[vait.key()] = colorBox;
				
				editWidget = colorBox;
				break;
			}
			case Variant::Type::Bool:
			{
				const bool value = vait.data()->value().toBool();
				QCheckBox * box = new QCheckBox( vait.data()->toolbarCaption(), configWidget);
				
				box->setChecked(value);
				connectMapWidget( box, SIGNAL(toggled(bool)));
				m_boolCheckMap[vait.key()] = box;
				editWidget = box;
				break;
			}
			case Variant::Type::Raw:
			case Variant::Type::PenStyle:
			case Variant::Type::PenCapStyle:
			case Variant::Type::Multiline:
			case Variant::Type::None:
			{
				// Do nothing, as these data types are not handled in the toolbar
				break;
			}
		}
		
		if( !editWidget)
			continue;
		
		// In the case of the toolbar, we don't want it too high
		if( editWidget->height() > parent->height()-2)
			editWidget->setMaximumHeight( parent->height()-2);
		
		switch ( type)
		{
			case Variant::Type::VarName:
			case Variant::Type::Combo:
			case Variant::Type::String:
			{
				QSizePolicy p( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed, 1, 1);
			
				editWidget->setSizePolicy( p);
				editWidget->setMaximumWidth( 250);
				break;
			}
			
			case Variant::Type::FileName:
			case Variant::Type::Port:
			case Variant::Type::Pin:
			case Variant::Type::Select:
			case Variant::Type::KeyPad:
			case Variant::Type::SevenSegment:
			case Variant::Type::Int:
			case Variant::Type::Double:
			case Variant::Type::Color:
			case Variant::Type::Bool:
			case Variant::Type::Raw:
			case Variant::Type::PenStyle:
			case Variant::Type::PenCapStyle:
			case Variant::Type::Multiline:
			case Variant::Type::None:
				break;
		}
		
		configLayout->addWidget( editWidget);
	}
	
	configLayout->addItem( new QSpacerItem( 0, 0,  QSizePolicy::Expanding, QSizePolicy::Fixed));
	
	return configWidget;
}


void ItemInterface::connectMapWidget( QWidget *widget, const char *_signal)
{
	connect( widget, _signal, this, SLOT(tbDataChanged()));
}


void ItemInterface::tbDataChanged()
{
	// Manual string values
	const KLineEditMap::iterator m_stringLineEditMapEnd = m_stringLineEditMap.end();
	for( KLineEditMap::iterator leit = m_stringLineEditMap.begin(); leit != m_stringLineEditMapEnd; ++leit)
	{
		slotSetData( leit.key(), leit.data()->text());
	}
	
	// String values from comboboxes
	const KComboBoxMap::iterator m_stringComboBoxMapEnd = m_stringComboBoxMap.end();
	for( KComboBoxMap::iterator cmit = m_stringComboBoxMap.begin(); cmit != m_stringComboBoxMapEnd; ++cmit)
	{
		slotSetData( cmit.key(), cmit.data()->currentText());
	}

	// Colors values from colorcombos
	const ColorComboMap::iterator m_colorComboMapEnd = m_colorComboMap.end();
	for( ColorComboMap::iterator ccit = m_colorComboMap.begin(); ccit != m_colorComboMapEnd; ++ccit)
	{
		slotSetData( ccit.key(), ccit.data()->color());
	}
	
	// Bool values from checkboxes
	const QCheckBoxMap::iterator m_boolCheckMapEnd = m_boolCheckMap.end();
	for( QCheckBoxMap::iterator chit = m_boolCheckMap.begin(); chit != m_boolCheckMapEnd; ++chit)
	{
		slotSetData( chit.key(), chit.data()->isChecked());
	}
	
	const IntSpinBoxMap::iterator m_intSpinBoxMapEnd = m_intSpinBoxMap.end();
	for( IntSpinBoxMap::iterator it = m_intSpinBoxMap.begin(); it != m_intSpinBoxMapEnd; ++it)
	{
		slotSetData( it.key(), it.data()->value());
	}
	
	// (?) Combined values from spin boxes and combo boxes
	// (?) Get values from all spin boxes
	
	const DoubleSpinBoxMap::iterator m_doubleSpinBoxMapEnd = m_doubleSpinBoxMap.end();
	for( DoubleSpinBoxMap::iterator sbit = m_doubleSpinBoxMap.begin(); sbit != m_doubleSpinBoxMapEnd; ++sbit)
	{
// 		VariantDataMap::iterator vait = variantData.find(sbit.key());
		slotSetData( sbit.key(), sbit.data()->value());
	}
	
	// Filenames from KURLRequesters
	const KURLReqMap::iterator m_stringURLReqMapEnd = m_stringURLReqMap.end();
	for( KURLReqMap::iterator urlit = m_stringURLReqMap.begin(); urlit != m_stringURLReqMapEnd; ++urlit)
	{
		slotSetData( urlit.key(), urlit.data()->url());
	}
	
	if(p_cvb)
		p_cvb->setModified(true);
}




void ItemInterface::slotSetData( const QString &id, QVariant value)
{
	if( !p_itemGroup || (p_itemGroup->itemCount() == 0))
		return;
	
	if( !p_itemGroup->itemsAreSameType())
	{
		kdDebug() << k_funcinfo << "Items are not the same type!"<<endl;
		return;
	}
	
	const ItemList itemList = p_itemGroup->items(true);
	const ItemList::const_iterator end = itemList.end();
	for( ItemList::const_iterator it = itemList.begin(); it != end; ++it)
	{
		if(*it)
			(*it)->property(id)->setValue(value);
	}
	if(p_cvb)
		p_cvb->setModified(true);
	
	
	VariantDataMap * variantMap = (*itemList.begin())->variantMap();
	VariantDataMap::iterator it = variantMap->find(id);
	if( it == variantMap->end())
		return;
	
	
	// setData might have been called from the PropertiesListView, so want
	// to see if the toolbar widgets want setting
	
	switch( it.data()->type())
	{
		case Variant::Type::String:
		{
			KLineEditMap::iterator mit = m_stringLineEditMap.find(id);
			if( mit != m_stringLineEditMap.end()) mit.data()->setText( it.data()->value().toString());
			break;
		}
		case Variant::Type::FileName:
		{
			KURLReqMap::iterator mit = m_stringURLReqMap.find(id);
			if( mit != m_stringURLReqMap.end()) mit.data()->setURL( it.data()->value().toString());
			break;
		}
		case Variant::Type::PenCapStyle:
		case Variant::Type::PenStyle:
		case Variant::Type::Port:
		case Variant::Type::Pin:
		case Variant::Type::VarName:
		case Variant::Type::Combo:
		case Variant::Type::Select:
		case Variant::Type::SevenSegment:
		case Variant::Type::KeyPad:
		{
			KComboBoxMap::iterator mit = m_stringComboBoxMap.find(id);
			if( mit != m_stringComboBoxMap.end()) mit.data()->setCurrentItem( it.data()->value().toString());
			break;
		}
		case Variant::Type::Int:
		{
			IntSpinBoxMap::iterator mit = m_intSpinBoxMap.find(id);
			if( mit != m_intSpinBoxMap.end()) {
				KIntSpinBox *sb = mit.data();
				sb->setValue( it.data()->value().toInt());
			}
			break;
		}
		case Variant::Type::Double:
		{
			DoubleSpinBoxMap::iterator mit = m_doubleSpinBoxMap.find(id);
			if( mit != m_doubleSpinBoxMap.end()) {
				DoubleSpinBox *sb = mit.data();
				sb->setValue( it.data()->value().toDouble());
			}
			break;
		}
		case Variant::Type::Color:
		{
			ColorComboMap::iterator mit = m_colorComboMap.find(id);
			if( mit != m_colorComboMap.end()) mit.data()->setColor( it.data()->value().toColor());
			break;
		}
		case Variant::Type::Bool:
		{
			QCheckBoxMap::iterator mit = m_boolCheckMap.find(id);
			if( mit != m_boolCheckMap.end()) mit.data()->setChecked( it.data()->value().toBool());
			break;
		}
		case Variant::Type::Raw:
		case Variant::Type::Multiline:
		case Variant::Type::None:
		{
			// This data will never be handled in the toolbar/PLV, so no need to worry about it
			break;
		}
	}
	
	ItemEditor::self()->updateMergeDefaults(p_itemGroup);
	
	if(p_cvb)
		p_cvb->requestStateSave(m_currentActionTicket);
}

#include "iteminterface.moc"
