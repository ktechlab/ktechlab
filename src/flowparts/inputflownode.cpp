//
// C++ Implementation: inputflownode
//
// Description: 
//
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "inputflownode.h"
#include "flowpart.h"

InputFlowNode::InputFlowNode(ICNDocument *icnDocument, int dir, const QPoint &pos, QString *id)
 : FPNode(icnDocument, Node::fp_in, dir, pos, id)
{
}


InputFlowNode::~InputFlowNode()
{
}

FlowPart *InputFlowNode::outputFlowPart() const
{
	FlowPart *flowPart = dynamic_cast<FlowPart*>(parentItem());

	return flowPart;
}
