//
// C++ Implementation: outputflownode
//
// Description: 
//
//
// Author: David Saxton <david@bluehaze.org>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "outputflownode.h"

OutputFlowNode::OutputFlowNode(ICNDocument* _icnView, int dir, const QPoint& pos, QString* id): FPNode(_icnView, Node::fp_out, dir, pos, id)
{
}


OutputFlowNode::~OutputFlowNode()
{
}

