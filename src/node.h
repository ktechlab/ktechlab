/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef NODE_H
#define NODE_H

#include <canvas.h>
#include <qguardedptr.h>

class CNItem;
class Item;
class ICNDocument;
class ICNDocument;
class Connector;
class Node;
class NodeData;
class NodeGroup;
class QTimer;

typedef QValueList<QGuardedPtr<Connector> > ConnectorList;
typedef QValueList<QGuardedPtr<Node> > NodeList;

/**
@short A standard node that can be associated with a Connector or a CNItem
@author David Saxton
*/
class Node : public QObject, public QCanvasPolygon
{
Q_OBJECT
public:
	/**
	 * Used for run-time identification of the node:
	 * Can be electronic node (so has values of current, voltage, etc)
	 * or a pic part node
	 */
	enum node_type
	{
		ec_pin,
		ec_junction,
		fp_in,
		fp_out,
		fp_junction
	};
	
	/**
	 * @param dir the direction of the node; 0 degrees for left, 90 degrees for
	 * up, etc in an anti-clockwise direction. An "up" node has the
	 * wire-connection point at the top and the (component/flowpart)-end at the
	 * bottom.
	 */
	Node( ICNDocument *icnDocument, Node::node_type type, int dir, const QPoint &pos, QString *id = 0L );
	virtual ~Node();
	
	/**
	 * Sets the node's visibility, as well as updating the visibility of the
	 * attached connectors as appropriate
	 */
	virtual void setVisible( bool yes );
	/**
	 * Returns the global id, that is unique to the node
	 * amongst all the nodes on the canvas
	 */
	const QString id() const { return m_id; }
	/**
	 * Returns the id that is internal to the CNItem to which the
	 * node belongs to. Returns a null QString if no parentitem
	 */
	const QString childId() const { return m_childId; }
	/**
	 * Use this function to set the child-id, that is unique to the node
	 * amongst the other nodes associated with its parent CNItem
	 */
	void setChildId( const QString &id ) { m_childId = id; }
	/**
	 * Sets the "level" of the node. By default, the level is 0. The level of
	 * the node tells the node what CNItems it can be connected to through
	 * a connector.
	 * @see level
	 */
	virtual void setLevel( const int level );
	/** 
	 * Returns the level of the nodes
	 * @see setLevel
	 */
	int level() const { return m_level; }
	/**
	 * Use this to identify the type of node - eg ECNode or FPNode
	 */
	node_type type() const { return m_type; }
	/**
	 * Returns true if the node can accept input connections. This will depend
	 * on the node type and number of input / output connections.
	 */
	bool acceptInput() const;
	/**
	 * Returns true if the node can accept output connections. This will depend
	 * on the node type and number of input / output connections.
	 */
	bool acceptOutput() const;
	/**
	 * Sets the orientation of the node.
	 */
	void setOrientation( int dir );
	/**
	 * Changes the lenght of the node. By default, this is 8. Some node types
	 * (such as junctions) do not make use of this value.
	 */
	void setLength( int length );
	/**
	 * Associates a CNItem with the node - ie the node belongs to the CNItem,
	 * and hence gets deleted when the CNItem gets deleted.s
	 */
	virtual void setParentItem( CNItem *parentItem );
	/**
	 * Returns true if the node is part of a CNItem
	 * (i.e. not between multiple connectors)
	 */
	bool isChildNode() const { return (p_parentItem != 0L); }
	/**
	 * Returns a pointer to the CNItem to which the node belongs,
	 * or Null if it doesn't.
	 */
	CNItem *parentItem() const { return p_parentItem; }
	/**
	 * Remove a specific connector
	 */
	void removeConnector( Connector *connector );
	/**
	 * Creates a new connector, sets this as the end node to the connector
	 * (i.e. this node is the connector's input node), and returns a pointer
	 * to the connector.
	 */
	Connector* createInputConnector( Node * startNode );
	/**
	 * Registers an input connector (i.e. this is the end node) as connected
	 * to this node.
	 */
	void addInputConnector( Connector * const connector );
	/**
	 * Registers an input connector (i.e. this is the start node) as connected
	 * to this node.
	 */
	void addOutputConnector( Connector * const connector );
	/**
	 * Returns the total number of connections to the node. This is the number
	 * of input connectors, the number of output connectors, and the parent
	 * item connector if it exists and is requested.
	 * @param includeParentItem Count the parent item as a connector if it exists
	 * @param includeHiddenConnectors hidden connectors are those as e.g. part of a subcircuit
	 */
	int numCon( bool includeParentItem, bool includeHiddenConnectors ) const;
	
	NodeData nodeData() const;
	
	ConnectorList inputConnectorList() const { return m_inputConnectorList; }
	ConnectorList outputConnectorList() const { return m_outputConnectorList; }
	
	void setNodeGroup( NodeGroup *ng ) { p_nodeGroup = ng; }
	NodeGroup *nodeGroup() const { return p_nodeGroup; }
	
	/**
	 * Returns true if this node is connected (or is the same as) the node given
	 * by other connectors or nodes (although not through CNItems)
	 * checkedNodes is a list of nodes that have already been checked for
	 * being the connected nodes, and so can simply return if they are in there.
	 * If it is null, it will assume that it is the first ndoe & will create a list
	 */
	bool isConnected( Node *node, NodeList *checkedNodes = 0L );
	
	void removeNullConnectors();
	/**
	 * Draw shape. Note that this has to remain public.
	 */
	virtual void drawShape( QPainter &p ) = 0;
	
public slots:
	void moveBy( double dx, double dy );
	void removeNode(Item*) { removeNode(); }
	void removeNode();
	void checkForRemoval( Connector *connector );
	void setNodeSelected( bool yes );
	
signals:
	void moved( Node *node );
	/**
	 * Emitted when the CNItem is removed. Normally, this signal is caught by associated
	 * nodes, who will remove themselves as well.
	 */
	void removed( Node* node );
	
protected:
	/** If this node has precisely two connectors emerging from it, then this
	 * function will trace thw two connectors until the point where they
	 * diverge; this point is returned. */
	QPoint findConnectorDivergePoint( bool * found );
	void initPoints();
	bool handleNewConnector( Connector * newConnector );
	/**
	 * Moves and rotates (according to m_dir) the painter, so that our current
	 * position is (0,0).
	 */
	void initPainter( QPainter & p );
	/**
	 * Undoes the effects of initPainter.
	 */
	void deinitPainter( QPainter & p );
	
	node_type m_type;
	QString m_id;
	QString m_childId;
	int m_dir;
	int m_length;
	ICNDocument *p_icnDocument;
	CNItem *p_parentItem;
	ConnectorList m_inputConnectorList;
	ConnectorList m_outputConnectorList;
	int m_level;
	NodeGroup *p_nodeGroup;
	static QColor m_selectedColor;

private:
	bool b_deleted;
};

#endif
