/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "item.h"
#include "cnitemgroup.h"
#include "itemgroup.h"
#include "iteminterface.h"
#include "pieditor.h"
#include "plvitem.h"
#include "propertieslistview.h"
#include "variant.h"


#include <kdebug.h>
#include <klocale.h>
#include <qevent.h>
#include <qheader.h>
#include <qpushbutton.h>
#include <qvariant.h>

PropertiesListView::PropertiesListView(QWidget *parent, const char *name)
 : KListView(parent, name)
{
	addColumn(i18n("Property"));
	addColumn(i18n("Data"));
	setFullWidth(true);
	setColumnAlignment(1,Qt::AlignRight);
	
	p_lastItem = 0;
	m_diffBt = 0;
	
	connect(this,SIGNAL(selectionChanged(QListViewItem*)),this,SLOT(slotSelectionChanged(QListViewItem*)));
	m_editor = 0;
	connect(header(),SIGNAL(sizeChange(int,int,int)),this,SLOT(headerSizeChanged(int,int,int)));
}

PropertiesListView::~PropertiesListView()
{
}

void PropertiesListView::slotClear()
{
	destroyEditor();
	delete m_diffBt;
	m_diffBt = 0;
	clear();
	m_plvItemMap.clear();
}

void PropertiesListView::slotCreate( ItemGroup * itemGroup )
{
	if ( !itemGroup || !itemGroup->activeItem() )
	{
		slotClear();
		return;
	}
	
	Item *item = itemGroup->activeItem();
	
	VariantDataMap *vmap = item->variantMap();
	// Build the list
	for( VariantDataMap::iterator vait = vmap->begin(); vait != vmap->end(); ++vait )
	{
		if ( vait.data()->isHidden() )
			continue;
		
		switch( vait.data()->type() )
		{
			case Variant::Type::Int:
			case Variant::Type::Double:
			case Variant::Type::String:
			case Variant::Type::FileName:
			case Variant::Type::Port:
			case Variant::Type::Pin:
			case Variant::Type::VarName:
			case Variant::Type::Combo:
			case Variant::Type::Select:
			case Variant::Type::Bool:
			case Variant::Type::PenStyle:
			case Variant::Type::PenCapStyle:
			case Variant::Type::SevenSegment:
			case Variant::Type::KeyPad:
			{
				m_plvItemMap[vait.key()] = new PLVItem( this, vait.key(), vait.data() );
				break;
			}
			case Variant::Type::Color:
			{
				m_plvItemMap[vait.key()] = new PLVColorItem( this, vait.key(), vait.data() );
				break;
			}
			case Variant::Type::Raw:
			case Variant::Type::Multiline:
			case Variant::Type::None:
			{
				break;
			}
		}
	}
	
	slotUpdate(itemGroup);
}


void PropertiesListView::slotUpdate( ItemGroup * itemGroup )
{
	if ( !itemGroup )
	{
		slotClear();
		return;
	}
	
	const PLVItemMap::iterator end = m_plvItemMap.end();
	for ( PLVItemMap::iterator it = m_plvItemMap.begin(); it != end; ++it )
	{
		it.data()->setEnabled( itemGroup->itemsHaveSameDataValue( it.key() ) );
	}
}

void PropertiesListView::slotSelectionChanged(QListViewItem *item)
{
	if (!item) return;
	destroyEditor();
	p_lastItem = dynamic_cast<PLVItem*>(item);
	if ( !p_lastItem->data() ) return;
	
	const Variant::Type::Value type = p_lastItem->data()->type();
	switch(type)
	{
		case Variant::Type::String:
		{
			m_editor = new PILineEdit(p_lastItem->id(),p_lastItem->data(),this);
			break;
		}
		case Variant::Type::Port:
		case Variant::Type::Pin:
		case Variant::Type::Combo:
		case Variant::Type::VarName:
		case Variant::Type::Select:
		case Variant::Type::PenStyle:
		case Variant::Type::PenCapStyle:
		case Variant::Type::SevenSegment:
		case Variant::Type::KeyPad:
		{
			m_editor = new PIStringCombo(p_lastItem->id(),p_lastItem->data(),this);
			break;
		}
		case Variant::Type::FileName:
		{
			m_editor = new PIFilename(p_lastItem->id(),p_lastItem->data(),this);
			break;
		}
		case Variant::Type::Int:
		{
			m_editor = new PIInt(p_lastItem->id(),p_lastItem->data(),this);
			break;
		}
		case Variant::Type::Double:
		{
			m_editor = new PIDouble(p_lastItem->id(),p_lastItem->data(),this);
			break;
		}
		case Variant::Type::Color:
		{
 			m_editor = new PIColor(p_lastItem->id(),p_lastItem->data(),this);
			break;
		}
		case Variant::Type::Bool:
		{
			m_editor = new PIBool(p_lastItem->id(),p_lastItem->data(),this);
			break;
		}
		case Variant::Type::Raw:
		case Variant::Type::Multiline:
		case Variant::Type::None:
		{
			break;
		}
	}
	
	connect(p_lastItem->data(),SIGNAL(destroyed()),this,SLOT(destroyEditor())); 
	// Connect so that changes in the editor change the canvas item data.
	connect(m_editor,SIGNAL(editorDataChanged(const QString&,QVariant)),ItemInterface::self(),SLOT(slotSetData(const QString&,QVariant)));
	connect(m_editor,SIGNAL(editorDataChanged(const QString&,QVariant)),this,SLOT(slotDataChanged(const QString&,QVariant)));
	
	int x = columnWidth(0);
	int y = viewportToContents(QPoint(0,itemRect(p_lastItem).y())).y();
	addChild(m_editor,x,y);
	m_editor->setFocus();
	m_editor->show();
	m_editor->setGeometry(QRect(x,y,columnWidth(1),itemRect(p_lastItem).height()));
	
	if(p_lastItem->data()->type() == Variant::Type::FileName)
	{
		// The folder button in the KURLComboBox has a minimum size taller than
		// the height of the ListViewItems so this is a temporary kludge to
		// make it look slightly acceptable.
		m_editor->setGeometry(QRect(x,y,columnWidth(1),itemRect(p_lastItem).height()+7));
	}
	
	// Active the editor as appropriate
	switch(type)
	{
		case Variant::Type::Port:
		case Variant::Type::Pin:
		case Variant::Type::Combo:
		case Variant::Type::VarName:
		case Variant::Type::Select:
		case Variant::Type::PenStyle:
		case Variant::Type::PenCapStyle:
		case Variant::Type::SevenSegment:
		case Variant::Type::KeyPad:
		{
			(static_cast<PIStringCombo*>(m_editor))->popup();
			break;
		}
		case Variant::Type::Color:
		{
			(static_cast<PIColor*>(m_editor))->popup();
			break;
		}
		case Variant::Type::Bool:
		{
			(static_cast<PIBool*>(m_editor))->popup();
			break;
		}
		case Variant::Type::FileName:
		{
			break;
		}
		case Variant::Type::Int:
		{
			break;
		}
		case Variant::Type::Double:
		{
			break;
		}
		case Variant::Type::String:
		{
			break;
		}
		case Variant::Type::Raw:
		case Variant::Type::Multiline:
		case Variant::Type::None:
		{
			break;
		}
	}
}

void PropertiesListView::destroyEditor()
{
	if( !m_editor ) return;
	
	removeChild( m_editor );
	delete m_editor;
	m_editor = 0;
}

void PropertiesListView::headerSizeChanged(int section, int /*oldSize*/, int newSize)
{
	if( !m_editor || section != 1 ) return;
	
	// Resize the editor to the new column width
	// and move it to the right place.
	QRect rect = m_editor->geometry();
	rect.setWidth(newSize);
	rect.setX( columnWidth(0) );
	m_editor->setGeometry(rect);
}

void PropertiesListView::slotDataChanged(const QString &/*id*/, QVariant data)
{
	PLVItem *pItem = static_cast<PLVItem*>(currentItem());
	pItem->updateData(data);
}

void PropertiesListView::slotMergeProperties()
{
	for( QListViewItemIterator it( this ); it.current(); ++it )
	{
		PLVItem * pItem = static_cast<PLVItem*>(it.current());
		if (pItem->isEnabled())
			continue;
		
		pItem->setEnabled(true);
		// manually call the updates on the canvas
		// and in the list
		pItem->updateData(pItem->data()->defaultValue());
		ItemInterface::self()->slotSetData(pItem->id(),pItem->data()->defaultValue());
	}
}


void PropertiesListView::slotSetDefaults()
{
	for( QListViewItemIterator it( this ); it.current(); ++it )
	{
		PLVItem *pItem = static_cast<PLVItem*>(it.current());
		ItemInterface::self()->slotSetData(pItem->id(),pItem->data()->defaultValue());
	}
}


void PropertiesListView::wheelEvent( QWheelEvent *e )
{
	QPoint _pos = contentsToViewport(e->pos());
	_pos -= pos();
	_pos.setY( _pos.y()+header()->height() );
	QListViewItem *item = itemAt(_pos);
	if ( item && item != dynamic_cast<QListViewItem*>(p_lastItem) )
	{
		e->accept();
		if(!item->isSelected()) slotSelectionChanged(item);
	}
	else KListView::wheelEvent(e);
}

#include "propertieslistview.moc"

