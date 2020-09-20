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
    a FlowNode which serves as Output for flowparts
*/
class OutputFlowNode : public FPNode
{
public:
    OutputFlowNode(ICNDocument *_icnView, int dir, const QPoint &pos, QString *id = nullptr);

    ~OutputFlowNode() override;

    /**
     * Returns true if the node can accept input connections. This will depend
     * on the node type and number of input / output connections.
     */
    bool acceptInput() const override;
    /**
     * Returns true if the node can accept output connections. This will depend
     * on the node type and number of input / output connections.
     */
    bool acceptOutput() const override;

    /**
     * Registers an input connector (i.e. this is the end node) as connected
     * to this node.
     */
    void addInputConnector(Connector *const connector) override;

public slots:

    /**
     * Draw shape. Note that this has to remain public.
     */
    void drawShape(QPainter &p) override;
};

#endif
