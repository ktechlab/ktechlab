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
	MechanicsDocument( const QString &caption, const char *name = 0 );
	~MechanicsDocument();

	virtual View *createView( ViewContainer *viewContainer, uint viewAreaId, const char *name = 0l ) override;

	virtual bool isValidItem( const QString &itemId ) override;
	virtual bool isValidItem( Item *item ) override;

	virtual void deleteSelection() override;
	virtual void copy() override;
	virtual void selectAll() override;
	virtual ItemGroup *selectList() const override;
	MechanicsItem *mechanicsItemWithID( const QString &id );
	virtual Item* addItem( const QString &id, const QPoint &p, bool newItem ) override;
	/**
	 * Adds a KtlQCanvasItem to the delete list to be deleted, when
	 * flushDeleteList() is called
	 */
	virtual void appendDeleteList( KtlQCanvasItem *qcanvasItem ) override;
	/**
	 * Permantly deletes all items that have been added to the delete list with
	 * the appendDeleteList( KtlQCanvasItem *qcanvasItem ) function.
	 */
	virtual void flushDeleteList() override;
	/**
	 * Register an item with the ICNDocument.
	 */
	virtual bool registerItem( KtlQCanvasItem *qcanvasItem ) override;

protected:
	MechanicsGroup *m_selectList;
	MechanicsSimulation *m_mechanicsSimulation;
};


#endif
