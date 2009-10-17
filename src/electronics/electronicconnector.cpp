//
// C++ Implementation: electronicconnector
//
// Description:
//
// Author: David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#include <cassert>
#include <cmath>

#include "circuiticndocument.h"
#include "electronicconnector.h"
#include "junctionnode.h"
#include "ecnode.h"
#include "wire.h"

ElectronicConnector::ElectronicConnector(ECNode *startNode, ECNode *endNode,
        ICNDocument *_ICNDocument, const QString &id)
        : Connector(_ICNDocument, id), m_currentAnimationOffset(0) {
    m_startEcNode = startNode;
    m_endEcNode = endNode;

    p_icnDocument->registerItem(this);
    p_icnDocument->requestRerouteInvalidatedConnectors();

    if (startNode && endNode) {
        connect(startNode, SIGNAL(numPinsChanged(unsigned)), this, SLOT(syncWiresWithNodes()));
        connect(endNode, SIGNAL(numPinsChanged(unsigned)), this, SLOT(syncWiresWithNodes()));
        syncWiresWithNodes();
    }
}

ElectronicConnector::~ElectronicConnector() {
// Our nodes should have been notified that we've been deleted. 
	assert(b_deleted);

/* 
I think what's going on here is that even though we create wires, they're *always* put under the control of Pins. 
Therefore, it is the pins that are responsible for deleting them. As long as the assertion above always passes, then we
know we've done our job and told both of our ECNodes that we're gone and the pins have been told to remove our wires. 

Whether things actually work that way is another matter.
*/

    /* Even though we segfault here, we MUST be able to delete wires from this class!
    Therefore we need to make sure our list of wires is accurate before we go into self destruct.
 
    unsigned size = m_wires.size();

    for (unsigned i = 0; i < size; i++)
        delete m_wires[i];   */
}

void ElectronicConnector::syncWiresWithNodes() {

    ECNode *startEcNode = m_startEcNode;
    ECNode *endEcNode = m_endEcNode;

    if (!startEcNode || !endEcNode) return;

    // FIXME more dynamic_cast to avoid using type() member
    const bool isStartNodeJunction = dynamic_cast<JunctionNode*>(startEcNode) != 0;
    const bool isEndNodeJunction   = dynamic_cast<JunctionNode*>(endEcNode)   != 0;
    unsigned newNumWires = 0;

    if (isStartNodeJunction || isEndNodeJunction)
        newNumWires = QMAX(startEcNode->numPins(), endEcNode->numPins());
    else newNumWires = QMIN(startEcNode->numPins(), endEcNode->numPins());

    unsigned oldNumWires = m_wires.size();

    if (newNumWires == oldNumWires) return;

// Critical section? ################3
    if (isStartNodeJunction)
        startEcNode->setNumPins(newNumWires);

    if (isEndNodeJunction)
        endEcNode->setNumPins(newNumWires);

// ####################################

    if (newNumWires > oldNumWires) {
        m_wires.resize(newNumWires);

        for (unsigned i = oldNumWires; i < newNumWires; i++) {
            if (startEcNode->pin(i) && endEcNode->pin(i))
                m_wires[i] = new Wire(startEcNode->pin(i), endEcNode->pin(i));
        }
    } else {
        for (unsigned i = newNumWires; i < oldNumWires; i++)
            delete m_wires[i];

        m_wires.resize(newNumWires);
    }

    updateConnectorLines();

// emit numWiresChanged(newNumWires);
}

void ElectronicConnector::incrementCurrentAnimation(double deltaTime) {
    // The values and equations used in this function have just been developed
    // empircally to be able to show a nice range of currents while still giving
    // a good indication of the amount of current flowing

    double I_min = 1e-4;
    double sf    = 3.0; // scaling factor

    for (unsigned i = 0; i < m_wires.size(); ++i) {
        assert(m_wires[i]);

        double I = m_wires[i]->current();
        double sign  = (I > 0) ? 1 : -1;
        double I_abs = I * sign;
        double prop  = (I_abs > I_min) ? std::log(I_abs / I_min) : 0.0;

        m_currentAnimationOffset += deltaTime * sf * std::pow(prop, 1.3) * sign;
    }
}

#include "electronicconnector.moc"
