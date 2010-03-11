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
 a FPNode with type() == fp_in
*/
class InputFlowNode : public FPNode
{
public:
	InputFlowNode(ICNDocument *icnDocument, int dir, const QPoint &pos, QString *id = 0L);

	~InputFlowNode();

	/** Returns the node's type. This member will be removed one day */
	node_type type() const { return Node::fp_in; }
};

#endif
