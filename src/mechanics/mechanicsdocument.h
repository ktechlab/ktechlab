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

typedef QList<MechanicsItem*> MechItemList;
typedef QList<MechanicsItem*> MechanicsItemList;

/**
@author David Saxton
*/
class MechanicsDocument : public ItemDocument
{
Q_OBJECT
public:
	MechanicsDocument( const QString &caption, const char *name = nullptr );
	~MechanicsDocument() override;
	
	View *createView( ViewContainer *viewContainer, uint viewAreaId, const char *name = nullptr ) override;
	
	bool isValidItem( const QString &itemId ) override;
	bool isValidItem( Item *item ) override;
	
	void deleteSelection() override;
	void copy() override;
	void selectAll() override;
	ItemGroup *selectList() const override;
	MechanicsItem *mechanicsItemWithID( const QString &id );
	Item* addItem( const QString &id, const QPoint &p, bool newItem ) override;

	/**
	 * Adds a KtlQCanvasItem to the delete list to be deleted, when
	 * flushDeleteList() is called
	 */
	void appendDeleteList( KtlQCanvasItem *qcanvasItem ) override;
	/**
	 * Permantly deletes all items that have been added to the delete list with
	 * the appendDeleteList( KtlQCanvasItem *qcanvasItem ) function.
	 */
	void flushDeleteList() override;
	/**
	 * Register an item with the ICNDocument.
	 */
	bool registerItem( KtlQCanvasItem *qcanvasItem ) override;

protected:
	MechanicsGroup *m_selectList;
	MechanicsSimulation *m_mechanicsSimulation;
};


#endif
