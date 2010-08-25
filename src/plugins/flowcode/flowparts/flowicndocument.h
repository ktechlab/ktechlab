//
// C++ Interface: flowicndocument
//
// Description:
//
//
// Author: Zoltan P <zoltan.padrah@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef FLOWICNDOCUMENT_H
#define FLOWICNDOCUMENT_H

#include <icndocument.h>

class Node;
class FPNode;

typedef QMap< QString, FPNode* > FPNodeMap;

/**
	A document holding a flow-diagram
	@author Zoltan P <zoltan.padrah@gmail.com>
*/
class FlowICNDocument : public ICNDocument
{
Q_OBJECT

public:
	FlowICNDocument(const QString &caption, const char *name);

	virtual ~FlowICNDocument();

	/**
	 * Reinherit this function to perform special checks on whether the two
	 * given QCanvasItems (either nodes or connectors or both) can be
	 * connected together.
	 */
	virtual bool canConnect ( QCanvasItem *qcanvasItem1, QCanvasItem *qcanvasItem2 ) const;
	
	/**
	 * Splits Connector con into two connectors at point pos2, and creates a connector from the node
	 * to the intersection of the two new connectors. If pointList is non-null, then the new connector
	 * from the node will be assigned those points
	 */
	virtual Connector *createConnector( Node *node, Connector *con, const QPoint &pos2, QPointList *pointList = 0);
	/**
	 * Splits con1 and con2 into two new connectors each at points pos1 and pos2, and creates a new connector
	 * between the two points of intersection given by pos1 and pos2. If pointList is non-null, then the new
	 * connector between the two points will be assigned those points
	 */
	virtual Connector *createConnector( Connector *con1, Connector *con2, const QPoint &pos1, const QPoint &pos2, QPointList *pointList = 0L );
	/**
	 * Creates a connector between two nodes, and returns a pointer to it
	 * and adds the operation to the undo list
	 */
	virtual Connector *createConnector( const QString &startNodeId, const QString &endNodeId, QPointList *pointList = 0);


	/**
	 * Returns a pointer to a node on the canvas with the given id,
	 * or NULL if no such node exists
	 */
	virtual Node* nodeWithID( const QString &id );
	FPNode* getFPnodeWithID( const QString &id );
	/**
	 * Assigns the orphan nodes into NodeGroups. You shouldn't call this
	 * function directly - instead use ItemDocument::requestEvent.
	 */
	virtual void slotAssignNodeGroups();	
	
	/**
	 * Permantly deletes all items that have been added to the delete list with
	 * the appendDeleteList( QCanvasItem *qcanvasItem ) function.
	 */
	virtual void flushDeleteList();

	/**
	 * registers (adds to the document) an item (a connector or a node)
	 * @param qcanvasItem the item to be registered
	 * @return true if succeeded, false if it didn't
	 */
	virtual bool registerItem( QCanvasItem *qcanvasItem );
	virtual void unregisterUID( const QString & uid );
	virtual NodeList nodeList() const;
protected:
	
	/**
	 * If there are two connectors joined to a node, then they can be merged
	 * into one connector. The node will not be removed.
	 * @param node The node between the two connectors
	 * @param noCreate If true, no new connectors will be created
	 * @returns true if it was successful in merging the connectors
	 */
	bool joinConnectors( FPNode *fpnode );
	/**
	 *        Selects all nodes on the document. Should be overridden.
	 */
	virtual void selectAllNodes();	

	
	/**
	 *        deletes all the elements containde in the nodeList. Should be overridden.
	 */
	virtual void deleteAllNodes();
	

	/// the list of flownodes in the documents
	FPNodeMap m_flowNodeList;
	
};

#endif
