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
typedef QList<MechanicsItem *> MechanicsItemList;

/**
@author David Saxton
*/
class MechanicsGroup : public ItemGroup
{
    Q_OBJECT
public:
    MechanicsGroup(MechanicsDocument *mechanicsDocument, const char *name = nullptr);
    ~MechanicsGroup() override;

    /**
     * Returns a list of top-level mechanics items only
     */
    MechanicsItemList toplevelMechItemList() const;
    /**
     * Sets the selection mode of all MechanicsItems in the group
     */
    void setSelectionMode(uint sm);
    /**
     * "Raises" (increases the z value of) the selected group of items
     */
    void setRaised(bool isRaised);
    /**
     * Removes all the children of the given item from the group
     */
    void removeChildren(Item *item);
    bool addItem(Item *item);
    bool removeItem(Item *item);
    bool addQCanvasItem(KtlQCanvasItem *item) override;
    bool contains(KtlQCanvasItem *item) const override;
    uint count() const override
    {
        return itemCount();
    }
    void deleteAllItems() override;
    void mergeGroup(ItemGroup *group) override;
    void removeAllItems() override;
    void removeQCanvasItem(KtlQCanvasItem *item) override;
    void setItems(KtlQCanvasItemList list) override;
    /**
     * Sets the selected state of all items in the group
     */
    void setSelected(bool sel) override;

    /**
     * Extracts the mechanics items from the item list
     */
    MechanicsItemList extractMechanicsItems() const;

protected:
    void updateInfo();

    bool b_isRaised;
};

#endif
