/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef FPNODE_H
#define FPNODE_H

#include "node.h"


class FlowPart;
typedef QValueList<FlowPart*> FlowPartList;

/**
You should use this node for all FlowParts. It ensures that connections between FlowParts are
always valid (eg not more than two outputs from one node, which makes no sense)
@short FlowPart node
@author David Saxton
*/
class FPNode : public Node
{
Q_OBJECT
public:
	FPNode( ICNDocument *_icnView, Node::node_type type, int dir, const QPoint &pos, QString *id = 0L );
    ~FPNode();
	
	/**
	 * Returns a pointer to the FlowPart attached to this node if this node isInput, or
	 * to the other end of the connector (if one exists) if it isOutput()
	 */
	FlowPart *outputFlowPart() const;
	/** 
	 * Returns a list of FlowParts attached to the node - either a single-item list containing
	 * the FlowPart attached to this node if isOutput, or a list of FlowParts connected to the
	 * input (?) connectors
	 */
	FlowPartList inputFlowParts() const;

	/**
	 * Sets the node's visibility, as well as updating the visibility of the
	 * attached connectors as appropriate
	 */
	virtual void setVisible( bool yes );
	/**
	 * Use this to identify the type of node - eg ECNode or FPNode -- will be removed!
	 */
	// virtual node_type type() const = 0;
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
	 * Removes a specific connector
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
	/**
	 * Returns true if this node is connected (or is the same as) the node given
	 * by other connectors or nodes (although not through CNItems)
	 * checkedNodes is a list of nodes that have already been checked for
	 * being the connected nodes, and so can simply return if they are in there.
	 * If it is null, it will assume that it is the first ndoe & will create a list
	 */
	bool isConnected( Node *node, NodeList *checkedNodes = 0L );
	/**
	 * Removes all the NULL connectors
	 */
	void removeNullConnectors();
	
	
	/**
	 * Returns a list of the input connectors; implemented inline 
	 */
	ConnectorList inputConnectorList() const { return m_inputConnectorList; }
	/**
	 * Returns a list of the output connectors; implemented inline 
	 */
	ConnectorList outputConnectorList() const { return m_outputConnectorList; }
	
	/**
	 * @return the list of all the connectors attached to the node
	 */
	virtual ConnectorList getAllConnectors() const { return m_inputConnectorList + m_outputConnectorList; }
	
	/**
	 * For a flownode: returns the first input connector, if it exist, or the fist outptut connector, if it exists.
	 * For an electric node: returns the first connector
	 * If the node isn't connected to anyithing, returns null ( 0 )
	 * @return pointer to the desired connector
	 */
	virtual Connector* getAConnector() const ;
	
	
public slots:	
	
	/**
	 * what is this?
	 */
	void checkForRemoval( Connector *connector );
	
	/**
	 * Draw shape. Note that this has to remain public.
	 */
	virtual void drawShape( QPainter & p );

protected:
	
	/** If this node has precisely two connectors emerging from it, then this
	 * function will trace the two connectors until the point where they
	 * diverge; this point is returned. */
	virtual QPoint findConnectorDivergePoint( bool * found );
	
	/** (please document this) registers some signals for the node and the new connector (?) */
	bool handleNewConnector( Connector * newConnector );
	

	ConnectorList m_inputConnectorList;
	ConnectorList m_outputConnectorList;

private:
	bool m_isInput;
};

#endif

