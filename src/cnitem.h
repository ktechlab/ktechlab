/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CNITEM_H
#define CNITEM_H

#include "item.h"
#include "ciwidgetmgr.h"


class Button;
class CNItem;
class ICNDocument;
class Connector;
class DoubleSpinBox;
class LibraryItem;
class Node;
class QSlider;
class QString;
class QToolButton;
class QMatrix;
class Slider;
class Text;


class NodeInfo
{
public:
	NodeInfo();

	QString id; // External id (ICNDocument scope)
	Node *node; //Pointer to the node
	double x; // X position relative to item
	double y; // Y position relative to item
	int orientation; // Orientation relative to item
};

typedef QMap<QString, QString> StringMap;
typedef QMap<QString, NodeInfo> NodeInfoMap; // Internal id, node info
typedef QList<QPointer<Connector> > ConnectorList;
typedef QMap<QString, QPointer<Text> > TextMap;

/**
Essentially, all items that live on ICNDocument should inherit from this class.
This class provides much functionality (moving items, creation of associated nodes,
saving and editing of associated data, cutting / copying, etc)
@short Base class for all components/flowparts/etc
@author Daniel Clarke
@author David Saxton
*/
class CNItem : public Item, public CIWidgetMgr
{
Q_OBJECT
public:
	CNItem( ICNDocument *_icnView, bool newItem, const QString &id );
	~CNItem() override;
	
	/**
	 * Creates a node which is attached to the item. The node will be moved
	 * about with the item, and destroyed along with the item. The position
	 * coordinates of the node are relative to the upper left corner of the item.
	 * @param type See Node::node_type
	 */
	Node* createNode( double _x, double _y, int orientation, const QString &name, uint type );
	/**
	 * Removes a child node. You should use this function if you want to remove
	 * any nodes during the lifetime of the CNItem.
	 */
	bool removeNode( const QString &name );
	/**
	 * Returns the closest node that is associated with the CNItem
	 */
	Node *getClosestNode( const QPoint &pos );
	/**
	 * Returns a list of connectors associated with the CNItem
	 */
	ConnectorList connectorList();
	bool preResize( QRect sizeRect ) override;
	bool mousePressEvent( const EventInfo &eventInfo ) override;
	bool mouseReleaseEvent( const EventInfo &eventInfo ) override;
	bool mouseDoubleClickEvent ( const EventInfo &eventInfo ) override;
	bool mouseMoveEvent( const EventInfo &eventInfo ) override;
	bool wheelEvent( const EventInfo &eventInfo ) override;
	void enterEvent(QEvent *) override;
	void leaveEvent(QEvent *) override;
	/**
	 * ICNDocument needs to know what 'cells' a CNItem is present in,
	 * so that connection mapping can be done to avoid CNItems.
	 * This function will add the hit penalty to the cells pointed to
	 * by ICNDocument::cells()
	 */
	virtual void updateConnectorPoints( bool add );
	/**
	 * Converts the id used to internally identify a node to the global
	 * ICNDocument node id. eg "vss" might return "node__13".
	 */
	QString nodeId( const QString &internalNodeId );
	/**
	 * Returns a pointer to the node with the given internal (child) id
	 */
	Node *childNode( const QString &childId );
	/**
	 * Returns the node map used:
	 * QMap<QString, NodeInfo> NodeInfoMap
	 * It's probably best to cache this data
	 */
	NodeInfoMap nodeMap() const { return m_nodeMap; }
	/**
	 * Returns the TextMap used for canvas text
	 */
	TextMap textMap() const { return m_textMap; }
	void setVisible( bool yes ) override;
	void updateZ( int baseZ ) override;

	ItemData itemData() const override;
	void restoreFromItemData( const ItemData &itemData ) override;
	virtual void updateNodeLevels();
	void drawShape( QPainter &p ) override;
	
signals:
	/**
	 * Emitted when the angle or flipped'ness changes. Note that CNItem doesn't
	 * actually emit this signal - instead, Component and FlowPart classes do.
	 */
	void orientationChanged();

public slots:
	/**
	 * Moves item - use this instead of moveBy() so that associated Nodes also get moved
	 */
	void moveBy( double dx, double dy ) override;
	/**
	 * Remove the item and associated nodes. It appends the item to the
	 * ICNDocument's delete list, so you must call ICNDocument::flushDeleteList()
	 * after calling this (and possible ICNDocument::clearDeleteList() befor
	 * calling it) The virtual function void handleRemove() is called to allow
	 * any child classes to clear up any neccessary data (which doesn't do
	 * anything by default), before CNItem does the rest
	 */
	void removeItem() override;
	/**
	 * This item has been resized, so update the nodes relative positions
	 */
	virtual void updateAttachedPositioning();

protected:
	void reparented( Item *oldParent, Item *newParent ) override;
	void postResize() override;
	/**
	 * CNItem handles drawing of text associated with the CNItem.
	 * @param id is a unique identifier that can be used to change the text displayed.
	 * @param pos is the position that the text occupies relative to the top left corner of the CNItem.
	 * @param display is the actual text to be displayed.
	 * @param internal is used to determine the z-level of the text - whether it should be below or above the item
	 * @param flags Text alignment flags - Qt::AlignmentFlags and Qt::TextFlags OR'd together.
	 */
	Text* addDisplayText( const QString &id, const QRect & pos, const QString &display, bool internal = true, int flags = Qt::AlignHCenter | Qt::AlignVCenter );
	void setDisplayText( const QString &id, const QString &display );
	/**
	 * Remove the display text with the given id
	 */
	void removeDisplayText( const QString &id );
	/**
	 * Sets the right colour if selected, transforms the matrix of the painter
	 */
	virtual void initPainter( QPainter &p );

	QPointer<ICNDocument> p_icnDocument;
	TextMap m_textMap;
	NodeInfoMap m_nodeMap;
	QColor m_selectedCol;
	QColor m_brushCol;
	bool b_pointsAdded;
};
typedef QList<CNItem*> CNItemList;

#endif

