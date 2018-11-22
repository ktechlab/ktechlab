/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "icndocument.h"
#include "item.h"
#include "itemgroup.h"
#include "mechanicsdocument.h"
#include "utils.h"

#include <qtimer.h>
#include <map>

ItemGroup::ItemGroup( ItemDocument *view, const char *name )
	: QObject( view, name )
{
	m_activeItem = 0l;
	b_itemsAreSameType = true;
	p_view = view;
	p_icnDocument = dynamic_cast<ICNDocument*>(p_view);
	p_mechanicsDocument = dynamic_cast<MechanicsDocument*>(p_view);
	QTimer::singleShot( 0, this, SLOT(getViewPtrs()) );
}


ItemGroup::~ItemGroup()
{
}


void ItemGroup::getViewPtrs()
{
	p_icnDocument = dynamic_cast<ICNDocument*>(p_view);
	p_mechanicsDocument = dynamic_cast<MechanicsDocument*>(p_view);
}


ItemList ItemGroup::items( bool excludeParentedItems ) const
{
	if (excludeParentedItems)
		return m_itemList;
	
	ItemList items = m_itemList;
	ItemList parents = m_itemList;
	
	uint oldSize = items.size();
	do
	{
		oldSize = items.size();
		ItemList children;
		
		ItemList::iterator end = parents.end();
		for ( ItemList::iterator it = parents.begin(); it != end; ++it )
			children += (*it)->children();
		
		end = children.end();
		for ( ItemList::iterator it = children.begin(); it != end; ++it )
		{
			if ( children.count(*it) > 1 )
				*it = 0l;
		}
		children.removeAll((Item*)0l);
		
		items += children;
		parents = children;
	}
	while ( oldSize != items.size() );
	
	return items;
}


bool ItemGroup::itemsHaveSameDataValue( const QString &id ) const
{
	if ( m_itemList.size() < 1 ) {
		return true;
	}
	
	if (!itemsAreSameType()) {
		return false;
	}
	
	ItemList::const_iterator it = m_itemList.begin();
	const ItemList::const_iterator end = m_itemList.end();
	QVariant firstData = (*it)->property(id)->value();
	for ( ++it; it != end; ++it )
	{
		if ( (*it) && (*it)->property(id) && (*it)->property(id)->value() != firstData ) {
			return false;
		}
	}
	return true;
}


bool ItemGroup::itemsHaveSameData() const
{
	if ( m_itemList.size() < 1 ) {
		return true;
	}
	
	if (!itemsAreSameType()) {
		return false;
	}
	
	VariantDataMap *variantMap = m_itemList.first()->variantMap();
	const VariantDataMap::const_iterator vitEnd = variantMap->end();
	for ( VariantDataMap::const_iterator vit = variantMap->begin(); vit != vitEnd; ++vit )
	{
		if ( !itemsHaveSameDataValue(vit.key()) ) {
			return false;
		}
	}
	return true;
}


bool ItemGroup::itemsHaveDefaultData() const
{
	if (!itemsHaveSameData()) {
		return false;
	}
	
	if ( m_itemList.size() < 1 ) {
		return true;
	}
	
	VariantDataMap *variantMap = (*m_itemList.begin())->variantMap();
	const VariantDataMap::const_iterator vitEnd = variantMap->end();
	for ( VariantDataMap::const_iterator vit = variantMap->begin(); vit != vitEnd; ++vit )
	{
		if ( !vit.value()->isHidden() && vit.value()->value() != vit.value()->defaultValue() )
			return false;
	}
	return true;
}


void ItemGroup::registerItem( Item *item )
{
	if ( !item || m_itemList.contains(item) ) {
		return;
	}
	
	m_itemList += item;
	updateAreSameStatus();
}


void ItemGroup::unregisterItem( Item *item )
{
	if ( m_itemList.removeAll(item) > 0 ) {
		updateAreSameStatus();
	}
}


void ItemGroup::updateAreSameStatus()
{
	b_itemsAreSameType = true;
	
	if ( m_itemList.size() < 2 ) {
		return;
	}
	
	QString activeId = (*m_itemList.begin())->id();
	int discardIndex = activeId.findRev("__");
	if ( discardIndex != -1 ) activeId.truncate(discardIndex);
	
	const ItemList::iterator end = m_itemList.end();
	for ( ItemList::iterator it = ++m_itemList.begin(); it != end && b_itemsAreSameType; ++it )
	{
		if (*it)
		{
			QString id = (*it)->id();
			discardIndex = id.findRev("__");
			if ( discardIndex != -1 ) id.truncate(discardIndex);
			if ( id != activeId )
			{
				b_itemsAreSameType = false;
			}
		}
	}
}


void ItemGroup::slotAlignHorizontally()
{
	if ( m_itemList.size() < 2 )
		return;
	
	double avg_y = 0.;
	
	const ItemList::iterator end = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
		avg_y += (*it)->y();
	
	int new_y = int(avg_y/(8*m_itemList.size()))*8+4;
	
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
		(*it)->move( (*it)->x(), new_y );
	
	p_icnDocument->requestStateSave();
}


void ItemGroup::slotAlignVertically()
{
	if ( m_itemList.size() < 2 )
		return;
	
	double avg_x = 0.;
	
	const ItemList::iterator end = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
		avg_x += (*it)->x();
	
	int new_x = int(avg_x/(8*m_itemList.size()))*8+4;
	
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
		(*it)->move( new_x, (*it)->y() );
	
	p_icnDocument->requestStateSave();
}


void ItemGroup::slotDistributeHorizontally()
{
	if ( m_itemList.size() < 2 )
		return;
	
	// We sort the items by their horizontal position so that we can calculate
	// an average spacing
	typedef std::multimap< double, Item * > DIMap;
	
	DIMap ranked;
	const ItemList::iterator ilend = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != ilend; ++it )
		ranked.insert( std::make_pair( (*it)->x(), *it ) );
	
	double avg_spacing = 0;
	
	Item * previous = 0l;
	const DIMap::iterator rankedEnd = ranked.end();
	for ( DIMap::iterator it = ranked.begin(); it != rankedEnd; ++it )
	{
		Item * item = it->second;
		if (previous)
		{
			double spacing = item->x() + item->offsetX() - (previous->x() + previous->width() + previous->offsetX());
			avg_spacing += spacing;
		}
		previous = item;
	}
	
	avg_spacing /= (m_itemList.size()-1);
	
	DIMap::iterator it = ranked.begin();
	// Position that we are up to
	double at = it->second->x() + it->second->width() + it->second->offsetX();
	for ( ++it; it != rankedEnd; ++it )
	{
		Item * item = it->second;
		double new_x = at - item->offsetX() + avg_spacing;
		item->move( snapToCanvas(new_x), item->y() );
		at = new_x + item->width() + item->offsetX();
	}
	
	p_icnDocument->requestStateSave();
}


void ItemGroup::slotDistributeVertically()
{
	if ( m_itemList.size() < 2 )
		return;
	
	// We sort the items by their horizontal position so that we can calculate
	// an average spacing
	typedef std::multimap< double, Item * > DIMap;
	
	DIMap ranked;
	const ItemList::iterator ilend = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != ilend; ++it )
		ranked.insert( std::make_pair( (*it)->y(), *it ) );
	
	double avg_spacing = 0;
	
	Item * previous = 0l;
	const DIMap::iterator rankedEnd = ranked.end();
	for ( DIMap::iterator it = ranked.begin(); it != rankedEnd; ++it )
	{
		Item * item = it->second;
		if (previous)
		{
			double spacing = item->y() + item->offsetY() - (previous->y() + previous->height() + previous->offsetY());
			avg_spacing += spacing;
		}
		previous = item;
	}
	
	avg_spacing /= (m_itemList.size()-1);
	
	DIMap::iterator it = ranked.begin();
	// Position that we are up to
	double at = it->second->y() + it->second->height() + it->second->offsetY();
	for ( ++it; it != rankedEnd; ++it )
	{
		Item * item = it->second;
		double new_y = at - item->offsetY() + avg_spacing;
		item->move( item->x(), snapToCanvas(new_y) );
		at = new_y + item->height() + item->offsetY();
	}
	
	p_icnDocument->requestStateSave();
}

#include "itemgroup.moc"
