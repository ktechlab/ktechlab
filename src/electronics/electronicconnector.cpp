//
// C++ Implementation: electronicconnector
//
// Description: 
//
//
// Author: David Saxton, Alan Grimes, Zoltan Padrah <zoltan.padrah@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "electronicconnector.h"

#include "junctionnode.h"
#include "ecnode.h"
#include "wire.h"

ElectronicConnector::ElectronicConnector(ECNode* startNode, ECNode* endNode, ICNDocument* _ICNDocument, const QString &id): Connector(_ICNDocument, id)
{
	m_startEcNode = startNode;
	m_endEcNode = endNode;
	
	if( startNode && endNode ) {
		connect(startNode, SIGNAL(numPinsChanged(unsigned)), this, SLOT(syncWiresWithNodes()));
		connect(endNode, SIGNAL(numPinsChanged(unsigned)), this, SLOT(syncWiresWithNodes()));
		syncWiresWithNodes();
	}
}


ElectronicConnector::~ElectronicConnector()
{
}


void ElectronicConnector::syncWiresWithNodes() {

	ECNode * startEcNode = m_startEcNode;
	ECNode * endEcNode = m_endEcNode;
	
	if (!startEcNode || !endEcNode) return;

	// FIXME more dynamic_cast to avoid using type() member
	const bool isStartNodeJunction = dynamic_cast<JunctionNode*>(startEcNode) != 0;
	const bool isEndNodeJunction   = dynamic_cast<JunctionNode*>(endEcNode)   != 0;

	unsigned newNumWires = 0;

	if (isStartNodeJunction || isEndNodeJunction)
		newNumWires = QMAX(startEcNode->numPins(), endEcNode->numPins());
	else	newNumWires = QMIN(startEcNode->numPins(), endEcNode->numPins());

	unsigned oldNumWires = m_wires.size();

	if (newNumWires == oldNumWires) return;

// Critical section? ################3
//	m_bIsSyncingWires = true;
	if (isStartNodeJunction)
		startEcNode->setNumPins(newNumWires);

	if (isEndNodeJunction)
		endEcNode->setNumPins(newNumWires);

//	m_bIsSyncingWires = false;
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
	emit numWiresChanged(newNumWires);
}

#include "electronicconnector.moc"
