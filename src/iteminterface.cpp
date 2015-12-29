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
#include "lineedit.h"

#include <kcombobox.h>
#include <kdebug.h>
#include <klineedit.h>
#include <knuminput.h>
#include <kurlrequester.h>
#include <ktoolbar.h>
#include <kxmlguifactory.h>

#include <Qt/qapplication.h>
#include <Qt/qlabel.h>
#include <Qt/qcheckbox.h>

#include <cassert>

ItemInterface * ItemInterface::m_pSelf = 0l;

ItemInterface * ItemInterface::self()
{
	if ( !m_pSelf )
		m_pSelf = new ItemInterface();
	
	return m_pSelf;
}


ItemInterface::ItemInterface()
	: QObject( KTechlab::self() )
{
	m_pActiveItemEditorToolBar = 0;
	p_cvb = 0l;
	p_itemGroup = 0l;
	p_lastItem = 0l;
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


void ItemInterface::slotItemDocumentChanged( ItemDocument * doc )
{
	slotClearAll();
	if ( ItemDocument * itemDocument = dynamic_cast<ItemDocument*>((Document*)p_cvb) )
	{
		disconnect( itemDocument, SIGNAL(selectionChanged()), this, SLOT(slotUpdateItemInterface()) );
	}
	
	p_itemGroup = 0l;
	p_cvb = doc;
	
	slotGetActionTicket();
	
	if (!p_cvb)
		return;
	
	connect( p_cvb, SIGNAL(selectionChanged()), this, SLOT(slotUpdateItemInterface()) );
	
	p_itemGroup = p_cvb->selectList();
	
	slotUpdateItemInterface();
}


void ItemInterface::clearItemEditorToolBar()
{
	if ( m_pActiveItemEditorToolBar && m_toolBarWidgetID != -1 ) {
		//m_pActiveItemEditorToolBar->removeItem(m_toolBarWidgetID); // TODO add proper replacmenet
        m_pActiveItemEditorToolBar->clear();
    }
	m_toolBarWidgetID = -1;
	itemEditTBCleared();
}


void ItemInterface::slotClearAll()
{
	ContextHelp::self()->slotClear();
	ItemEditor::self()->slotClear();
	clearItemEditorToolBar();
	p_lastItem = 0l;
}


void ItemInterface::slotMultipleSelected()
{
	ContextHelp::self()->slotMultipleSelected();
	ItemEditor::self()->slotMultipleSelected();
	clearItemEditorToolBar();
	p_lastItem = 0l;
}


void ItemInterface::slotUpdateItemInterface()
{
	if (!p_itemGroup)
		return;
	
	slotGetActionTicket();
	updateItemActions();
	
	if (!p_itemGroup->itemsAreSameType() )
	{
		slotMultipleSelected();
		return;
	}
	if ( p_lastItem && p_itemGroup->activeItem() )
	{
		ItemEditor::self()->itemGroupUpdated( p_itemGroup );
		return;
	}
	
	p_lastItem = p_itemGroup->activeItem();
	if (!p_lastItem)
	{
		slotClearAll();
		return;
	}
	
	ContextHelp::self()->slotUpdate(p_lastItem);
	ItemEditor::self()->slotUpdate(p_itemGroup);
	if ( CNItem * cnItem = dynamic_cast<CNItem*>((Item*)p_lastItem) )
	{
		ItemEditor::self()->slotUpdate(cnItem);
	}
	
	// Update item editor toolbar
	if ( ItemView * itemView = dynamic_cast<ItemView*>(p_cvb->activeView()) )
	{
		if ( KTechlab * ktl = KTechlab::self() )
		{
			if ( m_pActiveItemEditorToolBar = dynamic_cast<KToolBar*>(ktl->factory()->container("itemEditorTB",itemView)) )
			{
				//m_pActiveItemEditorToolBar->setFullSize( true ); // TODO proper replacement
                m_pActiveItemEditorToolBar->adjustSize();
				QWidget * widget = configWidget();
				m_toolBarWidgetID = 1;
				// m_pActiveItemEditorToolBar->insertWidget( m_toolBarWidgetID, 0, widget ); // TODO properly fix
                m_pActiveItemEditorToolBar->addWidget( widget );
			}
		}
	}
}


void ItemInterface::updateItemActions()
{
	ItemView * itemView = ((ItemDocument*)p_cvb) ? dynamic_cast<ItemView*>(p_cvb->activeView()) : 0l;
	if ( !itemView )
		return;
	
	bool itemsSelected = p_itemGroup && p_itemGroup->itemCount();
	
	itemView->actionByName("edit_raise")->setEnabled(itemsSelected);
	itemView->actionByName("edit_lower")->setEnabled(itemsSelected);
	
	if ( KTechlab::self() )
	{
		KTechlab::self()->actionByName("edit_cut")->setEnabled(itemsSelected);
		KTechlab::self()->actionByName("edit_copy")->setEnabled(itemsSelected);
	}
	
	CNItemGroup * cnItemGroup = dynamic_cast<CNItemGroup*>((ItemGroup*)p_itemGroup);
	CircuitView * circuitView = dynamic_cast<CircuitView*>(itemView);
	
	if ( cnItemGroup && circuitView  )
	{
		bool canFlip = cnItemGroup->canFlip();
		circuitView->actionByName("edit_flip_horizontally")->setEnabled( canFlip );
		circuitView->actionByName("edit_flip_vertically")->setEnabled( canFlip );
		
		bool canRotate = cnItemGroup->canRotate();
		circuitView->actionByName("edit_rotate_ccw")->setEnabled( canRotate );
		circuitView->actionByName("edit_rotate_cw")->setEnabled( canRotate );
	}
}


void ItemInterface::setFlowPartOrientation( unsigned orientation )
{
	CNItemGroup *cnItemGroup = dynamic_cast<CNItemGroup*>((ItemGroup*)p_itemGroup);
	if (!cnItemGroup)
		return;
	
	cnItemGroup->setFlowPartOrientation( orientation );
}


void ItemInterface::setComponentOrientation( int angleDegrees, bool flipped )
{
	CNItemGroup *cnItemGroup = dynamic_cast<CNItemGroup*>((ItemGroup*)p_itemGroup);
	if (!cnItemGroup)
		return;
	
	cnItemGroup->setComponentOrientation( angleDegrees, flipped );
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
	if ( !p_itemGroup || !p_itemGroup->activeItem() || !m_pActiveItemEditorToolBar )
		return 0l;
	
	VariantDataMap *variantMap = p_itemGroup->activeItem()->variantMap();
	
	QWidget * parent = m_pActiveItemEditorToolBar;
	
	// Create new widget with the toolbar or dialog as the parent
	QWidget * configWidget = new QWidget( parent, "tbConfigWidget" );
	configWidget->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding, 1, 1 ) );
	
	QHBoxLayout * configLayout = new QHBoxLayout( configWidget );
// 	configLayout->setAutoAdd( true );
	configLayout->setSpacing( 6 );
	
// 	configLayout->addItem( new QSpacerItem( 0, 0,  QSizePolicy::Expanding, QSizePolicy::Fixed ) );
	
	const VariantDataMap::iterator vaEnd = variantMap->end();
	for ( VariantDataMap::iterator vait = variantMap->begin(); vait != vaEnd; ++vait )
	{
		if ( vait.data()->isHidden() || vait.data()->isAdvanced() )
			continue;
		
		const Variant::Type::Value type = vait.data()->type();
		
		// common to all types apart from bool
		QString toolbarCaption = vait.data()->toolbarCaption();
		if ( type != Variant::Type::Bool && !toolbarCaption.isEmpty() )
			configLayout->addWidget( new QLabel( toolbarCaption, configWidget ) );
		
		QWidget * editWidget = 0l; // Should be set to the created widget
		
		switch( type )
		{
			case Variant::Type::Port:
			case Variant::Type::Pin:
			case Variant::Type::VarName:
			case Variant::Type::Combo:
			case Variant::Type::Select:
			case Variant::Type::KeyPad:
			case Variant::Type::SevenSegment:
			{
				QString value = vait.data()->displayString();
				if ( !value.isEmpty() && !vait.data()->allowed().contains(value) )
					vait.data()->appendAllowed(value);
				
				const QStringList allowed = vait.data()->allowed();
				
				KComboBox * box = new KComboBox(configWidget);
				
				box->insertStringList(allowed);
				box->setCurrentItem(value);
				
				if ( type == Variant::Type::VarName || type == Variant::Type::Combo )
					box->setEditable( true );
				
				m_stringComboBoxMap[vait.key()] = box;
				connectMapWidget( box, SIGNAL(editTextChanged(const QString &)));
				connectMapWidget( box, SIGNAL(activated(const QString &)));
				
				connect( *vait, SIGNAL(valueChanged(const QString &)), box, SLOT(setCurrentItem(const QString &)) );
				
				editWidget = box;
				break;
			}
			case Variant::Type::FileName:
			{
				QString value = vait.data()->value().toString();
				if ( !vait.data()->allowed().contains(value) )
					vait.data()->appendAllowed(value);
				
				const QStringList allowed = vait.data()->allowed();
				
				KUrlComboRequester * urlreq = new KUrlComboRequester( configWidget );
				urlreq->setFilter( vait.data()->filter() );
				connectMapWidget( urlreq, SIGNAL(urlSelected(const KUrl &)) );
				m_stringURLReqMap[vait.key()] = urlreq;
				
				KComboBox * box = urlreq->comboBox();
				box->insertStringList(allowed);
				box->setEditable( true );
				
				// Note this has to be called after inserting the allowed list
				urlreq->setUrl( vait.data()->value().toString() );
				
				// Generally we only want a file name once the user has finished typing out the full file name.
				connectMapWidget( box, SIGNAL(returnPressed(const QString &)));
				connectMapWidget( box, SIGNAL(activated(const QString &)));
				
				connect( *vait, SIGNAL(valueChanged(const QString &)), box, SLOT(setEditText(const QString &)) );
				
				editWidget = urlreq;
				break;
			}
			case Variant::Type::String:
			{
				LineEdit * edit = new LineEdit( configWidget );
				
				edit->setText( vait.data()->value().toString() );
				connectMapWidget(edit,SIGNAL(textChanged(const QString &)));
				m_stringLineEditMap[vait.key()] = edit;
				editWidget = edit;
				
				connect( *vait, SIGNAL(valueChanged(const QString &)), edit, SLOT(setText(const QString &)) );
				
				break;
			}
			case Variant::Type::Int:
			{
				KIntSpinBox *spin = new KIntSpinBox( (int)vait.data()->minValue(), (int)vait.data()->maxValue(), 1, vait.data()->value().toInt(),
                                                     configWidget, 10 );
				
				connectMapWidget( spin, SIGNAL(valueChanged(int)) );
				m_intSpinBoxMap[vait.key()] = spin;
				editWidget = spin;
				
				connect( *vait, SIGNAL(valueChanged(int)), spin, SLOT(setValue(int)) );
				
				break;
			}
			case Variant::Type::Double:
			{
				DoubleSpinBox *spin = new DoubleSpinBox( vait.data()->minValue(), vait.data()->maxValue(), vait.data()->minAbsValue(), vait.data()->value().toDouble(), vait.data()->unit(), configWidget );
				
				connectMapWidget( spin, SIGNAL(valueChanged(double)));
				m_doubleSpinBoxMap[vait.key()] = spin;
				editWidget = spin;
				
				connect( *vait, SIGNAL(valueChanged(double)), spin, SLOT(setValue(double)) );
				
				break;
			}
			case Variant::Type::Color:
			{
				QColor value = vait.data()->value().value<QColor>();
				
				ColorCombo * colorBox = new ColorCombo( (ColorCombo::ColorScheme)vait.data()->colorScheme(), configWidget );
				
				colorBox->setColor( value );
				connectMapWidget( colorBox, SIGNAL(activated(const QColor &)));
				m_colorComboMap[vait.key()] = colorBox;
				
				connect( *vait, SIGNAL(valueChanged(const QColor &)), colorBox, SLOT(setColor(const QColor &)) );
				
				editWidget = colorBox;
				break;
			}
			case Variant::Type::Bool:
			{
				const bool value = vait.data()->value().toBool();
				QCheckBox * box = new QCheckBox( vait.data()->toolbarCaption(), configWidget );
				
				box->setChecked(value);
				connectMapWidget( box, SIGNAL(toggled(bool)));
				m_boolCheckMap[vait.key()] = box;
				
				connect( *vait, SIGNAL(valueChanged(bool)), box, SLOT(setChecked(bool)) );
				
				editWidget = box;
				break;
			}
			case Variant::Type::Raw:
			case Variant::Type::PenStyle:
			case Variant::Type::PenCapStyle:
			case Variant::Type::Multiline:
			case Variant::Type::RichText:
			case Variant::Type::None:
			{
				// Do nothing, as these data types are not handled in the toolbar
				break;
			}
		}
		
		if ( !editWidget )
			continue;
		
        const int widgetH = QFontMetrics( configWidget->font() ).height() + 2;
        editWidget->setMinimumHeight( widgetH );  // note: this is hack-ish; something is not ok with the layout
        editWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		// In the case of the toolbar, we don't want it too high
		if ( editWidget->height() > parent->height()-2 )
			editWidget->setMaximumHeight( parent->height()-2 );
		
		switch ( type )
		{
			case Variant::Type::VarName:
			case Variant::Type::Combo:
			case Variant::Type::String:
			{
				QSizePolicy p( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed, 1, 1 );
			
				editWidget->setSizePolicy( p );
				editWidget->setMaximumWidth( 250 );
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
			case Variant::Type::RichText:
			case Variant::Type::None:
				break;
		}
		
		configLayout->addWidget( editWidget );
	}
	
	configLayout->addItem( new QSpacerItem( 0, 0,  QSizePolicy::Expanding, QSizePolicy::Fixed ) );
	
	return configWidget;
}


void ItemInterface::connectMapWidget( QWidget *widget, const char *_signal )
{
	connect( widget, _signal, this, SLOT(tbDataChanged()) );
}


void ItemInterface::tbDataChanged()
{
    qDebug() << Q_FUNC_INFO;
	// Manual string values
	const LineEditMap::iterator m_stringLineEditMapEnd = m_stringLineEditMap.end();
	for ( LineEditMap::iterator leit = m_stringLineEditMap.begin(); leit != m_stringLineEditMapEnd; ++leit )
	{
		slotSetData( leit.key(), leit.data()->text() );
	}
	
	// String values from comboboxes
	const KComboBoxMap::iterator m_stringComboBoxMapEnd = m_stringComboBoxMap.end();
	for ( KComboBoxMap::iterator cmit = m_stringComboBoxMap.begin(); cmit != m_stringComboBoxMapEnd; ++cmit )
	{
        qDebug() << Q_FUNC_INFO << "set KCombo data for " << cmit.key() << " to " << cmit.data()->currentText();
		slotSetData( cmit.key(), cmit.data()->currentText() );
	}

	// Colors values from colorcombos
	const ColorComboMap::iterator m_colorComboMapEnd = m_colorComboMap.end();
	for ( ColorComboMap::iterator ccit = m_colorComboMap.begin(); ccit != m_colorComboMapEnd; ++ccit )
	{
		slotSetData( ccit.key(), ccit.data()->color() );
	}
	
	// Bool values from checkboxes
	const QCheckBoxMap::iterator m_boolCheckMapEnd = m_boolCheckMap.end();
	for ( QCheckBoxMap::iterator chit = m_boolCheckMap.begin(); chit != m_boolCheckMapEnd; ++chit )
	{
		slotSetData( chit.key(), chit.data()->isChecked() );
	}
	
	const IntSpinBoxMap::iterator m_intSpinBoxMapEnd = m_intSpinBoxMap.end();
	for ( IntSpinBoxMap::iterator it = m_intSpinBoxMap.begin(); it != m_intSpinBoxMapEnd; ++it )
	{
		slotSetData( it.key(), it.data()->value() );
	}
	
	// (?) Combined values from spin boxes and combo boxes
	// (?) Get values from all spin boxes
	
	const DoubleSpinBoxMap::iterator m_doubleSpinBoxMapEnd = m_doubleSpinBoxMap.end();
	for ( DoubleSpinBoxMap::iterator sbit = m_doubleSpinBoxMap.begin(); sbit != m_doubleSpinBoxMapEnd; ++sbit )
	{
// 		VariantDataMap::iterator vait = variantData.find(sbit.key());
		slotSetData( sbit.key(), sbit.data()->value() );
	}
	
	// Filenames from KUrlRequesters
	const KUrlReqMap::iterator m_stringURLReqMapEnd = m_stringURLReqMap.end();
	for ( KUrlReqMap::iterator urlit = m_stringURLReqMap.begin(); urlit != m_stringURLReqMapEnd; ++urlit )
	{
		slotSetData( urlit.key(), urlit.data()->url() );
	}
	
	if (p_cvb)
		p_cvb->setModified(true);
}


void ItemInterface::setProperty( Variant * v )
{
	slotSetData( v->id(), v->value() );
}


void ItemInterface::slotSetData( const QString &id, QVariant value )
{
	if ( !p_itemGroup || (p_itemGroup->itemCount() == 0) )
		return;
	
	if ( !p_itemGroup->itemsAreSameType() )
	{
		kDebug() << k_funcinfo << "Items are not the same type!"<<endl;
		return;
	}
	
	const ItemList itemList = p_itemGroup->items(true);
	const ItemList::const_iterator end = itemList.end();
	for ( ItemList::const_iterator it = itemList.begin(); it != end; ++it )
	{
		if (*it)
			(*it)->property(id)->setValue(value);
	}
	
	if (p_cvb)
		p_cvb->setModified(true);
	
	ItemEditor::self()->itemGroupUpdated( p_itemGroup );
	
	if (p_cvb)
		p_cvb->requestStateSave(m_currentActionTicket);
}

#include "iteminterface.moc"
