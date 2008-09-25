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
	InputFlowNode(ICNDocument *icnDocument, int dir, const QPoint &pos, QString *id = 0L);

	~InputFlowNode();

	
	/**
	 * Returns a pointer to the FlowPart attached to this node if this node isInput, or
	 * to the other end of the connector (if one exists) if it isOutput()
	 */
	FlowPart *outputFlowPart() const;	
	
};

#endif
