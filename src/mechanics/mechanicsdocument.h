/***************************************************************************
 *   Copyright (C) 2004 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MECHANICSDOCUMENT_H
#define MECHANICSDOCUMENT_H

#include "itemdocument.h"

class KTechlab;
class MechanicsGroup;
class MechanicsItem;
class MechanicsSimulation;

typedef QValueList<MechanicsItem*> MechItemList;
typedef QValueList<MechanicsItem*> MechanicsItemList;

/**
@author David Saxton
*/
class MechanicsDocument : public ItemDocument {
	Q_OBJECT

public:
	MechanicsDocument(const QString &caption, const char *name = 0);
	~MechanicsDocument();

	virtual View *createView(ViewContainer *viewContainer, uint viewAreaId, const char *name = 0);

	virtual bool isValidItem(const QString &itemId);
	virtual bool isValidItem(Item *item);

	virtual void deleteSelection();
	virtual void copy();
	virtual void selectAll();
	virtual ItemGroup *selectList() const;
	MechanicsItem *mechanicsItemWithID(const QString &id);
	virtual Item* addItem(const QString &id, const QPoint &p, bool newItem);
	/**
	 * Adds a QCanvasItem to the delete list to be deleted, when
	 * flushDeleteList() is called
	 */
	virtual void appendDeleteList(QCanvasItem *qcanvasItem);
	/**
	 * Permantly deletes all items that have been added to the delete list with
	 * the appendDeleteList( QCanvasItem *qcanvasItem ) function.
	 */
	virtual void flushDeleteList();
	/**
	 * Register an item with the ICNDocument.
	 */
	virtual bool registerItem(QCanvasItem *qcanvasItem);

protected:
	ItemList	m_itemDeleteList;

	MechanicsGroup *m_selectList;
	MechanicsSimulation *m_mechanicsSimulation;
};
#endif
