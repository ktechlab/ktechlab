/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ICNDOCUMENT_H
#define ICNDOCUMENT_H

#include "itemdocument.h"

#include <qmap.h>

class Cells;
class CNItem;
class CNItemGroup;
class Connector;
class ECNode;
class FlowContainer;
class Node;
class NodeGroup;

typedef QMap< QString, Node* > NodeMap;
typedef QList<QPointer<Connector> > ConnectorList;
typedef QList<QPointer<Node> > NodeList;
typedef QList<NodeGroup*> NodeGroupList;
typedef QList<QPointer<NodeGroup> > GuardedNodeGroupList;

/**
@author David Saxton
*/
class ICNDocument : public ItemDocument
{
Q_OBJECT
public:
	ICNDocument( const QString &caption, const char *name );
	
	virtual ~ICNDocument();
	
	enum hit_score
	{
		hs_none = 0,
		hs_connector = 4,
		hs_item = 1000
	};

	virtual View *createView( ViewContainer *viewContainer, uint viewAreaId, const char *name = 0l );
	
	/**
	 * Will attempt to create an item with the given id at position p. Some item
	 * (such as PIC/START) have restrictions, and can only have one instance of
	 * themselves on the canvas, and adds the operation to the undo list
	 */
	virtual Item* addItem( const QString &id, const QPoint &p, bool newItem );

	/**
	 * short for casting whatever itemWithID(id) returns
	 */
	CNItem* cnItemWithID( const QString &id );
	/**
	 * Returns a pointer to a node on the canvas with the given id,
	 * or NULL if no such node exists
	 */
	virtual Node* nodeWithID( const QString &id ) = 0;
	/**
	 * Returns a pointer to a Connector on the canvas with the given id,
	 * or NULL if no such Connector exists
	 */
	Connector* connectorWithID( const QString &id );
	/**
	 * Adds a KtlQCanvasItem to the delete list to be deleted,
	 * when flushDeleteList() is called
	 */
	virtual void appendDeleteList( KtlQCanvasItem *qcanvasItem );
	/**
	 * Permantly deletes all items that have been added to the delete list with
	 * the appendDeleteList( KtlQCanvasItem *qcanvasItem ) function.
	 */
	virtual void flushDeleteList() = 0;
	/**
	 * Reinherit this function to perform special checks on whether the two
	 * given QCanvasItems (either nodes or connectors or both) can be
	 * connected together.
	 */
	virtual bool canConnect( KtlQCanvasItem *qcanvasItem1, KtlQCanvasItem *qcanvasItem2 ) const;
	/**
	 *        copies the selected items to the clipboard, in an XML text form
	 */
	virtual void copy();
	/**
	 *        selects everything in the current document
	 */
	virtual void selectAll();


	/**
	 * registers (adds to the document) an item (a connector or a node)
	 * @param qcanvasItem the item to be registered
	 * @return true if succeeded, false if it didn't
	 */
	virtual bool registerItem( KtlQCanvasItem *qcanvasItem );
	/**
	 * Returns a pointer to the 2-dimension array of ICNDocument cells.
	 */
	Cells *cells() const { return m_cells; }
	/**
	 * Adds score to the cells at the given cell referece
	 */
	void addCPenalty( int x, int y, int score );
	/**
	 * If there are two connectors joined to a node, then they can be merged
	 * into one connector. The node will not be removed.
	 * @param node The node between the two connectors
	 * @param noCreate If true, no new connectors will be created
	 * @returns true if it was successful in merging the connectors
	 */
	// bool joinConnectors( Node *node );
	/**
	 * Snaps a coordinate in the document to the grid
	 * @param pos The coordinate
	 * @return The snapped to grid coordinate
	 */
	static int gridSnap( int pos ); /// Returns 'pos' when snapped to grid
	/**
	 * Snaps a point to the grid
	 * @param pos The point
	 * @return The adjusted coordinate
	 */
	static QPoint gridSnap( const QPoint &pos );
	/**
	 * Returns true if the CNItem is valid - e.g. will return true for a
	 * component in a circuit, but not in a pic program
	 */
	virtual bool isValidItem( Item *item ) = 0;
	virtual bool isValidItem( const QString &itemId ) = 0;
	
	// TODO to document
	virtual ConnectorList getCommonConnectors( const ItemList &list );
	virtual NodeList getCommonNodes( const ItemList &list );
	
	/**
	 * returns all the nodes contained by the document. Note that this function is inefficient, 
	 * so don't use it in loops
	 * @return all the nodes contained by the document
	 */
	virtual NodeList nodeList() const = 0;
	
	/**
	 * @return all the connectors from the document
	 */
	const ConnectorList & connectorList() const { return m_connectorList; }
	
	/**
	 * @return all the nodegroups from the document
	 */
	const GuardedNodeGroupList & nodeGroupList() const { return m_nodeGroupList; }
	
	/**
	 * @return the selected items from the document
	 */
	virtual ItemGroup *selectList() const;
	
	/**
	 * Creates a connector between two nodes, and returns a pointer to it
	 * and adds the operation to the undo list
	 */
	virtual Connector* createConnector( const QString &startNodeId, const QString &endNodeId, QPointList *pointList = 0L ) = 0;
	/**
	 * Creates a connector from node1 to node2. If pointList is non-null, then the
	 * connector will be assigned those points
	 */
	//virtual

	Connector *createConnector( Node *node1, Node *node2, QPointList *pointList = 0);
	/**
	 * Splits Connector con into two connectors at point pos2, and creates a connector from the node
	 * to the intersection of the two new connectors. If pointList is non-null, then the new connector
	 * from the node will be assigned those points
	 */
	virtual Connector * createConnector( Node *node, Connector *con, const QPoint &pos2, QPointList *pointList = 0L ) = 0;
	/**
	 * Splits con1 and con2 into two new connectors each at points pos1 and pos2, and creates a new connector
	 * between the two points of intersection given by pos1 and pos2. If pointList is non-null, then the new
	 * connector between the two points will be assigned those points
	 */
	virtual Connector * createConnector( Connector *con1, Connector *con2, const QPoint &pos1, const QPoint &pos2, QPointList *pointList = 0L ) = 0;
	/**
	 * Returns the flowcontainer at the given position at the highest level that
	 * is not in the current select list, or 0l if there isn't one
	 */
	FlowContainer *flowContainer( const QPoint &pos );
	/**
	 * Sets the drag (e.g. horizontal arrow) cursor for resizing a CNItem, depending on the corner clicked on
	 */
	void setItemResizeCursor( int cornerType );
	
	void getTranslatable( const ItemList & itemList, ConnectorList * fixedConnectors = 0l, ConnectorList * translatableConnectors = 0l, NodeGroupList * translatableNodeGroups = 0l );
	
	/**
	 * Reroutes invalidated directors. You shouldn't call this function
	 * directly - instead use ItemDocument::requestEvent.
	 */
	void rerouteInvalidatedConnectors();
	/**
	 * Assigns the orphan nodes into NodeGroups. You shouldn't call this
	 * function directly - instead use ItemDocument::requestEvent.
	 */
	virtual void slotAssignNodeGroups();
	
	virtual void unregisterUID( const QString & uid );
	
public slots:
	/**
	 * Deletes all items in the selected item list, along with associated
	 * connectors, etc, and adds the operation to the undo list
	 */
	virtual void deleteSelection();
	/**
	 * This function looks at all the connectors and the nodes, determines
	 * which ones need rerouting, and then reroutes them
	 */
	void requestRerouteInvalidatedConnectors();
	/**
	 * Remaps the 2-dimension array of ICNDocument cells, and the various
	 * hitscores / etc associated with them. This is used for connector
	 * routing, and should be called after e.g. items have been moved
	 */
	void createCellMap();
	/**
	 * Call this to request NodeGroup reassignment.
	 */
	void slotRequestAssignNG();

signals:
	/**
	 * Emitted when a Connector is added
	 */
	void connectorAdded( Connector *connector );
	/**
	 * Emitted when a Node is added
	 */
	void nodeAdded( Node *node );

protected:
	/**
	 * Adds all connector points from the items (used in connector routing).
	 * This only needs to be called when connector(s) need routing.
	 */
	void addAllItemConnectorPoints();
	
	virtual void fillContextMenu( const QPoint &pos );
	/**
	 * Creates a new NodeGroup to control the node, if there does not already
	 * exist a NodeGroup containing the given node. The associated nodes will
	 * also be added to the NodeGroup.
	 * @returns a pointer to the NodeGroup if one was created, or a pointer to the existing one containing that node
	 */
	NodeGroup* createNodeGroup( Node *node );
	/**
	 * Finds (and deletes if found) the NodeGroup containing the given node.
	 * @returns true if the NodeGroup was found and deleted
	 */
	bool deleteNodeGroup( Node *node );

	friend class CanvasEditor;
	
	/**
	 *        deletes all the elements containde in the nodeList. Should be overridden.
	 */
	virtual void deleteAllNodes() = 0;
	
	/**
	 *        Selects all nodes on the document. Should be overridden.
	 */
	virtual void selectAllNodes() = 0;

	// this should be overridden in {Flow|Circuit}ICNDocument
	ConnectorList m_connectorList;
	CNItemGroup *m_selectList; // Selected objects

	// OVERLOADED	
	KtlQCanvasItemList m_itemDeleteList; // List of canvas items to be deleted

private:
	Cells *m_cells;
	GuardedNodeGroupList m_nodeGroupList;

};

/**
@author David Saxton
*/
class DirCursor
{
public:
	static DirCursor* self();
	~DirCursor();
	
	static QPixmap leftArrow()
	{
		return self()->m_leftArrow;
	}
	
	static QPixmap rightArrow()
	{
		return self()->m_rightArrow;
	}
	
	static QPixmap upArrow()
	{
		return self()->m_upArrow;
	}
	
	static QPixmap downArrow()
	{
		return self()->m_downArrow;
	}
	
protected:
	DirCursor();
	void initCursors();
	
	static DirCursor *m_self;
	QPixmap m_leftArrow;
	QPixmap m_rightArrow;
	QPixmap m_upArrow;
	QPixmap m_downArrow;
};


#endif
