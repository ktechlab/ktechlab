/***************************************************************************
 *   Copyright (C) 2004-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ITEM_H
#define ITEM_H

#include "variant.h"
#include "itemdocument.h"

#include <canvas.h>
#include <qpointer.h>

class Document;
class EventInfo;
class Item;
class ItemData;
class ItemDocument;
class ItemView;
class DoubleSpinBox;
class Document;
class Variant;
class QBitArray;
class QTimer;

typedef Variant Property;

typedef Item*(*createItemPtr)( ItemDocument *itemDocument, bool newItem, const char *id );
typedef QPointer<Item> GuardedItem;
typedef QMap<QString, Variant*> VariantDataMap;
typedef QList<GuardedItem> ItemList;

/**
@author David Saxton
@author Daniel Clarke
*/
class Item : /* public QObject, */ public KtlQCanvasPolygon
{
Q_OBJECT
public:
	Item( ItemDocument *itemDocument, bool newItem, const QString &id );
	virtual ~Item();

	/**
	 * @return Pointer to the VariantMap used for internal data storage
	 */
	VariantDataMap *variantMap() { return &m_variantData; }
	
	double dataDouble( const QString & id ) const;
	int dataInt( const QString & id ) const;
	bool dataBool( const QString & id ) const;
	QString dataString( const QString & id ) const;
	QColor dataColor( const QString & id ) const;
	
	virtual Property * createProperty( const QString & id, Variant::Type::Value type );
	Property * property( const QString & id ) const;
	bool hasProperty( const QString & id ) const;
	
	/**
	 * Whether or not we can resize the item
	 */
	virtual bool canResize() const { return false; }
	/**
	 * Returns whether the CNItem allows itself to be moved on the canvas.
	 * Most do, but some (such as the PicItem) don't allow this
	 */
	virtual bool isMovable() const { return true; }
	/**
	 * Returns whether or not what the item is displaying has (possibly) changed
	 * since this function was last called. If your item doesn't move, yet still
	 * continously changes what is being displayed (such as a seven segment
	 * display or a lamp), then set m_bDynamicContent to be true in the
	 * constructor or reinherit this to return true when the contents of the
	 * item have changed since this function was last called.
	 */
	virtual bool contentChanged() const { return m_bDynamicContent; }
	/**
	 * Returns a identifier for the CNItem, which is unique on the ICNDocument
	 */
	QString id() const { return m_id; }
	QString type() const { return m_type; }
	/**
	 * @return the font used for drawing items. This is taken to be the
	 * standard desktop font, limited to a size of 12 pixels.
	 */
	QFont font() const;
	/**
	 * Called from ItemLibrary after this class and subclasses have finished
	 * constructing themselves.
	 */
	virtual void finishedCreation();
	/**
	 * Sets the selected flag of the item to yes. selected or unselected will be
	 * emitted as appropriate
	 */
	virtual void setSelected( bool yes );
	/**
	 * Convenience function for setting the item bounding area as changed on the
	 * canvas
	 */
	void setChanged();
	/**
	 * Sets this item as a child of the given item. Calls reparented with the
	 * old and the new parent.
	 */
	void setParentItem( Item *parentItem );
	/**
	 * The parent item for this item, or NULL if none
	 */
	Item *parentItem() const { return p_parentItem; }
	ItemDocument *itemDocument() const { return p_itemDocument; }
	/**
	 * Returns the number of items away from the top item this is
	 * (parent-wise). Returns 0 if has no parent.
	 */
	int level() const;
	/**
	 * If true, then adds ItemDocument::Z::(RaisedItem-Item) to the z value of
	 * the item.
	 */
	void setRaised( bool isRaised );
	/**
	 * @Returns whether raised or not
	 */
	bool isRaised() const { return m_bIsRaised; }
	/**
	 * Sets this item to the given baseZ level, and calls this function for the
	 * children with baseZ incremented by one. Reinherit this function to set
	 * the Z of attached stuff (such as nodes).
	 */
	virtual void updateZ( int baseZ );
	/**
	 * Returns the item's position in the overall z-stack of items.
	 */
	int baseZ() const { return m_baseZ; }
	/**
	 * Adds a child. Calls the virtual function childAdded.
	 */
	void addChild( Item *child );
	/**
	 * Returns the list of children.
	 * @param if includeGrandChildren is true then this list will also contain
	 * the children's children, and so on recursively, instead of just the
	 * immediate children.
	 */
	ItemList children( bool includeGrandChildren = false ) const;
	/**
	 * Returns whether we have the given child as either a direct child, or as
	 * either a direct or indirect child
	 */
	bool contains( Item *item, bool direct = false ) const;
	/**
	 * Calls prePresize with the bounds, and if that returns true, sets
	 * m_sizeRect to the given rect, and then calls postResize. The center of
	 * \p sizeRect is taken as the point of rotation.
	 * @param forceItemPoints if true, will set the item points to a rectangle
	 * @of the given size. Pass true if you have already set the size, and want
	 * to update the appearance and bounding of the item.
	 */
	void setSize( QRect sizeRect, bool forceItemPoints = false );
	/**
	 * Convenience function.
	 * @see setSize( QRect sizeRect, bool forceItemPoints );
	 */
	void setSize( int x, int y, int w, int h, bool forceItemPoints = false ) { setSize( QRect(x,y,w,h), forceItemPoints ); }
	/**
	 * @returns the m_sizeRect rectangble that contains the item points
	 */
	QRect sizeRect() const { return m_sizeRect; }
	/**
	 * Reinherit this function if you want to determine what the minimum size is
	 * that this item can be resized to.
	 */
	virtual QSize minimumSize() const { return QSize(0,0); }
	int offsetX() const { return m_sizeRect.x(); }
	int offsetY() const { return m_sizeRect.y(); }
	int width() const { return m_sizeRect.width(); }
	int height() const { return m_sizeRect.height(); }
	virtual bool mousePressEvent( const EventInfo &eventInfo );
	virtual bool mouseReleaseEvent( const EventInfo &eventInfo );
	virtual bool mouseDoubleClickEvent ( const EventInfo &eventInfo );
	virtual bool mouseMoveEvent( const EventInfo &eventInfo );
	virtual bool wheelEvent( const EventInfo &eventInfo );
	virtual void enterEvent(QEvent *);
	virtual void leaveEvent(QEvent *);
	/**
	 * Returns the name of the CNItem, e.g. "Resistor"
	 */
	QString name() const { return m_name; }
	/**
	 * Modifies the exponent of the number so that it appears readable:
	 * eg 10000->10, 174822->175, 0.6->600, etc
	 */
	static int getNumberPre( double num );
	/**
	 * Returns the SI exponent of the number as a letter:
	 * eg 10000 returns 'k', 0.6 returns 'm', etc
	 */
	static QString getNumberMag( double num );
	/**
	 * Returns the multiplier required to get the num up to human readable form:
	 * eg 10000 returns 0.001, etc
	 */
	static double getMultiplier( double num );
	/**
	 * Returns the multiplier required to get the num from human readable form
	 * to its actual value based on the SI exponent:
	 * eg 'm' returns 0.001, etc
	 */
	static double getMultiplier( const QString &mag );
	
	virtual ItemData itemData() const;
	virtual void restoreFromItemData( const ItemData &itemData );
	
public slots:
	virtual void removeItem();
	/**
	 * Moves item - use this instead of moveBy() so that associated Nodes also get moved
	 */
	virtual void moveBy( double dx, double dy );
	/**
	 * Removes a child. Calls the virtual function childRemoved
	 */
	void removeChild( Item *child );
	
signals:
	/**
	 * Emitted when the CNItem is removed. Normally, this signal is caught by associated
	 * nodes, who will remove themselves as well.
	 */
	void removed( Item *item );
	/**
	 * Emitted when the item is selected or unselected.
	 */
	void selectionChanged();
	/**
	 * Emitted when the item is resized (after calling postResize)
	 */
	void resized();
	/**
	 * Emitted when the item is moved (by dx, dy).
	 */
	void movedBy( double dx, double dy );
	
protected slots:
	virtual void propertyChangedInitial();
	virtual void dataChanged() {};
	
protected:
	/**
	 * Reinherit this function if you want to do anything with children. Called
	 * after the parent is changed, with the old parent and the new parent.
	 */
	virtual void reparented( Item */*oldParent*/, Item */*newParent*/ ) {};
	/**
	 * Reinherit this function if you want to do anything with children. Called
	 * after a child has been added.
	 */
	virtual void childAdded( Item * ) {};
	/**
	 * Reinherit this function if you want to do anything with children. Called
	 * after a child has been removed.
	 */
	virtual void childRemoved( Item * ) {};
	/**
	 * Set the rough bounding points for this item. Calls itemPointsChanged
	 * after setting the points
	 */
	void setItemPoints( const Q3PointArray &pa, bool setSizeFromPoints = true );
	/**
	 * Reinherit this function if you want to apply any sort of transformation
	 * to the item points
	 */
	virtual void itemPointsChanged();
	virtual bool preResize( QRect sizeRect ) { Q_UNUSED(sizeRect); return true; }
	virtual void postResize() {};
	
	QString m_id;
	QString m_name; ///< Name (e.g. "Resistor")
	QString m_type;
	GuardedItem p_parentItem; // If attached to a parent item
	ItemList m_children;
	QPointer<ItemDocument> p_itemDocument;
	Q3PointArray m_itemPoints; // The unorientated and unsized item points
	QTimer * m_pPropertyChangedTimer; ///< Single show timer for one a property changes
	
	friend class ItemLibrary;
	
	int m_baseZ;
	bool m_bIsRaised;
	bool m_bDoneCreation;
	bool b_deleted;
	bool m_bDynamicContent;
	QRect m_sizeRect;
	VariantDataMap m_variantData;
};

#endif
