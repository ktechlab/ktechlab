//
// C++ Interface: outputflownode
//
// Description: 
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef OUTPUTFLOWNODE_H
#define OUTPUTFLOWNODE_H

#include <fpnode.h>

/**
	A FPNode with type() == fp_out
*/
class OutputFlowNode : public FPNode
{
public:
    OutputFlowNode(ICNDocument* _icnView, int dir, const QPoint& pos, QString* id = 0L);

    ~OutputFlowNode();

    virtual node_type type() const { return Node::fp_out; }

};

#endif
