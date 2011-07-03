/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ecnode.h"

#include "circuit.h"
#include "connector.h"
#include "electronicconnector.h"

#include <QDebug>

ECNode::ECNode(Circuit &c)
		: Node(), m_circuit(c) {

    // create 1 pin
    m_pins.clear();
    Pin *pin = new Pin();
    m_ownsPins = true;
    m_pins.append(pin);
    // register
    c.addPin(pin);
}

ECNode::ECNode(Circuit &c, Pin* pin): Node(), m_circuit(c)
{
    Q_ASSERT(pin);

    m_pins.clear();
    m_pins.append(pin);
    m_ownsPins = false;
    // ...
    c.addPin(pin);
}

ECNode::~ECNode() {
    // connectors
    foreach(Connector *con, m_connectorList){
        delete con;
    }
    m_connectorList.clear();
    // pins
    foreach(Pin *pin, m_pins){
        m_circuit.removePin(pin);
    }
    if(m_ownsPins){
        foreach(Pin *pin, m_pins){
            delete pin;
        }
    }
    m_pins.clear();
}

// TODO FIXME implement the removal/adding of pins from/to the circuit
// TODO FIXME deal with connectors when setting the pin count
void ECNode::setNumPins(unsigned num) {
	unsigned oldNum = m_pins.size();

	if (num == oldNum) return;

	if(!m_ownsPins)
        qCritical() << "ECNode::setNumPins: BUG: setting pin cound on a node which doesn't own its pins!\n";

    qCritical() << "ECNode::setNumPins: setting pin count is not completely implemented!" <<
        "expect crashes!";

    if(num > oldNum){
        // create pins
        for(unsigned i=oldNum; i<num; i++){
            Pin *p = new Pin();
            m_pins.append(p);
        }
    } else {
        // delete pins
        for(unsigned i=num; i<oldNum; i++){
            Pin *p = m_pins.at(i);
            m_pins.removeAt(i);
            delete p;
        }
    }
    Q_ASSERT((unsigned)m_pins.size() == num);
}

int ECNode::numPins() const
{
    return m_pins.size();
}

PinList ECNode::pins() const
{
    return m_pins;
}

Pin* ECNode::pin(unsigned int num) const
{
    if(num >= (unsigned)m_pins.size()){
        qDebug() << "BUG: tried to access nonexsitent pin in an EcNode!\n";
        return NULL;
    }
    return m_pins.at(num);
}

// -- connector related --

void ECNode::addConnector(Connector *connector) {
    Q_ASSERT(connector);
	m_connectorList.append(connector);
}

void ECNode::removeConnector(Connector *connector) {
    Q_ASSERT(connector);
    m_connectorList.removeAll(connector);
}

int ECNode::numCon() const
{
    return m_connectorList.size();
}

ConnectorList ECNode::allConnectors() const
{
    return m_connectorList;
}


#include "ecnode.moc"
