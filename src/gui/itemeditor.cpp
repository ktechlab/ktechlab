/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cnitem.h"
#include "cnitemgroup.h"
#include "itemeditor.h"
#include "orientationwidget.h"
#include "propertieslistview.h"

#include <klocale.h>
#include <kstandarddirs.h>

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwhatsthis.h>

#include <cassert>

ItemEditor * ItemEditor::m_pSelf = 0;

ItemEditor * ItemEditor::self( KateMDI::ToolView * parent)
{
	if(!m_pSelf)
	{
		assert(parent);
		m_pSelf = new ItemEditor(parent);
	}
	return m_pSelf;
}


ItemEditor::ItemEditor( KateMDI::ToolView * parent)
	: QWidget( (QWidget*)parent, "Item Editor")
{
	QWhatsThis::add( this, i18n("This allows editing of advanced properties of the selected item(s). Right click on the picture of the item to set the orientation."));
	
	QVBoxLayout *vlayout = new QVBoxLayout( this, 0, 6);

	m_nameLbl = new QLabel( this, "");
	vlayout->addWidget(m_nameLbl);
	vlayout->addSpacing(8);

	propList = new PropertiesListView(this);
	vlayout->addWidget(propList);
	QWhatsThis::add(propList,i18n("<qt>Shows properties associated with the currently selected item(s).<p>Select a property to change its value. If multiple items are selected with different values then the property will appear greyed out, use ""Merge Properties"" to make them the same.<p>Select ""Defaults to set all properties to their default values""")); 
	
	QHBoxLayout *h1Layout = new QHBoxLayout( vlayout, 4);
	QSpacerItem *spacer1 = new QSpacerItem( 1, 1);
	h1Layout->addItem(spacer1);
	
	m_defaultsBtn = new QPushButton( i18n("Defaults"), this);
	m_defaultsBtn->setEnabled(false);
	connect(m_defaultsBtn,SIGNAL(clicked()),propList,SLOT(slotSetDefaults()));
	h1Layout->addWidget(m_defaultsBtn);
	
	m_mergeBtn = new QPushButton( i18n("Merge properties"), this);
	m_mergeBtn->setEnabled(false);
	connect(m_mergeBtn,SIGNAL(clicked()),this,SLOT(mergeProperties()));
	h1Layout->addWidget(m_mergeBtn);
	
	// Orientation widget stuff
	QHBoxLayout *h2Layout = new QHBoxLayout( vlayout, 6);
	QSpacerItem *spacer2 = new QSpacerItem( 1, 1);
	h2Layout->addItem(spacer2);
	m_orientationWidget = new OrientationWidget(this);
	h2Layout->addWidget(m_orientationWidget);
	QWhatsThis::add(m_orientationWidget,i18n("Change the orientation of the selected item by selecting the appropriate button"));
	QSpacerItem *spacer3 = new QSpacerItem( 1, 1);
	h2Layout->addItem(spacer3);
	
	slotClear();
}


ItemEditor::~ItemEditor()
{
}


void ItemEditor::mergeProperties()
{
	propList->slotMergeProperties();
	m_mergeBtn->setEnabled(false);
}


void ItemEditor::slotClear()
{
	propList->slotClear();
	m_orientationWidget->slotClear();
	m_defaultsBtn->setEnabled(false);
	m_mergeBtn->setEnabled(false);
	updateNameLabel(0);
}


void ItemEditor::slotMultipleSelected()
{
	slotClear();
	m_nameLbl->setText( i18n("<h2>Multiple Items</h2>"));
}


void ItemEditor::slotUpdate( ItemGroup *itemGroup)
{
	if(!itemGroup) {
		slotClear();
		return;
	}
	
	updateMergeDefaults(itemGroup);
	propList->slotCreate(itemGroup);
	updateNameLabel(itemGroup->activeItem());
}


void ItemEditor::updateMergeDefaults( ItemGroup *itemGroup)
{
	if(!itemGroup)
	{
		m_defaultsBtn->setEnabled(false);
		m_mergeBtn->setEnabled(false);
		return;
	}
	
	m_mergeBtn->setEnabled( !itemGroup->itemsHaveSameData());
	m_defaultsBtn->setEnabled( !itemGroup->itemsHaveDefaultData());
	propList->slotUpdate(itemGroup);
}


void ItemEditor::slotUpdate( CNItem *item)
{
	m_orientationWidget->slotUpdate(item);
}


void ItemEditor::updateNameLabel( Item *item)
{
	if(item) {
		m_nameLbl->setText( "<h2>" + item->name() + "</h2>");
	} else {
		m_nameLbl->setText( i18n("<h2>No Item Selected</h2>"));
	}
}


#include "itemeditor.moc"
