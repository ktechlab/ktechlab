/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef FLOWCONTAINER_H
#define FLOWCONTAINER_H

#include "flowpart.h"

class RectangularOverlay;

/**
@author David Saxton
*/
class FlowContainer : public FlowPart
{
Q_OBJECT
public:
	FlowContainer( ICNDocument *_icnView, bool newItem, const QString &id );
	virtual ~FlowContainer();
	
	virtual bool canResize() const { return true; }
	/**
	 * Sets the bound to a simple rectangle if true, so that ICNDocument
	 * can tell whether an item is being dropped into it
	 */
	void setFullBounds( bool full );
	
	virtual void updateConnectorPoints( bool add = true );
	/**
	 * Returns whether the container is currently expanded or not
	 */
	bool isExpanded() const { return b_expanded; }
	/**
	 * Returns true if one of this parents is collapsed.
	 */
	bool parentIsCollapsed() const;
	void setExpanded( bool expanded );

	virtual void setSelected( bool yes );
	virtual void setVisible( bool yes );
	
	virtual QSize minimumSize() const;
	/**
	 * Update the visibility of items, connectors, nodes in the flowcontainer
	 */
	void updateContainedVisibility();
	
protected:
	virtual void itemPointsChanged() {};
	virtual void updateNodeLevels();
	virtual void childAdded( Item *child );
	virtual void childRemoved( Item *child );
	virtual void updateAttachedPositioning();
	virtual void postResize();
	virtual void filterEndPartIDs( QStringList *ids );
	virtual void drawShape( QPainter &p );
	void createTopContainerNode();
	void createBotContainerNode();
	
	virtual void buttonStateChanged(const QString &id, bool state);

	FPNode *m_ext_in;
	FPNode *m_int_in;
	FPNode *m_int_out;
	FPNode *m_ext_out;
	RectangularOverlay *m_rectangularOverlay;

	bool b_expanded;
};

#endif
