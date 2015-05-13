/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MECHANICSITEM_H
#define MECHANICSITEM_H

#include <item.h>
#include <Qt/qlist.h>

class LibraryItem;
class MechanicsItem;
// class MechanicsItemOverlayItem;
class MechanicsDocument;
typedef QList<MechanicsItem*> MechanicsItemList;

/**
@short Stores mass, moment of inertia
@author David Saxton
*/
class MechanicsInfo
{
public:
	MechanicsInfo();
	
	double mass; // Mass
	double momentOfInertia; // Moment of inertia
};

class CombinedMechanicsInfo : public MechanicsInfo
{
public:
	CombinedMechanicsInfo();
	CombinedMechanicsInfo( const MechanicsInfo &info );
	
	double x; // X coordinate of center of mass
	double y; // Y coordinate of center of mass
};

/**
@short Stores a position and orientation
@author David Saxton
*/
class PositionInfo
{
public:
	PositionInfo();
	/**
	 * Adds together two positions: for this=PARENT +(CHILD), the new position
	 * is formed by translating this position by that of the CHILDs
	 * translation, and then rotating everything about the center of this item
	 */
	const PositionInfo operator+( const PositionInfo &info );
	/**
	 * Not quite the inverse of operator+. Subtracts the given position info
	 * as if it was applied before this current info.
	 */
	const PositionInfo operator-( const PositionInfo &info );
	/**
	 * x position (0 is left)
	 */
	double x() const { return m_x; }
	/**
	 * y position (0 is top)
	 */
	double y() const { return m_y; }
	/**
	 * Angle in radians, positive direction is anticlockwise
	 */
	double angle() const { return m_angle; }
	/**
	 * Sets the x-position
	 */
	void setX( double x ) { m_x = x; }
	/**
	 * Sets the y-position
	 */
	void setY( double y ) { m_y = y; }
	/**
	 * Sets the angle
	 */
	void setAngle( double angle ) { m_angle = angle; }
	/**
	 * Adds (x,y) to the current position
	 */
	void translate( double dx, const double dy ) { m_x += dx; m_y += dy; }
	/**
	 * Rotates anticlockwise by the given amount (in radians)
	 */
	void rotate( double angle ) { m_angle += angle; }
	/**
	 * Resets the position to (0,0), and the orientation to 0
	 */
	void reset();
	/**
	 * Rotates the current position about the given point through the given
	 * angle in radians anticlockwise. This will change the position and
	 * orientation.
	 */
	void rotateAboutPoint( double x, double y, double angle );
	
protected:
	double m_x;
	double m_y;
	double m_angle;
};


/**
@author David Saxton
*/
class MechanicsItem : public Item
{
Q_OBJECT
public:
	MechanicsItem( MechanicsDocument *mechanicsDocument, bool newItem, const QString &id );
	virtual ~MechanicsItem();
	
	enum SelectionMode
	{
		sm_move,
		sm_resize,
		sm_rotate
	};
	/**
	 * Sets the selection mode (sm_resize or sm_rotate). Note that setSelected
	 * also needs to be called to select the item.
	 */
	void setSelectionMode( SelectionMode sm );
	virtual void setSelected( bool yes );
	/**
	 * @returns the selection mode
	 */
	SelectionMode selectionMode() const { return m_selectionMode; }
	/**
	 * Move the MechanicsItem by the given amount
	 */
	virtual void moveBy( double dx, double dy );
	/**
	 * Returns the absolute position on the canvas
	 */
	PositionInfo absolutePosition() const;
	/**
	 * Returns the position relative to the parent item (or the absolute
	 * position if there is no parent item)
	 */
	PositionInfo relativePosition() const { return m_relativePosition; }
	/**
	 * Returns the mechanics info for this item (so not taking into account that
	 * of attached children)
	 */
	MechanicsInfo *mechanicsInfo() { return &m_mechanicsInfo; }
	/**
	 * Returns the combined mechanics info for this item (which takes into
	 * account that of attached children).
	 */
	CombinedMechanicsInfo *mechanicsInfoCombined() { return &m_mechanicsInfoCombined; }
	/**
	 * Returns the rectangle that can legitimately fit inside the given bounding
	 * rectangle, given this items current rotation. Legitimately means that
	 * whether this item is allowed to be distorted, inverted, resized, etc.
	 */
	QRect maxInnerRectangle( const QRect &outerRect ) const;
	
	virtual ItemData itemData() const;
	
	virtual bool mousePressEvent( const EventInfo &eventInfo );
	virtual bool mouseReleaseEvent( const EventInfo &eventInfo );
	virtual bool mouseDoubleClickEvent ( const EventInfo &eventInfo );
	virtual bool mouseMoveEvent( const EventInfo &eventInfo );
	virtual bool wheelEvent( const EventInfo &eventInfo );
	virtual void enterEvent(QEvent *);
	virtual void leaveEvent(QEvent *);
	
public slots:
	/**
	 * Rotate the item by the given amount (in radians)
	 */
	void rotateBy( double dtheta );
	void parentMoved();
	 
signals:
	/**
	 * Emitted when this item moves (translates or rotates)
	 */
	void moved();
	
protected slots:
	/**
	 * Recalculate the combined mechanics info (e.g. when mass is changed, or child added)
	 */
	void updateMechanicsInfoCombined();
	
protected:
	virtual void reparented( Item *oldItem, Item *newItem );
	virtual void childAdded( Item *child );
	virtual void childRemoved( Item *child );
	/**
	 * Called when this item is resized, so that sub classes can do whatever
	 */
	virtual void itemResized() {};
	/**
	 * Sets the correct orientation on the painter
	 */
	void initPainter( QPainter &p );
	/**
	 * *Must* be called after calling initPainter, if initPainter was called
	 */
	void deinitPainter( QPainter &p );
	virtual void dataChanged();
	virtual void itemPointsChanged() { updateCanvasPoints(); }
	/**
	 * Calculates the setPoints required from the current m_itemPoints and the
	 * current position / angle
	 */
	void updateCanvasPoints();
	
	MechanicsDocument *p_mechanicsDocument;
	PositionInfo m_relativePosition; // Absolution position if not attached to a parent item, or otherwise relative to parent item
	MechanicsInfo m_mechanicsInfo;
	CombinedMechanicsInfo m_mechanicsInfoCombined;
	
private:
	SelectionMode m_selectionMode;
};

#endif
