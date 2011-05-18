//
// C++ Interface: inputflownode
//
// Description:
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef INPUTFLOWNODE_H
#define INPUTFLOWNODE_H

#include <fpnode.h>

/**
 a FlowNode which serves as an input for a flowpart
*/
class InputFlowNode : public FPNode
{
	public:
		InputFlowNode ( ICNDocument *icnDocument, int dir, const QPoint &pos, QString *id = 0L );

		~InputFlowNode();


		/**
		 * Returns a pointer to the FlowPart attached to this node if this node isInput, or
		 * to the other end of the connector (if one exists) if it isOutput()
		 */
		virtual FlowPart *outputFlowPart() const;

		/**
		 * Returns a list of FlowParts attached to the node - either a single-item list containing
		 * the FlowPart attached to this node if isOutput, or a list of FlowParts connected to the
		 * input (?) connectors
		 */
		virtual FlowPartList inputFlowParts() const;

		/**
		 * Returns true if the node can accept input connections. This will depend
		 * on the node type and number of input / output connections.
		 */
		virtual bool acceptInput() const;
		/**
		 * Returns true if the node can accept output connections. This will depend
		 * on the node type and number of input / output connections.
		 */
		virtual bool acceptOutput() const;
		
		/**
		 * Registers an input connector (i.e. this is the start node) as connected
		 * to this node.
	 	*/
		virtual void addOutputConnector( Connector * const connector );		
		
	public slots:


		/**
		 * Draw shape. Note that this has to remain public.
		 */
		virtual void drawShape ( QPainter & p );
};

#endif
