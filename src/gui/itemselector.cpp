/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <vector> // Temporay fix for pthread.h problem
#include "circuitdocument.h"
#include "docmanager.h"
#include "flowcodedocument.h"
#include "itemdocument.h"
#include "itemlibrary.h"
#include "itemselector.h"
#include "libraryitem.h"
#include "mechanicsdocument.h"

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <klocale.h>
#include <kconfiggroup.h>

#include <Qt/q3dragobject.h>
#include <Qt/qlayout.h>
#include <Qt/q3popupmenu.h>
#include <Qt/qwhatsthis.h>

#include <cassert>

ILVItem::ILVItem( K3ListView* parent, const QString &id )
	: K3ListViewItem( parent, 0 )
{
	m_id = id;
	b_isRemovable = false;
	m_pProjectItem = 0l;
}

ILVItem::ILVItem( K3ListViewItem* parent, const QString &id )
	: K3ListViewItem( parent, 0 )
{
	m_id = id;
	b_isRemovable = false;
	m_pProjectItem = 0l;
}


ItemSelector::ItemSelector( QWidget *parent, const char *name )
	: K3ListView( parent /*, name */ )
{
    addColumn( i18n( "Component" ) );
	setFullWidth(true);
	setSorting( -1, false );
    setRootIsDecorated(true);
    setDragEnabled(true);
	setFocusPolicy( Qt::NoFocus );
	
// 	connect( this, SIGNAL(executed(K3ListViewItem*) ), this, SLOT(slotItemExecuted(K3ListViewItem*)) );
	connect( this, SIGNAL(clicked(Q3ListViewItem*)), this, SLOT(slotItemClicked(Q3ListViewItem*)) );
	connect( this, SIGNAL(doubleClicked(Q3ListViewItem*)), this, SLOT(slotItemDoubleClicked(Q3ListViewItem*)) );
	connect( this, SIGNAL(contextMenuRequested(Q3ListViewItem*, const QPoint&, int )), this,
             SLOT(slotContextMenuRequested(Q3ListViewItem*, const QPoint&, int )) );
	connect( this, SIGNAL(selectionChanged(Q3ListViewItem*)), this, SLOT(slotItemSelected( Q3ListViewItem* )) );
}

ItemSelector::~ItemSelector()
{
	writeOpenStates();
}


void ItemSelector::clear()
{
	m_categories.clear();
	K3ListView::clear();
}


void ItemSelector::addItem( const QString & caption, const QString & id, const QString & _category, const QPixmap & icon, bool removable )
{
	ILVItem *parentItem = 0L;
	
	QString category = _category;
	if ( !category.startsWith("/") ) {
		category.prepend('/');
	}
	
	do
	{
		category.remove(0,1);
		QString cat;
		category.replace( "\\/", "|" );
		int pos = category.find('/');
		if ( pos == -1 ) cat = category;
		else cat = category.left( pos );
		
		cat.replace( "|", "/" );
	
		if ( m_categories.findIndex(cat) == -1 )
		{
			m_categories.append(cat);
			
			if (parentItem) {
				parentItem = new ILVItem( parentItem, "" );
			}
			else {
				parentItem = new ILVItem( this, "" );
			}
			parentItem->setOpen( readOpenState(cat) );
			
			parentItem->setExpandable(true);
			parentItem->setText( 0, cat );
		}
		else
		{
			parentItem = (ILVItem*)findItem( cat, 0 );
		}
		
		category.remove( 0, pos );
	} while ( category.contains('/') );
	
	if ( !parentItem )
	{
		kdError() << "Unexpected error in finding parent item for category list"<<endl;
		return;
	}
	
	ILVItem *item = new ILVItem( parentItem, id );
	item->setPixmap( 0, icon );
	item->setText( 0, caption );
	item->setRemovable(removable);
}


void ItemSelector::writeOpenStates()
{
	//KConfig *config = kapp->config();
    KSharedConfigPtr configPtr = KGlobal::config();
	//config->setGroup( name() );
    KConfigGroup configGroup = configPtr->group( name() );
	
	const QStringList::iterator end = m_categories.end();
	for ( QStringList::iterator it = m_categories.begin(); it != end; ++it )
	{
		Q3ListViewItem *item = findItem( *it, 0 );
		if (item) {
			configGroup.writeEntry( *it+"IsOpen", item->isOpen() );
		}
	}
}


bool ItemSelector::readOpenState( const QString &id )
{
	//KConfig *config = kapp->config();
    KSharedConfigPtr configPtr = KGlobal::config();
	//config->setGroup( name() );
    KConfigGroup configGroup = configPtr->group( name() );
	
	return configGroup.readEntry<bool>( id+"IsOpen", true );
}


void ItemSelector::slotContextMenuRequested( Q3ListViewItem* item, const QPoint& pos, int /*col*/ )
{
	if ( !item || !(static_cast<ILVItem*>(item))->isRemovable() ) {
		return;
	}
	
	Q3PopupMenu *menu = new Q3PopupMenu(this);
	menu->insertItem( i18n("Remove %1").arg(item->text(0)), this, SLOT(slotRemoveSelectedItem()), Qt::Key_Delete );
	menu->popup(pos);
}


void ItemSelector::slotRemoveSelectedItem()
{
	ILVItem *item = dynamic_cast<ILVItem*>(selectedItem());
	if (!item)
		return;
	
	emit itemRemoved( item->key( 0, 0 ) );
	ILVItem *parent = dynamic_cast<ILVItem*>(item->K3ListViewItem::parent());
	delete item;
	// Get rid of the category as well if it has no children
	if ( parent && !parent->firstChild() )
	{
		m_categories.remove(parent->text(0));
		delete parent;
	}
}


void ItemSelector::setListCaption( const QString &caption )
{
	setColumnText( 0, caption );
}


void ItemSelector::slotItemSelected( Q3ListViewItem * item )
{
	if (!item)
		return;
	
	emit itemSelected( item->key( 0, 0 ) );
}


void ItemSelector::slotItemClicked( Q3ListViewItem *item )
{
	if (!item)
		return;
	
	if ( ItemDocument * itemDocument = dynamic_cast<ItemDocument*>(DocManager::self()->getFocusedDocument()) )
		itemDocument->slotUnsetRepeatedItemId();
	
	emit itemClicked( item->key( 0, 0 ) );
}


void ItemSelector::slotItemDoubleClicked( Q3ListViewItem *item )
{
	if (!item)
		return;
	
	QString id = item->key( 0, 0 );
	
	if ( Document * doc = DocManager::self()->getFocusedDocument() )
	{
		if ( doc->type() == Document::dt_flowcode && id.startsWith("flow/") )
			(static_cast<FlowCodeDocument*>(doc))->slotSetRepeatedItemId(id);
		
		else if ( doc->type() == Document::dt_circuit && (id.startsWith("ec/") || id.startsWith("sc/")) )
			(static_cast<CircuitDocument*>(doc))->slotSetRepeatedItemId(id);
		
		else if ( doc->type() == Document::dt_mechanics && id.startsWith("mech/") )
			(static_cast<MechanicsDocument*>(doc))->slotSetRepeatedItemId(id);
	}
	
	emit itemDoubleClicked(id);
}


Q3DragObject* ItemSelector::dragObject()
{
	const QString id = currentItem()->key(0,0);
	
	Q3StoredDrag * d = 0l;
	
	if ( id.startsWith("flow/") )
		d = new Q3StoredDrag( "ktechlab/flowpart", this );
	
	else if ( id.startsWith("ec/") )
		d = new Q3StoredDrag( "ktechlab/component", this );
	
	else if ( id.startsWith("sc/") )
		d = new Q3StoredDrag( "ktechlab/subcircuit", this );
	
	else if ( id.startsWith("mech/") )
		d = new Q3StoredDrag( "ktechlab/mechanical", this );
	
	if (d)
	{
		QByteArray data;
		QDataStream stream( &data, IO_WriteOnly );
		stream << id;
		d->setEncodedData(data);
	}
	
	// A pixmap cursor is often hard to make out
// 	QPixmap *pixmap = const_cast<QPixmap*>(currentItem()->pixmap(0));
// 	if (pixmap) d->setPixmap(*pixmap);

    return d;
}



//BEGIN class ComponentSelector
ComponentSelector * ComponentSelector::m_pSelf = 0l;


ComponentSelector * ComponentSelector::self( KateMDI::ToolView * parent )
{
	if (!m_pSelf)
	{
		assert(parent);
		m_pSelf = new ComponentSelector(parent);
	}
	return m_pSelf;
}


ComponentSelector::ComponentSelector( KateMDI::ToolView * parent )
	: ItemSelector( (QWidget*)parent, "Component Selector" )
{
	QWhatsThis::add( this, i18n(
			"Add components to the circuit diagram by dragging them into the circuit.<br><br>"
			
			"To add more than one component of the same type, doubleclick on a component, and click repeatedly in the circuit to place the component. Right click to stop placement.<br><br>"
			
			"Some components (such as subcircuits) can be removed by right clicking on the item and selecting \"Remove\"."
							   ) );
	
	setListCaption( i18n("Component") );
	
	LibraryItemList *items = itemLibrary()->items();
	const LibraryItemList::iterator end = items->end();
	for ( LibraryItemList::iterator it = items->begin(); it != end; ++it )
	{
		if ( (*it)->type() == LibraryItem::lit_component )
			addItem( (*it)->name(), (*it)->activeID(), (*it)->category(), (*it)->icon16() );
	}
}
//END class ComponentSelector



//BEGIN class FlowPartSelector
FlowPartSelector * FlowPartSelector::m_pSelf = 0l;


FlowPartSelector * FlowPartSelector::self( KateMDI::ToolView * parent )
{
	if (!m_pSelf)
	{
		assert(parent);
		m_pSelf = new FlowPartSelector(parent);
	}
	return m_pSelf;
}


FlowPartSelector::FlowPartSelector( KateMDI::ToolView * parent )
	: ItemSelector( (QWidget*)parent, "Part Selector" )
{
	QWhatsThis::add( this, i18n("Add FlowPart to the FlowCode document by dragging them there.<br><br>To add more than one FlowPart of the same type, doubleclick on a FlowPart, and click repeatedly in the FlowChart to place the component. Right click to stop placement.") );
	
	setListCaption( i18n("Flow Part") );
	
	LibraryItemList *items = itemLibrary()->items();
	const LibraryItemList::iterator end = items->end();
	for ( LibraryItemList::iterator it = items->begin(); it != end; ++it )
	{
		if ( (*it)->type() == LibraryItem::lit_flowpart )
			addItem( (*it)->name(), (*it)->activeID(), (*it)->category(), (*it)->icon16() );
	}
}
//END class FlowPartSelector


//BEGIN class MechanicsSelector
MechanicsSelector * MechanicsSelector::m_pSelf = 0l;


MechanicsSelector * MechanicsSelector::self( KateMDI::ToolView * parent )
{
	if (!m_pSelf)
	{
		assert(parent);
		m_pSelf = new MechanicsSelector( (QWidget*)parent );
	}
	return m_pSelf;
}


MechanicsSelector::MechanicsSelector( QWidget *parent )
	: ItemSelector( (QWidget*)parent, "Mechanics Selector" )
{
	QWhatsThis::add( this, i18n("Add mechanical parts to the mechanics work area by dragging them there.") );
	
	LibraryItemList *items = itemLibrary()->items();
	const LibraryItemList::iterator end = items->end();
	for ( LibraryItemList::iterator it = items->begin(); it != end; ++it )
	{
		if ( (*it)->type() == LibraryItem::lit_mechanical )
		{
			addItem( (*it)->name(), (*it)->activeID(), (*it)->category(), (*it)->icon16() );
		}
	}
}
//END class MechanicsSelector


#include "itemselector.moc"
