/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ITEMGROUP_H
#define ITEMGROUP_H

#include <QPointer>
#include <QObject>

class Item;
class ICNDocument;
class ItemDocument;
class DoubleSpinBox;
class ItemGroup;
class MechanicsDocument;
class Variant;

typedef QList<QPointer<Item>> ItemList;

class KtlQCanvasItem;
class KtlQCanvasItemList;

/**
Generic base class for controlling a selection of Item. Provides
some functionality such as for dealing with item data
@author David Saxton
*/
class ItemGroup : public QObject
{
    Q_OBJECT
public:
    ItemGroup(ItemDocument *view);
    ~ItemGroup() override;

    /**
     * Returns a pointer to the "active" CNItem - i.e. the last CNItem
     * to be added to the CNItemGroup. This will always return a pointer to
     * a single item, unless there are no CNItems in the group
     */
    Item *activeItem() const
    {
        return m_activeItem;
    }
    uint itemCount() const
    {
        return m_itemList.count();
    }
    virtual bool addQCanvasItem(KtlQCanvasItem *qcanvasItem) = 0;
    virtual void setItems(KtlQCanvasItemList list) = 0;
    virtual void removeQCanvasItem(KtlQCanvasItem *qcanvasItem) = 0;
    virtual bool contains(KtlQCanvasItem *qcanvasItem) const = 0;
    virtual uint count() const = 0;
    bool isEmpty() const
    {
        return (count() == 0);
    }
    virtual void mergeGroup(ItemGroup *group) = 0;
    virtual void removeAllItems() = 0;
    virtual void deleteAllItems() = 0;
    /**
     * Returns a list of all the Items in the group.
     * @param excludeParented whether to return items whose (grand-) parents are
     * already in the list.
     */
    ItemList items(bool excludeParented = true) const;
    /**
     * Sets the selected state of all items in the group
     */
    virtual void setSelected(bool sel) = 0;

    /**
     * Returns true iff either there are no items, or itemsAreSameType and the
     * value of each data (excluding hidden data) for each item is the same
     */
    bool itemsHaveSameData() const;
    /**
     * Returns truee iff either there are no items, or itemsAreSameType and the
     * value of the data with the given id is the same for each item
     */
    bool itemsHaveSameDataValue(const QString &id) const;
    /**
     * Returns true iff all the iff itemsHaveSameData() returns true and the
     * value of the data are the defaults
     */
    bool itemsHaveDefaultData() const;
    /**
     * Returns true if all the items in the group are the same (e.g.
     * resistors). This is checked for by looking at the ids of the items,
     * and seeing if the string before "__#" is the same Note: if there are zero
     * items in the group, then this will return true
     */
    bool itemsAreSameType() const
    {
        return b_itemsAreSameType;
    }

public slots:
    /**
     * Align the selected items horizontally so that their positions have the
     * same y coordinate.
     */
    void slotAlignHorizontally();
    /**
     * Align the selected items horizontally so that their positions have the
     * same x coordinate.
     */
    void slotAlignVertically();
    /**
     * Distribute the selected items horizontally so that they have the same
     * spacing in the horizontal direction.
     */
    void slotDistributeHorizontally();
    /**
     * Distribute the selected items vertically so that they have the same
     * spacing in the vertical direction.
     */
    void slotDistributeVertically();

Q_SIGNALS:
    void itemAdded(Item *item);
    void itemRemoved(Item *item);

protected:
    /**
     * Subclasses must call this to register the item with the data interface
     */
    void registerItem(Item *item);
    /**
     * Subclasses must call this to unregister the item with the data interface
     */
    void unregisterItem(Item *item);
    void updateAreSameStatus();

    ItemList m_itemList;
    bool b_itemsAreSameType;
    ItemDocument *p_view;

    ICNDocument *p_icnDocument;
    MechanicsDocument *p_mechanicsDocument;
    Item *m_activeItem;

private slots:
    void getViewPtrs();
};

#endif
