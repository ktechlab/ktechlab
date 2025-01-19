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

//#include <canvas.h> // 2018.10.16 - not needed
#include "canvasitems.h"
#include <QPointer>

class CNItem;
class Item;
class ICNDocument;
class ICNDocument;
class Connector;
class Node;
class NodeData;
class NodeGroup;
class QTimer;

typedef QList<QPointer<Connector>> ConnectorList;
typedef QList<QPointer<Node>> NodeList;

/**
@short A standard node that can be associated with a Connector or a CNItem
@author David Saxton
*/
class Node : /* public QObject, */ public KtlQCanvasPolygon
{
    Q_OBJECT
public:
    // this shall disappear one day
    /**
     * Used for run-time identification of the node:
     * Can be electronic node (so has values of current, voltage, etc)
     * or a pic part node
     * this enum will be cleared soon
     */
    enum node_type { ec_pin, ec_junction, fp_in, fp_out, fp_junction };

    /**
     * @param dir the direction of the node; 0 degrees for left, 90 degrees for
     * up, etc in an anti-clockwise direction. An "up" node has the
     * wire-connection point at the top and the (component/flowpart)-end at the
     * bottom.
     */
    Node(ICNDocument *icnDocument, Node::node_type type, int dir, const QPoint &pos, QString *id = nullptr);
    ~Node() override;

    /**
     * Sets the node's visibility, as well as updating the visibility of the
     * attached connectors as appropriate
     */
    void setVisible(bool yes) override;
    /**
     * Returns the global id, that is unique to the node
     * amongst all the nodes on the canvas
     */
    const QString id() const
    {
        return m_id;
    }
    /**
     * Returns the id that is internal to the CNItem to which the
     * node belongs to. Returns a null QString if no parentitem
     */
    const QString childId() const
    {
        return m_childId;
    }
    /**
     * Use this function to set the child-id, that is unique to the node
     * amongst the other nodes associated with its parent CNItem
     */
    void setChildId(const QString &id)
    {
        m_childId = id;
    }
    /**
     * Sets the "level" of the node. By default, the level is 0. The level of
     * the node tells the node what CNItems it can be connected to through
     * a connector.
     * @see level
     */
    virtual void setLevel(const int level);
    /**
     * Returns the level of the nodes
     * @see setLevel
     */
    int level() const
    {
        return m_level;
    }

    /**
     * Sets the orientation of the node.
     */
    void setOrientation(int dir);
    /**
     * Changes the length of the node. By default, this is 8. Some node types
     * (such as junctions) do not make use of this value.
     */
    void setLength(int length);
    /**
     * Associates a CNItem with the node - ie the node belongs to the CNItem,
     * and hence gets deleted when the CNItem gets deleted.s
     */
    virtual void setParentItem(CNItem *parentItem);
    /**
     * Returns true if the node is part of a CNItem
     * (i.e. not between multiple connectors)
     */
    bool isChildNode() const
    {
        return (p_parentItem != nullptr);
    }
    /**
     * Returns a pointer to the CNItem to which the node belongs,
     * or Null if it doesn't.
     */
    CNItem *parentItem() const
    {
        return p_parentItem;
    }

    NodeData nodeData() const;

    void setNodeGroup(NodeGroup *ng)
    {
        p_nodeGroup = ng;
    }
    NodeGroup *nodeGroup() const
    {
        return p_nodeGroup;
    }

    /* interface common to ecnode and fpnode; these might be required by ItemDocumentData, ICNDocument  */

    virtual bool isConnected(Node *node, NodeList *checkedNodes = nullptr) = 0;

    virtual void removeConnector(Connector *connector) = 0;

    /**
     * Returns the total number of connections to the node. This is the number
     * of connectors and the parent
     * item connector if it exists and is requested.
     * @param includeParentItem Count the parent item as a connector if it exists
     * @param includeHiddenConnectors hidden connectors are those as e.g. part of a subcircuit
     */
    virtual int numCon(bool includeParentItem, bool includeHiddenConnectors) const = 0;

    /**
     * @return the list of all the connectors attached to the node
     */
    virtual ConnectorList getAllConnectors() const = 0;

    /**
     * For a flownode: returns the first input connector, if it exist, or the fist outptut connector, if it exists.
     * For an electric node: returns the first connector
     * If the node isn't connected to anyithing, returns null ( 0 )
     * @return pointer to the desired connector
     */
    virtual Connector *getAConnector() const = 0;

    /**
     * Removes all the NULL connectors
     */
    virtual void removeNullConnectors() = 0;

    /**
     * Draw shape. Note that this has to remain public.
     */
    void drawShape(QPainter &p) override = 0;

    void setICNDocument(ICNDocument *documentPtr);

public slots:
    void moveBy(double dx, double dy) override;
    void removeNode(Item *)
    {
        removeNode();
    }
    void removeNode();
    void setNodeSelected(bool yes);

signals:
    void moved(Node *node);
    /**
     * Emitted when the CNItem is removed. Normally, this signal is caught by associated
     * nodes, who will remove themselves as well.
     */
    void removed(Node *node);

protected:
    virtual void initPoints();
    /**
     * Moves and rotates (according to m_dir) the painter, so that our current
     * position is (0,0).
     */
    void initPainter(QPainter &p);
    /**
     * Undoes the effects of initPainter.
     */
    void deinitPainter(QPainter &p);

    /** If this node has precisely two connectors emerging from it, then this
     * function will trace the two connectors until the point where they
     * diverge; this point is returned. */
    virtual QPoint findConnectorDivergePoint(bool *found) = 0;

    /** The node's type. This member will be removed! */
    node_type m_type;

    int m_dir;
    int m_length;
    int m_level;

    ICNDocument *p_icnDocument;
    CNItem *p_parentItem;

    NodeGroup *p_nodeGroup;

    static QColor m_selectedColor;

private:
    // these fields are critical to saved circuit documents.
    QString m_id;
    QString m_childId;

    bool b_deleted;
};

#endif
