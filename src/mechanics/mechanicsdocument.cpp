/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "mechanicsdocument.h"
#include "canvasmanipulator.h"
#include "documentiface.h"
#include "drawpart.h"
#include "itemlibrary.h"
#include "mechanicsgroup.h"
#include "mechanicsitem.h"
#include "mechanicssimulation.h"
#include "mechanicsview.h"

#include <KLocalizedString>

MechanicsDocument::MechanicsDocument(const QString &caption)
    : ItemDocument(caption)
{
    m_type = Document::dt_mechanics;
    m_pDocumentIface = new MechanicsDocumentIface(this);
    m_fileExtensionInfo = QString("*.mechanics|%1 (*.mechanics)\n*|%2").arg(i18n("Mechanics")).arg(i18n("All Files"));
    m_fileExtensionValue = QString(".mechanics");
    m_canvas->retune(128);

    m_selectList = new MechanicsGroup(this);

    m_cmManager->addManipulatorInfo(CMSelect::manipulatorInfo());
    m_cmManager->addManipulatorInfo(CMDraw::manipulatorInfo());
    m_cmManager->addManipulatorInfo(CMRightClick::manipulatorInfo());
    m_cmManager->addManipulatorInfo(CMRepeatedItemAdd::manipulatorInfo());
    m_cmManager->addManipulatorInfo(CMItemResize::manipulatorInfo());
    m_cmManager->addManipulatorInfo(CMMechItemMove::manipulatorInfo());
    m_mechanicsSimulation = new MechanicsSimulation(this);
    requestStateSave();
}

MechanicsDocument::~MechanicsDocument()
{
    m_bDeleted = true;

    // Remove all items from the canvas
    selectAll();
    deleteSelection();
    delete m_mechanicsSimulation;
}

View *MechanicsDocument::createView(ViewContainer *viewContainer, uint viewAreaId)
{
    ItemView *itemView = new MechanicsView(this, viewContainer, viewAreaId);
    handleNewView(itemView);
    return itemView;
}

ItemGroup *MechanicsDocument::selectList() const
{
    return m_selectList;
}

bool MechanicsDocument::isValidItem(const QString &itemId)
{
    return itemId.startsWith("mech/") || itemId.startsWith("dp/");
}

bool MechanicsDocument::isValidItem(Item *item)
{
    return item && ((dynamic_cast<MechanicsItem *>(item)) || (dynamic_cast<DrawPart *>(item)));
}

Item *MechanicsDocument::addItem(const QString &id, const QPoint &p, bool newItem)
{
    if (!isValidItem(id))
        return nullptr;

    Item *item = itemLibrary()->createItem(id, this, newItem);
    if (!item)
        return nullptr;

    QRect rect = item->boundingRect();

    int dx = (int)(p.x()) - rect.width() / 2;
    int dy = (int)(p.y()) - rect.height() / 2;

    if (dx < 16 || dx > (m_canvas->width()))
        dx = 16;
    if (dy < 16 || dy > (m_canvas->height()))
        dy = 16;

    item->move(dx, dy);
    item->show();

    registerItem(item);
    // 	setModified(true);
    requestStateSave();
    return item;
}

void MechanicsDocument::deleteSelection()
{
    // End whatever editing mode we are in, as we don't want to start editing
    // something that is about to no longer exist...
    m_cmManager->cancelCurrentManipulation();

    if (m_selectList->isEmpty())
        return;

    // We nee to tell the selete items to remove themselves, and then
    // pass the items that have add themselves to the delete list to the
    // CommandAddItems command

    m_selectList->deleteAllItems();
    flushDeleteList();
    setModified(true);

    // We need to emit this so that property widgets etc...
    // can clear themselves.
    emit selectionChanged();
    requestStateSave();
}

bool MechanicsDocument::registerItem(KtlQCanvasItem *qcanvasItem)
{
    return ItemDocument::registerItem(qcanvasItem);
}

void MechanicsDocument::appendDeleteList(KtlQCanvasItem *qcanvasItem)
{
    MechanicsItem *mechItem = dynamic_cast<MechanicsItem *>(qcanvasItem);
    if (!mechItem || m_itemDeleteList.contains(mechItem)) {
        return;
    }

    m_itemDeleteList.append(mechItem);
    m_itemList.remove(mechItem->id());

    disconnect(mechItem, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));

    mechItem->removeItem();
}

void MechanicsDocument::flushDeleteList()
{
    // Remove duplicate items in the delete list
    ItemList::iterator end = m_itemDeleteList.end();
    for (ItemList::iterator it = m_itemDeleteList.begin(); it != end; ++it) {
        if (*it && m_itemDeleteList.count(*it) > 1)
            *it = nullptr;
    }
    m_itemDeleteList.removeAll(QPointer<Item>(nullptr));

    end = m_itemDeleteList.end();
    for (ItemList::iterator it = m_itemDeleteList.begin(); it != end; ++it) {
        m_itemList.remove((*it)->id());
        (*it)->setCanvas(nullptr);
        delete *it;
    }
}

MechanicsItem *MechanicsDocument::mechanicsItemWithID(const QString &id)
{
    return dynamic_cast<MechanicsItem *>(itemWithID(id));
}

void MechanicsDocument::selectAll()
{
    const ItemMap::iterator end = m_itemList.end();
    for (ItemMap::iterator it = m_itemList.begin(); it != end; ++it) {
        select(*it);
    }
}

void MechanicsDocument::copy()
{
}

#include "moc_mechanicsdocument.cpp"
