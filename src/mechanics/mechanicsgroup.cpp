/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mechanicsgroup.h"
#include "mechanicsdocument.h"
#include "mechanicsitem.h"

MechanicsGroup::MechanicsGroup(MechanicsDocument *mechanicsDocument)
    : ItemGroup(mechanicsDocument)
{
    b_isRaised = false;
}

MechanicsGroup::~MechanicsGroup()
{
}

bool MechanicsGroup::addItem(Item *item)
{
    if (!item || !item->canvas() || m_itemList.contains(item)) {
        return false;
    }

    // Check that the item's parent isn't already selected
    Item *parent = item->parentItem();
    while (parent) {
        if (m_itemList.contains(parent))
            return false;
        parent = parent->parentItem();
    }
    removeChildren(item);

    registerItem(item);
    updateInfo();
    item->setSelected(true);
    if (MechanicsItem *mechanicsItem = dynamic_cast<MechanicsItem *>(item))
        mechanicsItem->setRaised(b_isRaised);
    emit itemAdded(item);
    return true;
}

bool MechanicsGroup::removeItem(Item *item)
{
    if (!item || !m_itemList.contains(item)) {
        return false;
    }
    unregisterItem(item);
    updateInfo();
    item->setSelected(false);
    MechanicsItem *mechanicsItem = dynamic_cast<MechanicsItem *>(item);
    if (mechanicsItem)
        mechanicsItem->setRaised(false);
    emit itemRemoved(item);
    return true;
}

void MechanicsGroup::removeChildren(Item *item)
{
    if (!item)
        return;

    const ItemList children = item->children();
    const ItemList::const_iterator end = children.end();
    for (ItemList::const_iterator it = children.begin(); it != end; ++it) {
        removeChildren(*it);
        removeItem(*it);
    }
}

void MechanicsGroup::setRaised(bool isRaised)
{
    b_isRaised = isRaised;
    const ItemList::iterator end = m_itemList.end();
    for (ItemList::iterator it = m_itemList.begin(); it != end; ++it) {
        MechanicsItem *mechanicsItem = dynamic_cast<MechanicsItem *>((Item *)*it);
        if (mechanicsItem)
            mechanicsItem->setRaised(b_isRaised);
    }
}

void MechanicsGroup::setSelectionMode(uint sm)
{
    const ItemList::iterator end = m_itemList.end();
    for (ItemList::iterator it = m_itemList.begin(); it != end; ++it) {
        MechanicsItem *mechanicsItem = dynamic_cast<MechanicsItem *>((Item *)*it);
        if (mechanicsItem)
            mechanicsItem->setSelectionMode((MechanicsItem::SelectionMode)sm);
    }
}

MechanicsItemList MechanicsGroup::extractMechanicsItems() const
{
    MechanicsItemList mechanicsItemList;

    const ItemList::const_iterator end = m_itemList.end();
    for (ItemList::const_iterator it = m_itemList.begin(); it != end; ++it) {
        MechanicsItem *mechanicsItem = dynamic_cast<MechanicsItem *>((Item *)*it);
        if (mechanicsItem)
            mechanicsItemList.append(mechanicsItem);
    }

    return mechanicsItemList;
}

MechanicsItemList MechanicsGroup::toplevelMechItemList() const
{
    MechanicsItemList toplevel;

    MechanicsItemList mechItemList = extractMechanicsItems();

    const MechanicsItemList::const_iterator end = mechItemList.end();
    for (MechanicsItemList::const_iterator it = mechItemList.begin(); it != end; ++it) {
        MechanicsItem *parent = *it;
        while (parent) {
            if (!parent->parentItem() && !toplevel.contains(parent))
                toplevel.append(parent);

            parent = dynamic_cast<MechanicsItem *>(parent->parentItem());
        }
    }

    return toplevel;
}

void MechanicsGroup::setSelected(bool sel)
{
    const ItemList::iterator end = m_itemList.end();
    for (ItemList::iterator it = m_itemList.begin(); it != end; ++it) {
        if (*it && (*it)->isSelected() != sel) {
            (*it)->setSelected(sel);
        }
    }
}

bool MechanicsGroup::addQCanvasItem(KtlQCanvasItem *item)
{
    return addItem(dynamic_cast<Item *>(item));
}

bool MechanicsGroup::contains(KtlQCanvasItem *item) const
{
    return m_itemList.contains(dynamic_cast<Item *>(item));
}

void MechanicsGroup::deleteAllItems()
{
    const ItemList::iterator end = m_itemList.end();
    for (ItemList::iterator it = m_itemList.begin(); it != end; ++it) {
        if (*it)
            (*it)->removeItem();
    }

    removeAllItems();
}

void MechanicsGroup::mergeGroup(ItemGroup *itemGroup)
{
    MechanicsGroup *group = dynamic_cast<MechanicsGroup *>(itemGroup);
    if (!group) {
        return;
    }

    const ItemList items = group->items();
    const ItemList::const_iterator end = items.end();
    for (ItemList::const_iterator it = items.begin(); it != end; ++it) {
        addItem(*it);
    }
}

void MechanicsGroup::removeAllItems()
{
    while (!m_itemList.isEmpty())
        removeItem(m_itemList.first());
}

void MechanicsGroup::removeQCanvasItem(KtlQCanvasItem *item)
{
    removeItem(dynamic_cast<Item *>(item));
}

void MechanicsGroup::setItems(KtlQCanvasItemList list)
{
    {
        ItemList removeList;
        const ItemList::iterator end = m_itemList.end();
        for (ItemList::iterator it = m_itemList.begin(); it != end; ++it) {
            if (!list.contains(*it)) {
                removeList.append(*it);
            }
        }
        const ItemList::iterator rend = removeList.end();
        for (ItemList::iterator it = removeList.begin(); it != rend; ++it) {
            removeItem(*it);
            (*it)->setSelected(false);
        }
    }

    const KtlQCanvasItemList::iterator end = list.end();
    for (KtlQCanvasItemList::iterator it = list.begin(); it != end; ++it) {
        // We don't need to check that we've already got the item as it will
        // be checked in the function call
        addQCanvasItem(*it);
    }
}

void MechanicsGroup::updateInfo()
{
}
