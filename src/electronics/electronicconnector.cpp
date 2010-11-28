//
// C++ Implementation: electronicconnector
//
// Description:
//
// Author: David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//

#include "electronicconnector.h"
#include "wire.h"
#include "ecnode.h"

#include "QDebug"

ElectronicConnector::ElectronicConnector(ECNode *startNode, ECNode *endNode)
        : Connector() {
    Q_ASSERT(startNode);
    Q_ASSERT(endNode);

    int wirecount = startNode->numPins();

    // same size?
    if( startNode->numPins() != endNode->numPins() ){
        qCritical() << "BUG: connecting 2 nodes with different pin count!"
            << "Going for minimum!\n";
        if(startNode->numPins() < endNode->numPins())
            wirecount = startNode->numPins();
        else
            wirecount = endNode->numPins();
    }

    // nodes
    m_startEcNode = startNode;
    m_endEcNode = endNode;

    // create wires
    m_wires.clear();
    for(int i=0; i<wirecount; i++){
        Wire *w = new Wire( startNode->pin(i), endNode->pin(i));
        m_wires.append(w);
    }
}

ElectronicConnector::~ElectronicConnector() {
    // wires
    foreach(Wire *w, m_wires){
        delete w;
    }
    m_wires.clear();
    m_startEcNode = 0;
    m_endEcNode = 0;
}

Node* ElectronicConnector::startNode() const
{
    return m_startEcNode;
}

Node* ElectronicConnector::endNode() const
{
    return m_endEcNode;
}

int ElectronicConnector::numWires() const
{
    return m_wires.size();
}

Wire* ElectronicConnector::wire(unsigned int num) const
{
    if(num >= m_wires.size()){
        qCritical() << "BUG: trying to access inexistent wire in a connector!\n";
        return NULL;
    }
    return m_wires.at(num);
}

#include "electronicconnector.moc"
