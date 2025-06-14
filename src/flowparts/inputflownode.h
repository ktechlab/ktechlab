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
    InputFlowNode(ICNDocument *icnDocument, int dir, const QPoint &pos, QString *id = nullptr);

    ~InputFlowNode() override;

    /**
     * Returns a pointer to the FlowPart attached to this node if this node isInput, or
     * to the other end of the connector (if one exists) if it isOutput()
     */
    FlowPart *outputFlowPart() const override;

    /**
     * Returns a list of FlowParts attached to the node - either a single-item list containing
     * the FlowPart attached to this node if isOutput, or a list of FlowParts connected to the
     * input (?) connectors
     */
    FlowPartList inputFlowParts() const override;

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
     * Registers an input connector (i.e. this is the start node) as connected
     * to this node.
     */
    void addOutputConnector(Connector *const connector) override;

public Q_SLOTS:

    /**
     * Draw shape. Note that this has to remain public.
     */
    void drawShape(QPainter &p) override;
};

#endif
