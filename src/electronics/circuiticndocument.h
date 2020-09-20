//
// C++ Interface: circuiticndocument
//
// Description:
//
//
// Author: Zoltan P <zoltan.padrah@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CIRCUITICNDOCUMENT_H
#define CIRCUITICNDOCUMENT_H

#include <icndocument.h>

class ECNode;

typedef QMap<QString, ECNode *> ECNodeMap;

/**
A document holding a circuit

    @author Zoltan P <zoltan.padrah@gmail.com>
*/
class CircuitICNDocument : public ICNDocument
{
    Q_OBJECT

public:
    CircuitICNDocument(const QString &caption, const char *name);

    ~CircuitICNDocument() override;

    /**
     * Reinherit this function to perform special checks on whether the two
     * given QCanvasItems (either nodes or connectors or both) can be
     * connected together.
     */
    bool canConnect(KtlQCanvasItem *qcanvasItem1, KtlQCanvasItem *qcanvasItem2) const override;

    /**
     * Splits Connector con into two connectors at point pos2, and creates a connector from the node
     * to the intersection of the two new connectors. If pointList is non-null, then the new connector
     * from the node will be assigned those points
     */
    Connector *createConnector(Node *node, Connector *con, const QPoint &pos2, QPointList *pointList = nullptr) override;
    /**
     * Splits con1 and con2 into two new connectors each at points pos1 and pos2, and creates a new connector
     * between the two points of intersection given by pos1 and pos2. If pointList is non-null, then the new
     * connector between the two points will be assigned those points
     */
    Connector *createConnector(Connector *con1, Connector *con2, const QPoint &pos1, const QPoint &pos2, QPointList *pointList = nullptr) override;
    /**
     * Creates a connector between two nodes, and returns a pointer to it
     * and adds the operation to the undo list
     */
    Connector *createConnector(const QString &startNodeId, const QString &endNodeId, QPointList *pointList = nullptr) override;
    /**
     * Creates a connector from node1 to node2. If pointList is non-null, then the
     * connector will be assigned those points
     */
    virtual Connector *createConnector(Node *node1, Node *node2, QPointList *pointList = nullptr)
    {
        return ICNDocument::createConnector(node1, node2, pointList);
    }

    /**
     * Returns a pointer to a node on the canvas with the given id,
     * or nullptr if no such node exists
     */
    Node *nodeWithID(const QString &id) override;
    ECNode *getEcNodeWithID(const QString &id);

    /**
     * Assigns the orphan nodes into NodeGroups. You shouldn't call this
     * function directly - instead use ItemDocument::requestEvent.
     */
    void slotAssignNodeGroups() override;

    /**
     * Permantly deletes all items that have been added to the delete list with
     * the appendDeleteList( KtlQCanvasItem *qcanvasItem ) function.
     */
    void flushDeleteList() override;

    /**
     * registers (adds to the document) an item (a connector or a node)
     * @param qcanvasItem the item to be registered
     * @return true if succeeded, false if it didn't
     */
    bool registerItem(KtlQCanvasItem *qcanvasItem) override;

    void unregisterUID(const QString &uid) override;

    NodeList nodeList() const override;

protected:
    /**
     * If there are two connectors joined to a node, then they can be merged
     * into one connector. The node will not be removed.
     * @param ecnode The node between the two connectors
     * @returns true if it was successful in merging the connectors
     */
    bool joinConnectors(ECNode *ecnode);
    /**
     *        Selects all nodes on the document. Should be overridden.
     */
    void selectAllNodes() override;

    /**
     * 	deletes all the elements containde in the nodeList. Should be overridden.
     */
    void deleteAllNodes() override;

    /**
     *        deletes all node groups in the nodeGroupList. Should be overridden.
     */
    // virtual void deleteAllNodeGroups();

    /// the list of nodes contained by the document
    ECNodeMap m_ecNodeList;
};

#endif
