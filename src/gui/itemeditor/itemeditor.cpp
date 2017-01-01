/***************************************************************************
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cnitem.h"
#include "cnitemgroup.h"
#include "component.h"
#include "componentmodelwidget.h"
#include "itemeditor.h"
#include "orientationwidget.h"
#include "propertyeditor.h"
#include "katemdi.h"

#include <klocale.h>
#include <kstandarddirs.h>

#include <Qt/qlayout.h>
#include <Qt/qlabel.h>
#include <Qt/qpushbutton.h>
#include <Qt/qwhatsthis.h>
#include <Qt/qdebug.h>

#include <cassert>

ItemEditor * ItemEditor::m_pSelf = 0l;

ItemEditor * ItemEditor::self( KateMDI::ToolView * parent )
{
	if (!m_pSelf)
	{
		assert(parent);
		m_pSelf = new ItemEditor(parent);
	}
	return m_pSelf;
}


ItemEditor::ItemEditor( KateMDI::ToolView * parent )
	: QWidget( (QWidget*)parent, "Item Editor" )
{
	QWhatsThis::add( this, i18n("This allows editing of advanced properties of the selected item(s). Right click on the picture of the item to set the orientation.") );

    if (parent->layout()) {
        parent->layout()->addWidget(this);
        qDebug() << Q_FUNC_INFO << " added item selector to parent's layout " << parent;
    } else {
        qWarning() << Q_FUNC_INFO << " unexpected null layout on parent " << parent ;
    }

	QVBoxLayout * vlayout = new QVBoxLayout( this, 0, 6 );

	//BEGIN Create Name Label
	m_pNameLabel = new QLabel( this, "" );
	m_pNameLabel->setTextFormat( Qt::RichText );
	
	QFont font;
	font.setBold( true );
	if ( font.pointSize() != 0 )
		font.setPointSize( int(font.pointSize() * 1.4) );
	m_pNameLabel->setFont( font );
	//END Create Name Label

	m_pPropertyEditor = new PropertyEditor(this);
	QWhatsThis::add(m_pPropertyEditor,i18n("<p>Shows properties associated with the currently selected item(s).<br/>Select a property to change its value. If multiple items are selected with different values then the property will appear greyed out, use \"Merge Properties\" to make them the same.<br/>Select \"Defaults\" to set all properties to their default values")); 
	
	m_pComponentModelWidget = new ComponentModelWidget( this );
	
	
	vlayout->addWidget( m_pNameLabel );
	vlayout->addWidget( m_pPropertyEditor, 3 );
	vlayout->addWidget( m_pComponentModelWidget, 5 );
	
	
	// Orientation widget stuff
	QHBoxLayout *h2Layout = new QHBoxLayout( vlayout, 6 );
	h2Layout->addItem( new QSpacerItem( 1, 1 ) );
	m_pOrientationWidget = new OrientationWidget(this);
	h2Layout->addWidget(m_pOrientationWidget);
	QWhatsThis::add(m_pOrientationWidget,i18n("Change the orientation of the selected item by selecting the appropriate button"));
	h2Layout->addItem( new QSpacerItem( 1, 1 ) );
	
	slotClear();
}


ItemEditor::~ItemEditor()
{
}


void ItemEditor::slotClear()
{
	m_pPropertyEditor->reset( );
	m_pComponentModelWidget->reset();
	m_pOrientationWidget->slotClear();
	updateNameLabel(0l);
}


void ItemEditor::slotMultipleSelected()
{
	slotClear();
	m_pNameLabel->setText( i18n("Multiple Items") );
}


void ItemEditor::slotUpdate( ItemGroup * itemGroup )
{
	if (!itemGroup)
	{
		slotClear();
		return;
	}
	
	m_pPropertyEditor->create(itemGroup);
	updateNameLabel(itemGroup->activeItem());
	
	m_pOrientationWidget->slotUpdate( dynamic_cast<CNItemGroup *>(itemGroup) );
}


void ItemEditor::itemGroupUpdated( ItemGroup * itemGroup )
{
	m_pPropertyEditor->updateDefaultsButton();
	m_pOrientationWidget->slotUpdate( dynamic_cast<CNItemGroup *>(itemGroup) );
}


void ItemEditor::slotUpdate( Item * item )
{
	m_pComponentModelWidget->init( dynamic_cast<Component*>(item) );
}


void ItemEditor::updateNameLabel( Item *item )
{
	if (item)
		m_pNameLabel->setText( item->name() );
	
	else
		m_pNameLabel->setText( i18n("No Item Selected") );
}


#include "itemeditor.moc"
