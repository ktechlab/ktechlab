/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <cmath>
#include <map>
#include <QSet>

#include <QDebug>

#include "circuit.h"
#include "element.h"
#include "logic.h"
#include "nonlinear.h"
#include "pin.h"
#include "reactive.h"
#include "wire.h"
#include <elementmap.h>
#include <component.h>

typedef std::multimap<int, PinSet> PinSetMap;

//BEGIN class Circuit
Circuit::Circuit() : 
		m_bCanCache(false), isSetChanged(true) {
	m_elementSet = new ElementSet(this, 0, 0); // why do we do this?
	m_pLogicCacheBase = new LogicCacheNode;
	m_elementList.clear();
}

Circuit::~Circuit() {
	delete m_elementSet;
	m_elementSet = 0;
	delete m_pLogicCacheBase;
	m_pLogicCacheBase = 0;
}

void Circuit::addPin(Pin *node) {
//	if (m_pinList.contains(node)) return;
	assert(node);
	m_pinList.insert(node);
}

int Circuit::removePin(Pin* pin)
{
    Q_ASSERT(pin);

    if(m_pinList.find(pin) != m_pinList.end()){
        m_pinList.erase(pin);
        return 0;
    }
    return -1;
}

void Circuit::addElement(Element *element) {
	assert(element);
	m_elementList.insert(element);
}

// static function
int Circuit::identifyGround(PinSet nodeList, int *highest) {
	// What this function does:
	// We are given a list of pins. First, we divide them into groups of pins
	// that are directly connected to each other (e.g. through wires or
	// switches). Then, each group of connected pins is looked at to find the
	// pin with the highest "ground priority", and this is taken to be
	// the priority of the group. The highest ground priority from all the
	// groups is recorded. If the highest ground priority found is the maximum,
	// then all the pins in groups with this priority are marked as ground
	// (their eq-id is set to -1). Otherwise, the first group of pins with the
	// highest ground priority found is marked as ground, and all others are
	// marked as non ground (their eq-id is set to 0).

	int temp_highest;

	if (!highest)
		highest = &temp_highest;

	// Now to give all the Pins ids
	PinSetMap eqs;

	while (!nodeList.empty()) {
		PinSet associated;
		PinSet nodes;
		Pin *node = *nodeList.begin();
		recursivePinAdd(node, &nodeList, &associated, &nodes);

		if (nodes.size() > 0) {
			eqs.insert(std::make_pair(associated.size(), nodes));
		}
	}

	// Now, we want to look through the associated Pins,
	// to find the ones with the highest "Ground Priority". Anything with a lower
	// priority than Pin::gt_never will not be considered
	*highest = Pin::gt_never; // The highest priority found so far

	int numGround = 0; // The number of node groups found with that priority

	const PinSetMap::iterator eqsEnd = eqs.end();
	for (PinSetMap::iterator it = eqs.begin(); it != eqsEnd; ++it) {
		int highPri = Pin::gt_never; // The highest priority found in these group of nodes
		const PinSet::iterator send = it->second.end();

		for (PinSet::iterator sit = it->second.begin(); sit != send; ++sit) {
			if ((*sit)->groundType() < highPri)
				highPri = (*sit)->groundType();
		}

		if (highPri == *highest)
			numGround++;
		else if (highPri < *highest) {
			numGround = 1;
			*highest = highPri;
		}
	}

	if (*highest == Pin::gt_never) {
		(*highest)--;
		numGround = 0;
	}

	// If there are no Always Ground nodes, then we only want to set one of the nodes as ground
	else if (*highest > Pin::gt_always)
		numGround = 1;

	// Now, we can give the nodes their cnode ids, or tell them they are ground
	bool foundGround = false; // This is only used when we don't have a Always ground node

	for (PinSetMap::iterator it = eqs.begin(); it != eqsEnd; ++it) {
		bool ground = false;

		const PinSet::iterator send = it->second.end();
		for (PinSet::iterator sit = it->second.begin(); sit != send; ++sit) {
			ground |= (*sit)->groundType() <= (*highest);
		}

		if (ground && (!foundGround || *highest == Pin::gt_always)) {
			for (PinSet::iterator sit = it->second.begin(); sit != send; ++sit) {
				(*sit)->setEqId(-1);
			}

			foundGround = true;
		} else {
			for (PinSet::iterator sit = it->second.begin(); sit != send; ++sit) {
				(*sit)->setEqId(0);
			}
		}
	}

	return numGround;
}

/// Setup the simulator!!! 
void Circuit::init() {
	unsigned branchCount = 0;

	const ElementList::iterator listEnd = m_elementList.end();
	for (ElementList::iterator it = m_elementList.begin(); it != listEnd; ++it) {
		branchCount += (*it)->numCBranches();
	}

	// Now to give all the Pins ids
	unsigned groundCount = 0;
	PinSetMap eqs;
	PinSet unassignedNodes = m_pinList;

	while (!unassignedNodes.empty()) {
		PinSet associated;
		PinSet nodes;

		if (recursivePinAdd(*unassignedNodes.begin(), &unassignedNodes, &associated, &nodes)) {
			groundCount++;
		}

		if (nodes.size() > 0) {
			eqs.insert(std::make_pair(associated.size(), nodes));
		}
	}

	unsigned cnodeCount = eqs.size() - groundCount;

	delete m_pLogicCacheBase;
	m_pLogicCacheBase = 0;

// ### ##########################################################
	delete m_elementSet;
	m_elementSet = new ElementSet(this, cnodeCount, branchCount);
// ### ##########################################################

	// Now, we can give the nodes their cnode ids, or tell them they are ground
	int i = 0;
	const PinSetMap::iterator eqsEnd = eqs.end();
	for (PinSetMap::iterator it = eqs.begin(); it != eqsEnd; ++it) {
		bool foundGround = false;
		const PinSet::iterator sEnd = it->second.end();
		for (PinSet::iterator sit = it->second.begin(); sit != sEnd; ++sit)
			foundGround |= (*sit)->eqId() == -1;

		if (foundGround) continue;

		bool foundEnergyStoragePin = false;
		for (PinSet::iterator sit = it->second.begin(); sit != sEnd; ++sit) {
			(*sit)->setEqId(i);

			bool energyStorage = false;
			const ElementList elements = (*sit)->elements();
			ElementList::const_iterator elementsEnd = elements.end();
			for (ElementList::const_iterator it = elements.begin(); it != elementsEnd; ++it) {
assert(*it);
// HACK ALERT!!! -- special knowledge of specific elements used. 
				if (((*it)->type() == Element::Element_Capacitance)
				        || ((*it)->type() == Element::Element_Inductance)) {
					energyStorage = true;
					break;
				}
			}

			// A pin attached to an energy storage pin overrides one that doesn't.
			// If the two pins have equal status with in this regard, we pick the
			// one with the highest absolute voltage on it.

			if (foundEnergyStoragePin && !energyStorage)
				continue;

			double v = (*sit)->voltage();
			if (energyStorage && !foundEnergyStoragePin) {
				foundEnergyStoragePin = true;
				m_elementSet->setXLoc(i,v);
				continue;
			}

			if (std::abs(v) > std::abs(m_elementSet->xValue(i)))
				m_elementSet->setXLoc(i,v);
		}
		i++;
	}

	// And add the elements to the elementSet
	for (ElementList::iterator it = m_elementList.begin(); it != listEnd; ++it) {
		// We don't want the element to prematurely try to do anything,
		// as it doesn't know its actual cnode ids yet
		(*it)->setCNodes();
		(*it)->setCBranches();
		m_elementSet->addElement(*it);
	}

	// And give the branch ids to the elements
	i = 0;
	for (ElementList::iterator it = m_elementList.begin(); it != listEnd; ++it) {
		switch ((*it)->numCBranches()) {
		case 0:
			break;
		case 1:
			(*it)->setCBranches(i++);
			break;
		case 2:
			(*it)->setCBranches(i, i + 1);
			i += 2;
			break;
		case 3:
			(*it)->setCBranches(i, i + 1, i + 2);
			i += 3;
			break;
		default:
			// What the?!
			break;
		}
	}
	// update the CNode IDs for the elements
    std::set<ElementMap*>::iterator elementMapEnd = m_elementMapSet.end();
    for(std::set<ElementMap*>::iterator it = m_elementMapSet.begin(); it != elementMapEnd; ++it)
        (*it)->setupCNodes();
}

void Circuit::initCache() {
	m_elementSet->updateInfo();

	m_bCanCache = true;

	m_pLogicOut.clear();

	delete m_pLogicCacheBase;
	m_pLogicCacheBase = 0;

	const ElementList::iterator end = m_elementList.end();
	for (ElementList::iterator it = m_elementList.begin(); it != end && m_bCanCache; ++it) {
		switch ((*it)->type()) {
		case Element::Element_BJT:
		case Element::Element_CCCS:
		case Element::Element_CCVS:
		case Element::Element_CurrentSource:
		case Element::Element_Diode:
		case Element::Element_JFET:
		case Element::Element_LogicIn:
		case Element::Element_MOSFET:
		case Element::Element_OpAmp:
		case Element::Element_Resistance:
		case Element::Element_VCCS:
		case Element::Element_VCVS:
		case Element::Element_VoltagePoint:
		case Element::Element_VoltageSource:
			break;
		case Element::Element_LogicOut:
			break;
		case Element::Element_CurrentSignal:
		case Element::Element_VoltageSignal:
		case Element::Element_Capacitance:
		case Element::Element_Inductance:
			m_bCanCache = false;
			break;
		}
	}

	if (!m_bCanCache) return;

	for (ElementList::iterator it = m_elementList.begin(); it != end && m_bCanCache; ++it) {
		if ((*it)->type() == Element::Element_LogicOut)
			m_pLogicOut.push_back(static_cast<LogicOut*>(*it));
	}

	m_pLogicCacheBase = new LogicCacheNode;
}

void Circuit::setCacheInvalidated() {
	if (m_pLogicCacheBase) {
		delete m_pLogicCacheBase;
		m_pLogicCacheBase = 0; 
	}
}

void Circuit::cacheAndUpdate() {
	if(!m_pLogicCacheBase) m_pLogicCacheBase = new LogicCacheNode();

	LogicCacheNode *node = m_pLogicCacheBase;

	for(unsigned i = 0; i < m_pLogicOut.size(); i++) {
		if (m_pLogicOut[i]->isHigh()) {
			node = node->addOrGetHigh();
		} else 	node = node->addOrGetLow();
	}

	QuickVector *data = node->getData();
	if (data && data->size() == m_elementSet->vectorsize()) {
		m_elementSet->loadX(data);
		m_elementSet->updateInfo();
		return;
	}

	if(m_elementSet->containsNonLinear())
		m_elementSet->doNonLinear(150, 1e-10, 1e-13);
	else	m_elementSet->doLinear(true);

	node->setData(m_elementSet);
}

// WARNING: circuitdocument also has one of these. 
bool Circuit::recursivePinAdd(Pin *node, PinSet *unassignedNodes, PinSet *associated, PinSet *nodes) {
	if (unassignedNodes->find(node) == unassignedNodes->end())
		return false;

	unassignedNodes->erase(node);
	bool foundGround = node->eqId() == -1;

	const PinSet circuitDependentPins = node->circuitDependentPins();
	const PinSet::const_iterator dEnd = circuitDependentPins.end();
	for (PinSet::const_iterator it = circuitDependentPins.begin(); it != dEnd; ++it) {
		associated->insert(*it);
	}

	nodes->insert(node);

	const PinSet localConnectedPins = node->localConnectedPins();
	const PinSet::const_iterator end = localConnectedPins.end();
	for (PinSet::const_iterator it = localConnectedPins.begin(); it != end; ++it)
		foundGround |= recursivePinAdd(*it, unassignedNodes, associated, nodes);

	return foundGround;
}

void Circuit::doNonLogic() {
//	if ( !m_elementSet || m_cnodeCount+m_branchCount <= 0 )
	if (!m_elementSet)
		return;

	if (m_bCanCache) {
		if (!m_elementSet->bChanged() && !m_elementSet->AChanged())
			return;

		cacheAndUpdate();
		updateNodalVoltages();
		m_elementSet->bUnchanged();

		return;
	}

	stepReactive();

	if (m_elementSet->containsNonLinear()) {
		m_elementSet->doNonLinear(10, 1e-9, 1e-12);
		updateNodalVoltages();
	} else {
		if (m_elementSet->doLinear(true))
			updateNodalVoltages();
	}
}

void Circuit::stepReactive() {
	ElementList::iterator listEnd = m_elementList.end();
	for (ElementList::iterator it = m_elementList.begin(); it != listEnd; ++it) {
		Element * const e = *it;

		if (e && e->isReactive())
			(static_cast<Reactive*>(e))->time_step();
	}
}

void Circuit::updateNodalVoltages() {
	const PinSet::iterator endIt = m_pinList.end();
	for (PinSet::iterator it = m_pinList.begin(); it != endIt; ++it) {
		Pin *const node = *it;
		int i = node->eqId();

		const double v = m_elementSet->cNode(i)->voltage();
		node->setVoltage(std::isfinite(v) ? v : 0.);
	}
}

void Circuit::updateCurrents() {
    // invalidate all currents on all pins
    PinSet::iterator pinsEnd = m_pinList.end();
    for(PinSet::iterator it = m_pinList.begin(); it != pinsEnd; ++it){
        (*it)->setSourceCurrent(0);
        // invalidate all the currents through the wires
        foreach(Wire *wire, (*it)->wires()){
            wire->setCurrentKnown(false);
        }
    }
    // calculate the values of the currents
	ElementList::iterator listEnd = m_elementList.end();
	for (ElementList::iterator it = m_elementList.begin(); it != listEnd; ++it) {
		(*it)->updateCurrents();
	}
    // set the currents coming from the elements
    std::set<ElementMap*>::iterator elementMapSetEnd = m_elementMapSet.end();
    for(std::set<ElementMap*>::iterator it = m_elementMapSet.begin();
                        it != elementMapSetEnd; ++it)
            (*it)->mergeCurrents();
    //
    QList<Pin*> pinsToVisit;
    QMap<Pin*,int> unknownWireCurrentCount;
    for (PinSet::iterator it = m_pinList.begin(); it != pinsEnd; ++it){
        int wireCount = (*it)->wires().size();
        unknownWireCurrentCount.insert(*it, wireCount);
        int elementCount = (*it)->elements().size();
        if( elementCount > 1){
            qCritical() << "BUG: one pin associated with multipe elements";
        }
        if( wireCount == 1 ){
            if( elementCount == 1 ){
                // we know the current
                pinsToVisit.append(*it);
            } else {
                // 1 wire, no element ?!
                qCritical() << "BUG: invalid circuit structure: pin with a wire and no element attached";
            }
        } else {
            if(wireCount == 0){
                if(elementCount == 0){
                    qCritical() << "BUG: loose pin: no element or wire connected to it";
                } else {
                    qDebug() << "found 1 unconnected element pin";
                }
            } else {
                // wireCount > 1
                if( elementCount > 0 ){
                    qCritical() << "BUG: more wires, and also elements associated with a pin";
                }
            } // wireCount > 1
        }
    }
    int visitedPinCount = 0;
    while( !pinsToVisit.empty() ){
        Pin *currentPin = pinsToVisit.takeFirst();
        if( unknownWireCurrentCount.value(currentPin) > 1 ){
            qCritical() << "BUG: pin should have at most 1 unknown associated wire current";
        }
        visitedPinCount ++;
        double currentOut = currentPin->sourceCurrent(); // zero or coming from the element
        Wire *outWire = NULL;
        foreach(Wire *awire, currentPin->wires()){
            if( awire->currentIsKnown() )
                currentOut += awire->currentFor(currentPin);
            else
                if(! outWire )
                    outWire = awire;
                else
                    qCritical() << "BUG: indeed the pin has more unknown wire currents";
        }
        // now we should know the output wire and the value of the current
        if( outWire == NULL)
            qDebug() << "All currents are known for this pin";
        else {
            outWire->setCurrentFor(currentPin, -currentOut);
            Pin *otherPin = outWire->otherPin(currentPin);
            int otherWireUnknownCount = unknownWireCurrentCount.value(otherPin);
            otherWireUnknownCount--;
            unknownWireCurrentCount.insert(otherPin, otherWireUnknownCount);
            if(otherWireUnknownCount == 1)
                pinsToVisit.append(otherPin);
            if(otherWireUnknownCount == 0){
                // the other pin has all the wires known, so check if the sum of currents is 0
                // TODO verify the currents
                double currentSum = otherPin->sourceCurrent();
                foreach(Wire *w, otherPin->wires()){
                    currentSum += w->currentFor(otherPin);
                }
                if( QABS( currentSum ) > 1e-6 ){
                    qWarning() << "BUG: current's sum is not zero in a pin, but it's"
                        << currentSum << "; current values are:";
                    QString out;
                    out.sprintf("(%.5f) ", otherPin->sourceCurrent());
                    foreach(Wire *w, otherPin->wires()){
                        QString tmp;
                        tmp.sprintf(" %.5f", w->currentFor(otherPin));
                        out.append(tmp);
                    }
                    qWarning() << out;
                }
            } // if otherWireUnknownCount == 0
        } // outWire != null
    } // while !pinsToVisit.empty()
    // statistics
    qDebug() << "visited" << visitedPinCount << "pins out of "
        << m_pinList.size() ;
}

void Circuit::addElementMap(ElementMap* em)
{
    if(!em){
        qCritical("BUG: tried to add null element map");
        return;
    }
    m_elementMapSet.insert(em);
    m_elementList.insert(em->element());
    for(int i=0; i<4; ++i)
        if( em->pin(i))
            m_pinList.insert(em->pin(i));

    // debug stuff:
    qDebug() << "circuit has: " << m_elementMapSet.size() << "ElementMaps,"
        << m_elementList.size() << "elements,"
        << m_pinList.size() << "pins";

}

void Circuit::addComponent(const Component& comp)
{
    foreach(ElementMap *map, comp.elementMapList()){
        addElementMap(map);
    }
}


void Circuit::removeElementMap(ElementMap* em)
{
    if(!em){
        qCritical("BUG: tried to remove null element map");
        return;
    }
    m_elementMapSet.erase(em);
    m_elementList.erase(em->element());
    for(int i=0; i<4; ++i)
        if( em->pin(i))
            m_pinList.erase(em->pin(i));
}

void Circuit::displayEquations() {
	m_elementSet->displayEquations();
}
//END class Circuit

//BEGIN class LogicCacheNode
LogicCacheNode::LogicCacheNode() {
	low  = 0;
	high = 0;
	data = 0;
}

LogicCacheNode::~LogicCacheNode() {
	if(low)  delete low;
	if(high) delete high;
	if(data) delete data;
}

/*!
    \fn LogicCacheNode::addOrGetHigh()
 */
LogicCacheNode *LogicCacheNode::addOrGetHigh()
{
	if(high) return high;
	return high = new LogicCacheNode();
}

/*!
    \fn LogicCacheNode::addOrGetLow()
 */
LogicCacheNode *LogicCacheNode::addOrGetLow()
{
	if(low) return low;
	return low = new LogicCacheNode();
}

/*!
    \fn LogicCacheNode::*GetData()
 */
QuickVector *LogicCacheNode::getData() const
{
	return data;
}

/*!
    \fn LogicCacheNode::setData(QuickVector *newData)
 */
void LogicCacheNode::setData(const ElementSet *aSet)
{
	const QuickVector *newData = aSet->xForCache();

	if(data) {
		if(data->size() == newData->size()) {
			*data = *newData;
			return;
		} else delete data;
	}

	data = new QuickVector(newData);
}
//END class LogicCacheNode

