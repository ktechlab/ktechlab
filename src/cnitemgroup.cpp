/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cnitemgroup.h"
#include "component.h"
#include "connector.h"
#include "connectorline.h"
#include "flowpart.h"
#include "icndocument.h"
#include "node.h"
#include "nodegroup.h"

CNItemGroup::CNItemGroup( ICNDocument *icnDocument, const char *name)
	: ItemGroup( icnDocument, name )
{
	p_icnDocument = icnDocument;
	m_connectorCount = 0;
	m_nodeCount = 0;
	m_currentLevel = -1;
}

CNItemGroup::~CNItemGroup()
{
}

bool CNItemGroup::addItem( Item *item )
{
	// Note, we must prepend the item to the list so that
	// activeCNItem() can return the item at the start
	// of the list as the most recently added item if some
	// the previous activeCNItem is removed
	
	if ( !item || !item->canvas() || m_itemList.contains(item) || !item->isMovable() )
		return false;
	
	if ( m_currentLevel != -1 && item->level() > m_currentLevel )
		return false;
	
	if ( item && m_currentLevel > item->level() )
		removeAllItems();
	
	registerItem(item);
	m_currentLevel = item->level();
	setActiveItem(item);
	item->setSelected(true);
	updateInfo();
	emit itemAdded(item);
	return true;
}


bool CNItemGroup::addNode( Node *node )
{
	if ( !node || m_nodeList.contains(node) || node->isChildNode() )
		return false;
	m_nodeList.prepend(node);
	node->setSelected(true);
	updateInfo();
	emit nodeAdded(node);
	return true;
}

bool CNItemGroup::addConnector( Connector *con )
{
	if(!con) return false;

	m_connectorList.insert(con);
	con->setSelected(true);
	updateInfo();
	emit connectorAdded(con);
	return true;
}

bool CNItemGroup::addQCanvasItem( QCanvasItem *qcanvasItem )
{
	if (!qcanvasItem) return false;
	
	Item *item = dynamic_cast<Item*>(qcanvasItem);
	if (item)
		return addItem(item);
	
	Node *node = dynamic_cast<Node*>(qcanvasItem);
	if (node)
		return addNode(node);
	
	Connector *connector = dynamic_cast<Connector*>(qcanvasItem);
	if (!connector)
	{
		ConnectorLine *connectorLine = dynamic_cast<ConnectorLine*>(qcanvasItem);
		if (connectorLine)
			connector = connectorLine->parent();
	}
	if (connector)
		return addConnector(connector);
	
	return false;
}

void CNItemGroup::setItems( QCanvasItemList list )
{
	ItemList itemRemoveList = m_itemList;
	ConnectorList connectorRemoveList = m_connectorList;
	NodeList nodeRemoveList = m_nodeList;
	
	const QCanvasItemList::const_iterator end = list.end();
	for ( QCanvasItemList::const_iterator it = list.begin(); it != end; ++it )
	{
		if ( Item *item = dynamic_cast<Item*>(*it) )
			itemRemoveList.remove( item );
		else if ( Node *node = dynamic_cast<Node*>(*it) )
			nodeRemoveList.remove( node );
		else if ( Connector *con = dynamic_cast<Connector*>(*it) )
			connectorRemoveList.erase( con );
		else if ( ConnectorLine *conLine = dynamic_cast<ConnectorLine*>(*it) )
			connectorRemoveList.erase( conLine->parent() );
	}
	
	{
		const ItemList::const_iterator end = itemRemoveList.end();
		for ( ItemList::const_iterator it = itemRemoveList.begin(); it != end; ++it )
		{
			removeItem(*it);
			(*it)->setSelected(false);
		}
	}
	
	{
		const NodeList::const_iterator end = nodeRemoveList.end();
		for ( NodeList::const_iterator it = nodeRemoveList.begin(); it != end; ++it )
		{
			removeNode(*it);
			(*it)->setSelected(false);
		}
	}
	
	{
		const ConnectorList::const_iterator end = connectorRemoveList.end();
		for ( ConnectorList::const_iterator it = connectorRemoveList.begin(); it != end; ++it )
		{
			removeConnector(*it);
			(*it)->setSelected(false);
		}
	}
	
	{
		const QCanvasItemList::const_iterator end = list.end();
		for ( QCanvasItemList::const_iterator it = list.begin(); it != end; ++it )
		{
			// We don't need to check that we've already got the item as it will
			// be checked in the function call
			addQCanvasItem(*it);
		}
	}
}


void CNItemGroup::removeItem( Item *item )
{
	if ( !item || !m_itemList.contains(item) )
		return;
	unregisterItem(item);
	if ( m_activeItem == item )
		getActiveItem();
	
	item->setSelected(false);
	updateInfo();
	emit itemRemoved(item);
}


void CNItemGroup::removeNode( Node *node )
{
	if ( !node || !m_nodeList.contains(node) )
		return;
	m_nodeList.remove(node);
	node->setSelected(false);
	updateInfo();
	emit nodeRemoved(node);
}


void CNItemGroup::removeConnector( Connector *con )
{
	if(!con) return;
	m_connectorList.erase(con);
	con->setSelected(false);
	updateInfo();
	emit connectorRemoved(con);
}


void CNItemGroup::removeQCanvasItem( QCanvasItem *qcanvasItem )
{
	if (!qcanvasItem) return;
	
	Item *item = dynamic_cast<Item*>(qcanvasItem);
	if (item)
		return removeItem(item);
	
	Node *node = dynamic_cast<Node*>(qcanvasItem);
	if (node)
		return removeNode(node);
	
	Connector *connector = dynamic_cast<Connector*>(qcanvasItem);
	if (!connector)
	{
		ConnectorLine *connectorLine = dynamic_cast<ConnectorLine*>(qcanvasItem);
		if (connectorLine)
			connector = connectorLine->parent();
	}
	if (connector)
		return removeConnector(connector);
}


NodeList CNItemGroup::nodes( bool excludeParented ) const
{
	NodeList nodeList = m_nodeList;
	if (excludeParented)
		return nodeList;
	
	NodeGroupList translatableNodeGroups;
	p_icnDocument->getTranslatable( items(false), 0l, 0l, &translatableNodeGroups );
	
	NodeGroupList::iterator end = translatableNodeGroups.end();
	for ( NodeGroupList::iterator it = translatableNodeGroups.begin(); it != end; ++it )
	{
		const NodeList internal = (*it)->internalNodeList();
		NodeList::const_iterator internalEnd = internal.end();
		for ( NodeList::const_iterator intIt = internal.begin(); intIt != internalEnd; ++intIt )
		{
			if ( *intIt && !nodeList.contains(*intIt) )
				nodeList << *intIt;
		}
	}
	
	return nodeList;
}


ConnectorList CNItemGroup::connectors( bool excludeParented ) const
{
	ConnectorList connectorList = m_connectorList;
	if (excludeParented)
		return connectorList;
	
	ConnectorList translatableConnectors;
	NodeGroupList translatableNodeGroups;
	p_icnDocument->getTranslatable( items(false), 0, &translatableConnectors, &translatableNodeGroups );
	
	ConnectorList::iterator tcEnd = translatableConnectors.end();
	for ( ConnectorList::iterator it = translatableConnectors.begin(); it != tcEnd; ++it )
	{
		if ( *it) connectorList.insert(*it);
	}
	
	NodeGroupList::iterator end = translatableNodeGroups.end();
	for ( NodeGroupList::iterator it = translatableNodeGroups.begin(); it != end; ++it )
	{
		const NodeList internal = (*it)->internalNodeList();
		NodeList::const_iterator internalEnd = internal.end();
		for ( NodeList::const_iterator intIt = internal.begin(); intIt != internalEnd; ++intIt )
		{
			const ConnectorList connected = (*intIt)->getAllConnectors();
			ConnectorList::const_iterator connectedEnd = connected.end();
			for ( ConnectorList::const_iterator conIt = connected.begin(); conIt != connectedEnd; ++conIt )
			{
				if ( *conIt)
					connectorList.insert(*conIt);
			}
		}
	}
	
	return connectorList;
}


bool CNItemGroup::contains( QCanvasItem *qcanvasItem ) const
{
	if (!qcanvasItem)
		return false;
	
	const ItemList::const_iterator ciEnd = m_itemList.end();
	for ( ItemList::const_iterator it = m_itemList.begin(); it != ciEnd; ++it )
	{
		if ( *it == qcanvasItem )
			return true;
	}
	const ConnectorList::const_iterator conEnd = m_connectorList.end();
	for ( ConnectorList::const_iterator it = m_connectorList.begin(); it != conEnd; ++it )
	{
		if ( *it == qcanvasItem )
			return true;
	}
	const NodeList::const_iterator nodeEnd = m_nodeList.end();
	for ( NodeList::const_iterator it = m_nodeList.begin(); it != nodeEnd; ++it )
	{
		if ( *it == qcanvasItem )
			return true;
	}
	
	return false;
}


void CNItemGroup::setSelected( bool sel )
{
	const ItemList::iterator ciEnd = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != ciEnd; ++it )
	{
		if (*it && (*it)->isSelected() != sel )
			(*it)->setSelected(sel);
	}
	const ConnectorList::iterator conEnd = m_connectorList.end();
	for ( ConnectorList::iterator it = m_connectorList.begin(); it != conEnd; ++it )
	{
		if ( *it && (*it)->isSelected() != sel )
			(*it)->setSelected(sel);
	}
	const NodeList::iterator nodeEnd = m_nodeList.end();
	for ( NodeList::iterator it = m_nodeList.begin(); it != nodeEnd; ++it )
	{
		if ( *it && (*it)->isSelected() != sel )
			(*it)->setSelected(sel);
	}
}


bool CNItemGroup::canRotate() const
{
	const ItemList::const_iterator end = m_itemList.end();
	for ( ItemList::const_iterator it = m_itemList.begin(); it != end; ++it )
	{
		// Components can rotate
		if ( dynamic_cast<Component*>((Item*)*it) )
			return true;
	}
	return false;
}


bool CNItemGroup::canFlip() const
{
	const ItemList::const_iterator end = m_itemList.end();
	for ( ItemList::const_iterator it = m_itemList.begin(); it != end; ++it )
	{
		// Components can flip
		if ( dynamic_cast<Component*>((Item*)*it) )
			return true;
	}
	return false;
}


void CNItemGroup::slotRotateCW()
{
	const ItemList::iterator end = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
	{
		Component *component = dynamic_cast<Component*>((Item*)*it);
		if ( component && component->isMovable() )
		{
			int oldAngle = component->angleDegrees();
			component->setAngleDegrees( oldAngle + 90 );
		}
	}
	p_icnDocument->requestStateSave();
}

void CNItemGroup::slotRotateCCW()
{
	const ItemList::iterator end = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
	{
		Component *component = dynamic_cast<Component*>((Item*)*it);
		if ( component && component->isMovable() )
		{
			int oldAngle = component->angleDegrees();
			component->setAngleDegrees( oldAngle - 90 );
		}
	}
	p_icnDocument->requestStateSave();
}


void CNItemGroup::flipHorizontally()
{
	const ItemList::iterator end = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
	{
		Component *component = dynamic_cast<Component*>((Item*)*it);
		if ( component && component->isMovable() )
		{
			bool oldFlipped = component->flipped();
			component->setFlipped(!oldFlipped);
		}
	}
	p_icnDocument->requestStateSave();
}


void CNItemGroup::flipVertically()
{
	const ItemList::iterator end = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
	{
		Component *component = dynamic_cast<Component*>((Item*)*it);
		if ( component && component->isMovable() )
		{
			bool oldFlipped = component->flipped();
			
			int oldAngle = component->angleDegrees();
			component->setAngleDegrees( oldAngle + 180 );
			component->setFlipped( ! oldFlipped );
			component->setAngleDegrees( oldAngle + 180 );
		}
	}
	p_icnDocument->requestStateSave();
}


bool CNItemGroup::haveSameOrientation() const
{
	// set true once determined what is in this itemgroup
	bool areFlowparts = false;
	bool areComponents = false;
	
	// for components
	int angleDegrees = 0;
	bool flipped = false;
	
	// for flowparts
	unsigned orientation = 0;
	
	const ItemList::const_iterator end = m_itemList.end();
	for ( ItemList::const_iterator it = m_itemList.begin(); it != end; ++it )
	{
		Component * component = dynamic_cast<Component*>((Item*)*it);
		FlowPart * flowpart = dynamic_cast<FlowPart*>((Item*)*it);
		
		if ( component && flowpart )
			return false;
		
		if ( !component && !flowpart )
			return false;
		
		if ( component )
		{
			if ( areFlowparts )
				return false;
			
			if ( !areComponents )
			{
				// It's the first component we've come across
				angleDegrees = component->angleDegrees();
				flipped = component->flipped();
				areComponents = true;
			}
			else
			{
				if ( angleDegrees != component->angleDegrees() )
					return false;
				
				if ( flipped != component->flipped() )
					return false;
			}
		}
		else
		{
			if ( areComponents )
				return false;
			
			if ( !areFlowparts )
			{
				// It's the first flowpart we've come across
				orientation = flowpart->orientation();
				areFlowparts = true;
			}
			else
			{
				if ( orientation != flowpart->orientation() )
					return false;
			}
		}
	}
	
	return true;
}


void CNItemGroup::setOrientationAngle( int _angle )
{
	const ItemList::iterator end = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
	{
		Component *component = dynamic_cast<Component*>((Item*)*it);
		if ( component && component->isMovable() )
		{
			int oldAngle = component->angleDegrees();
			if ( oldAngle != _angle )
			{
				component->setAngleDegrees(_angle);
			}
		}
	}
	p_icnDocument->requestStateSave();
}


void CNItemGroup::setComponentOrientation( int angleDegrees, bool flipped )
{
	const ItemList::iterator end = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
	{
		Component *component = dynamic_cast<Component*>((Item*)*it);
		if ( component && component->isMovable() )
		{
			int oldAngle = component->angleDegrees();
			int oldFlipped = component->flipped();
			if ( (oldAngle != angleDegrees) || (oldFlipped != flipped) )
			{
				component->setFlipped(flipped);
				component->setAngleDegrees(angleDegrees);
			}
		}
	}
	p_icnDocument->requestStateSave();
}


void CNItemGroup::setFlowPartOrientation( unsigned orientation )
{
	const ItemList::iterator end = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
	{
		FlowPart * flowPart = dynamic_cast<FlowPart*>((Item*)*it);
		if ( flowPart && flowPart->isMovable() )
			flowPart->setOrientation(orientation);
	}
	p_icnDocument->requestStateSave();
}


void CNItemGroup::mergeGroup( ItemGroup *itemGroup )
{
	CNItemGroup *group = dynamic_cast<CNItemGroup*>(itemGroup);
	if (!group) return;
	
	const ItemList items = group->items();
	const ConnectorList connectors = group->connectors();
	const NodeList nodes = group->nodes();
	
	const ItemList::const_iterator ciEnd = items.end();
	for ( ItemList::const_iterator it = items.begin(); it != ciEnd; ++it )
	{
		addItem(*it);
	}
	const ConnectorList::const_iterator conEnd = connectors.end();
	for ( ConnectorList::const_iterator it = connectors.begin(); it != conEnd; ++it )
	{
		addConnector(*it);
	}
	const NodeList::const_iterator nodeEnd = nodes.end();
	for ( NodeList::const_iterator it = nodes.begin(); it != nodeEnd; ++it )
	{
		addNode(*it);
	}
}

void CNItemGroup::removeAllItems()
{
	while ( !m_itemList.isEmpty() )
		removeItem(*m_itemList.begin());

	while ( !m_connectorList.empty() )
		removeConnector(*m_connectorList.begin());

	while ( !m_nodeList.isEmpty() )
		removeNode(*m_nodeList.begin());
}

void CNItemGroup::deleteAllItems()
{
	const ItemList::iterator ciEnd = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != ciEnd; ++it )
	{
		if (*it)
			(*it)->removeItem();
	}
	const NodeList::iterator nodeEnd = m_nodeList.end();
	for ( NodeList::iterator it = m_nodeList.begin(); it != nodeEnd; ++it )
	{
		if ( *it && !(*it)->isChildNode() )
		{
			(*it)->removeNode();
		}
	}
	const ConnectorList::iterator conEnd = m_connectorList.end();
	for ( ConnectorList::iterator it = m_connectorList.begin(); it != conEnd; ++it )
	{
		if (*it)
		{
			(*it)->removeConnector();
		}
	}
	
	// Clear the lists
	removeAllItems();
}
	

void CNItemGroup::updateInfo()
{
	m_connectorCount = m_connectorList.size();
	m_nodeCount = m_nodeList.count();
	
	if ( m_itemList.isEmpty() )
		m_currentLevel = -1;
}


void CNItemGroup::getActiveItem()
{
	if ( m_itemList.isEmpty() )
		setActiveItem(0l);
	else
		setActiveItem( *m_itemList.begin() );
}


void CNItemGroup::setActiveItem( Item *item )
{
	if ( item == m_activeItem )
		return;
	m_activeItem = item;
}


QStringList CNItemGroup::itemIDs()
{
	QStringList list;
	ItemList::iterator end = m_itemList.end();
	for ( ItemList::iterator it = m_itemList.begin(); it != end; ++it )
	{
		if (*it) {
			list += (*it)->id();
		}
	}
	return list;
}

#include "cnitemgroup.moc"
