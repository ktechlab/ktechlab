//
// C++ Interface: junctionflownode
//
// Description:
//
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef JUNCTIONFLOWNODE_H
#define JUNCTIONFLOWNODE_H

#include <fpnode.h>

/**
    A FlowNode which serves as a junction between connectors
*/
class JunctionFlowNode : public FPNode
{
public:
    JunctionFlowNode(ICNDocument *_icnView, int dir, const QPoint &pos, QString *id = nullptr);

    ~JunctionFlowNode() override;

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
public Q_SLOTS:
    /**
     * what is this?
     */
    void checkForRemoval(Connector *connector) override;
    /**
     * Draw shape. Note that this has to remain public.
     */
    void drawShape(QPainter &p) override;

protected:
    void initPoints() override;
};

#endif
