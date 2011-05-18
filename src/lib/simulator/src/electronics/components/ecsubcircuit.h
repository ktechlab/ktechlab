/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ECSUBCIRCUIT_H
#define ECSUBCIRCUIT_H

#include <dipcomponent.h>

#include <q3valuevector.h>

/**
"Container" component for subcircuits
@author David Saxton
*/
class ECSubcircuit : public DIPComponent
{
Q_OBJECT
public:
	ECSubcircuit( ICNDocument *icnDocument, bool newItem, const char *id = 0L );
	~ECSubcircuit();
	
	static Item* construct( ItemDocument *itemDocument, bool newItem, const char *id );
	static LibraryItem *libraryItem();
	
	/**
	 * Create numExtCon nodes, deleting any old ones
	 */
	void setNumExtCon( unsigned numExtCon );
	/**
	 * Give the connecting node at position numId the given name
	 */
	void setExtConName( unsigned numId, const QString & name );
	/**
	 * Called from SubcircuitData once the subcircuit has been fully attached
	 */
	void doneSCInit();
	
public slots:
	virtual void removeItem();
	
signals:
	/**
	 * Emitted when the current subcircuit is deleted
	 */
	void subcircuitDeleted();
	
protected:
	virtual void dataChanged();
	virtual void drawShape( QPainter &p );
	Q3ValueVector<QString> m_conNames;
};

#endif
