/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MECHANICSGROUP_H
#define MECHANICSGROUP_H

#include <itemgroup.h>


class MechanicsItem;
class MechanicsDocument;
typedef QList<MechanicsItem*> MechanicsItemList;

/**
@author David Saxton
*/
class MechanicsGroup : public ItemGroup
{
Q_OBJECT
public:
	MechanicsGroup( MechanicsDocument *mechanicsDocument, const char *name = 0);
	~MechanicsGroup();
	
	/**
	 * Returns a list of top-level mechanics items only
	 */
	MechanicsItemList toplevelMechItemList() const;
	/**
	 * Sets the selection mode of all MechanicsItems in the group
	 */
	void setSelectionMode( uint sm );
	/**
	 * "Raises" (increases the z value of) the selected group of items
	 */
	void setRaised( bool isRaised );
	/**
	 * Removes all the children of the given item from the group
	 */
	void removeChildren( Item *item );
	bool addItem( Item *item );
	bool removeItem( Item *item );
	virtual bool addQCanvasItem(QCanvasItem* item);
	virtual bool contains(QCanvasItem* item) const;
	virtual uint count() const { return itemCount(); }
	virtual void deleteAllItems();
	virtual void mergeGroup(ItemGroup* group);
	virtual void removeAllItems();
	virtual void removeQCanvasItem(QCanvasItem* item);
	virtual void setItems(QCanvasItemList list);
	/**
	 * Sets the selected state of all items in the group
	 */
	virtual void setSelected( bool sel );
	/**
	 * Extracts the mechanics items from the item list
	 */
	MechanicsItemList extractMechanicsItems() const;
	
protected:
	void updateInfo();
	
	bool b_isRaised;
};

#endif
