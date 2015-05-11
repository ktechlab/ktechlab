/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CANVASITEMGROUP_H
#define CANVASITEMGROUP_H

#include "itemgroup.h"


class CNItem;
class Item;
class ICNDocument;
class Component;
class Connector;
class FlowPart;
class Node;
class ECNode;
// class FPNode;
class ICNDocument;
class KtlQCanvasItem;
class KtlQCanvasItemList;

typedef QList<QPointer<Item> > ItemList;
typedef QList<QPointer<Node> > NodeList;
typedef QList<QPointer<Connector> > ConnectorList;

/**
@author David Saxton
*/
class CNItemGroup : public ItemGroup
{
Q_OBJECT
public:
	CNItemGroup( ICNDocument *icnDocument, const char *name = 0 );
	~CNItemGroup();
	
	/**
	 * Adds a CNItem to the group, if it is not already in it, or other items at
	 * a lower levels are already in the group. If there are items are a high level,
	 * those items are removed first. Returns false on failure to add.
	 */
	bool addItem( Item *item );
	/**
	 * Adds a Node to the group, if it is not already in it. Note: This node
	 * will *NOT* be added if it is a child node, and the function will return false.
	 * If the node is not already present, and is added, then this will return true.
	 */
	bool addNode( Node *node );
	/**
	 * Adds a Connector to the group, if it is not already in it (if it is, returns false)
	 */
	bool addConnector( Connector *con );
	/**
	 * If the item is a a CNItem, Node or Connector, returns the status
	 * for that particular add function, else returns false
	 */
	virtual bool addQCanvasItem( KtlQCanvasItem *qcanvasItem );
	/**
	 * Sets the contained items to those in this list
	 */
	virtual void setItems( KtlQCanvasItemList list );
	/**
	 * Removes the CNItem from the group
	 */
	void removeItem( Item *item );
	/**
	 * Removes the Node from the group
	 */
	void removeNode( Node *node );
	/**
	 * Removes the Connector from the group
	 */
	void removeConnector( Connector *con );
	/**
	 * If the item is a a CNItem, Node or Connector, then attempts to remove it
	 */
	virtual void removeQCanvasItem( KtlQCanvasItem *qcanvasItem );
	/**
	 * Returns true if the KtlQCanvasItem passed is contained in the group
	 */
	virtual bool contains( KtlQCanvasItem *qcanvasItem ) const;
	/**
	 * Returns the number of Nodes in the CanvasGroup
	 */
	uint nodeCount() const { return m_nodeCount; }
	/**
	 * Returns the number of Connectors in the CanvasGroup
	 */
	uint connectorCount() const { return m_connectorCount; }
	/**
	 * Returns the total number of items in the group
	 * (CNItems, Nodes, Connectors)
	 */
	uint count() const { return itemCount()+m_nodeCount+m_connectorCount; }
	/**
	 * Sets the selected state of all items in the group
	 */
	virtual void setSelected( bool sel );
	/**
	 * Sets the orientation (degrees component) of all items in the group
	 */
	void setOrientationAngle( int angleDegrees );
	/**
	 * Sets the orientation (flipped component) of all items in the group
	 */
	void setOrientationFlipped( bool flipped );
	/**
	 * Sets the orientation of all flowparts in the group
	 */
	void setFlowPartOrientation( unsigned orientation );
	/**
	 * Sets the orientation (degrees and flipped) of all components in the group
	 */
	void setComponentOrientation( int angleDegrees, bool flipped );
	/**
	 * Merges all items in the given group with this group
	 */
	virtual void mergeGroup( ItemGroup *group );
	/**
	 * Removes all items from this group (doesn't delete them)
	 * @param unselect whether to unselect the items or not. This will be done after removal from group
	 */
	virtual void removeAllItems();
	/**
	 * Attempts to delete everything in the group.
	 * Note: You *must* call ICNDocument::flushDeleteList() after calling this function,
	 * as this function only tells the items to remove themselves
	 */
	virtual void deleteAllItems();
	/**
	 * Returns a list of all the Nodes in the group.
	 * @param excludeParented if false, then nodes that are fully contained
	 * within item children will also be returned.
	 */
	NodeList nodes( bool excludeParented = true ) const;
	/**
	 * Returns a list of all the Connectors in the group.
	 * @param excludeParented if false, then connectors that are fully contained
	 * within item children will also be returned.
	 */
	ConnectorList connectors( bool excludeParented = true ) const;
	/**
	 * Returns a list of the ids of all the CNItems in the group.
	 */
	QStringList itemIDs();
	/**
	 * Returns true if at least some of the CNItems in this group can be
	 * rotated. Returns false if no items present.
	 */
	bool canRotate() const;
	/**
	 * Returns true if at least some of the CNItems in this group can be
	 * flipped. Returns false if no items present.
	 */
	bool canFlip() const;
	/**
	 * @return whether all the components or flowparts have the same
	 * orientation. If there are a mixture of components and flowparts
	 * (or other items), this will return false.
	 */
	bool haveSameOrientation() const;
	
	
public slots:
	/**
	 * Sets the orientation of all selected items to 0 degrees.
	 */
	void slotSetOrientation0() { setOrientationAngle(0); }
	/**
	 * Sets the orientation of all selected items to 90 degrees.
	 */
	void slotSetOrientation90() { setOrientationAngle(90); }
	/**
	 * Sets the orientation of all selected items to 180 degrees.
	 */
	void slotSetOrientation180() { setOrientationAngle(180); }
	/**
	 * Sets the orientation of all selected items to 270 (actually -90) degrees.
	 */
	void slotSetOrientation270() { setOrientationAngle(-90); }
	/**
	 * Rotates all CNItems in the group clockwise
	 */
	void slotRotateCW();
	/**
	 * Rotates all CNItems in the group counter-clockwise
	 */
	void slotRotateCCW();
	/**
	 * Flips all CNItems in the group through a horizontal axis.
	 */
	void flipVertically();
	/**
	 * Flips all CNItems in the group through a veritcal axis.
	 */
	void flipHorizontally();

signals:
	void connectorAdded( Connector *con );
	void connectorRemoved( Connector *con );
	void nodeAdded( Node *node );
	void nodeRemoved( Node *node );

protected:
	void updateInfo();
	void getActiveItem();
	void setActiveItem( Item *item );

private:
	ICNDocument *p_icnDocument;
	ConnectorList m_connectorList;
	NodeList m_nodeList;
	uint m_connectorCount;
	uint m_nodeCount;
	int m_currentLevel; // We can only accept CNItems of one level
};

#endif
