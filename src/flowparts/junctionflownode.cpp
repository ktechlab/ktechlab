//
// C++ Implementation: junctionflownode
//
// Description: 
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "junctionflownode.h"

JunctionFlowNode::JunctionFlowNode(ICNDocument* _icnView, int dir, const QPoint& pos, QString* id ): FPNode(_icnView, Node::fp_junction, dir, pos, id)
{
}


JunctionFlowNode::~JunctionFlowNode()
{
}
