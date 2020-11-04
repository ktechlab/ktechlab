/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "circuit.h"
#include "circuitdocument.h"
#include "element.h"
#include "elementset.h"
#include "logic.h"
#include "nonlinear.h"
#include "pin.h"
#include "reactive.h"
#include "wire.h"

//#include <vector>
#include <cmath>
#include <map>

typedef std::multimap<int, PinList> PinListMap;

// BEGIN class Circuit
Circuit::Circuit()
{
    m_bCanAddChanged = true;
    m_pNextChanged[0] = m_pNextChanged[1] = nullptr;
    m_logicOutCount = 0;
    m_bCanCache = false;
    m_pLogicOut = nullptr;
    m_elementSet = new ElementSet(this, 0, 0); // why do we do this?
    m_cnodeCount = m_branchCount = -1;
    m_prepNLCount = 0;
    m_pLogicCacheBase = new LogicCacheNode;
}

Circuit::~Circuit()
{
    delete m_elementSet;
    delete m_pLogicCacheBase;
    delete[] m_pLogicOut;
}

void Circuit::addPin(Pin *node)
{
    if (m_pinList.contains(node))
        return;
    m_pinList.append(node);
}

void Circuit::addElement(Element *element)
{
    if (m_elementList.contains(element))
        return;
    m_elementList.append(element);
}

bool Circuit::contains(Pin *node)
{
    return m_pinList.contains(node);
}

// static function
int Circuit::identifyGround(PinList nodeList, int *highest)
{
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
    PinListMap eqs;
    while (!nodeList.isEmpty()) {
        PinList associated;
        PinList nodes;
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
    int numGround = 0;        // The number of node groups found with that priority
    const PinListMap::iterator eqsEnd = eqs.end();
    for (PinListMap::iterator it = eqs.begin(); it != eqsEnd; ++it) {
        int highPri = Pin::gt_never; // The highest priority found in these group of nodes
        const PinList::iterator send = it->second.end();
        for (PinList::iterator sit = it->second.begin(); sit != send; ++sit) {
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
    for (PinListMap::iterator it = eqs.begin(); it != eqsEnd; ++it) {
        bool ground = false;
        const PinList::iterator send = it->second.end();
        for (PinList::iterator sit = it->second.begin(); sit != send; ++sit) {
            ground |= (*sit)->groundType() <= (*highest);
        }
        if (ground && (!foundGround || *highest == Pin::gt_always)) {
            for (PinList::iterator sit = it->second.begin(); sit != send; ++sit) {
                (*sit)->setEqId(-1);
            }
            foundGround = true;
        } else {
            for (PinList::iterator sit = it->second.begin(); sit != send; ++sit) {
                (*sit)->setEqId(0);
            }
        }
    }

    return numGround;
}

void Circuit::init()
{
    m_branchCount = 0;

    const ElementList::iterator listEnd = m_elementList.end();
    for (ElementList::iterator it = m_elementList.begin(); it != listEnd; ++it) {
        m_branchCount += (*it)->numCBranches();
    }

    // Now to give all the Pins ids
    int groundCount = 0;
    PinListMap eqs;
    PinList unassignedNodes = m_pinList;
    while (!unassignedNodes.isEmpty()) {
        PinList associated;
        PinList nodes;
        Pin *node = *unassignedNodes.begin();
        if (recursivePinAdd(node, &unassignedNodes, &associated, &nodes)) {
            groundCount++;
        }
        if (nodes.size() > 0) {
            eqs.insert(std::make_pair(associated.size(), nodes));
        }
    }

    m_cnodeCount = eqs.size() - groundCount;

    delete m_pLogicCacheBase;
    m_pLogicCacheBase = nullptr;

    delete m_elementSet;
    m_elementSet = new ElementSet(this, m_cnodeCount, m_branchCount);

    // Now, we can give the nodes their cnode ids, or tell them they are ground
    QuickVector *x = m_elementSet->x();
    int i = 0;
    const PinListMap::iterator eqsEnd = eqs.end();
    for (PinListMap::iterator it = eqs.begin(); it != eqsEnd; ++it) {
        bool foundGround = false;

        const PinList::iterator sEnd = it->second.end();
        for (PinList::iterator sit = it->second.begin(); sit != sEnd; ++sit)
            foundGround |= (*sit)->eqId() == -1;

        if (foundGround)
            continue;

        bool foundEnergyStoragePin = false;

        for (PinList::iterator sit = it->second.begin(); sit != sEnd; ++sit) {
            (*sit)->setEqId(i);

            bool energyStorage = false;
            const ElementList elements = (*sit)->elements();
            ElementList::const_iterator elementsEnd = elements.end();
            for (ElementList::const_iterator it = elements.begin(); it != elementsEnd; ++it) {
                if (!*it)
                    continue;

                if (((*it)->type() == Element::Element_Capacitance) || ((*it)->type() == Element::Element_Inductance)) {
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
                (*x)[i] = v;
                continue;
            }

            if (std::abs(v) > std::abs((*x)[i]))
                (*x)[i] = v;
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
            (*it)->setCBranches(i);
            i += 1;
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
}

void Circuit::initCache()
{
    m_elementSet->updateInfo();

    m_bCanCache = true;
    m_logicOutCount = 0;

    delete[] m_pLogicOut;
    m_pLogicOut = nullptr;

    delete m_pLogicCacheBase;
    m_pLogicCacheBase = nullptr;

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
        case Element::Element_VoltageSource: {
            break;
        }

        case Element::Element_LogicOut: {
            m_logicOutCount++;
            break;
        }

        case Element::Element_CurrentSignal:
        case Element::Element_VoltageSignal:
        case Element::Element_Capacitance:
        case Element::Element_Inductance: {
            m_bCanCache = false;
            break;
        }
        }
    }

    if (!m_bCanCache)
        return;

    m_pLogicOut = new LogicOut *[m_logicOutCount];
    unsigned i = 0;
    for (ElementList::iterator it = m_elementList.begin(); it != end && m_bCanCache; ++it) {
        if ((*it)->type() == Element::Element_LogicOut)
            m_pLogicOut[i++] = static_cast<LogicOut *>(*it);
    }

    m_pLogicCacheBase = new LogicCacheNode;
}

void Circuit::setCacheInvalidated()
{
    if (m_pLogicCacheBase) {
        delete m_pLogicCacheBase->high;
        m_pLogicCacheBase->high = nullptr;

        delete m_pLogicCacheBase->low;
        m_pLogicCacheBase->low = nullptr;

        delete m_pLogicCacheBase->data;
        m_pLogicCacheBase->data = nullptr;
    }
}

void Circuit::cacheAndUpdate()
{
    LogicCacheNode *node = m_pLogicCacheBase;
    for (unsigned i = 0; i < m_logicOutCount; i++) {
        if (m_pLogicOut[i]->outputState()) {
            if (!node->high)
                node->high = new LogicCacheNode;

            node = node->high;
        } else {
            if (!node->low)
                node->low = new LogicCacheNode;

            node = node->low;
        }
    }

    if (node->data) {
        (*m_elementSet->x()) = *node->data;
        m_elementSet->updateInfo();
        return;
    }

    if (m_elementSet->containsNonLinear())
        m_elementSet->doNonLinear(150, 1e-10, 1e-13);
    else
        m_elementSet->doLinear(true);

    if (node->data) {
        node->data = m_elementSet->x();
    } else {
        node->data = new QuickVector(m_elementSet->x());
    }

    //	node->data = new Vector( m_elementSet->x()->size() );
    //	*node->data = *m_elementSet->x();
}

void Circuit::createMatrixMap()
{
    m_elementSet->createMatrixMap();
}

bool Circuit::recursivePinAdd(Pin *node, PinList *unassignedNodes, PinList *associated, PinList *nodes)
{
    if (!unassignedNodes->contains(node))
        return false;
    unassignedNodes->removeAll(node);

    bool foundGround = node->eqId() == -1;

    const PinList circuitDependentPins = node->circuitDependentPins();
    const PinList::const_iterator dEnd = circuitDependentPins.end();
    for (PinList::const_iterator it = circuitDependentPins.begin(); it != dEnd; ++it) {
        if (!associated->contains(*it))
            associated->append(*it);
    }

    nodes->append(node);

    const PinList localConnectedPins = node->localConnectedPins();
    const PinList::const_iterator end = localConnectedPins.end();
    for (PinList::const_iterator it = localConnectedPins.begin(); it != end; ++it)
        foundGround |= recursivePinAdd(*it, unassignedNodes, associated, nodes);

    return foundGround;
}

void Circuit::doNonLogic()
{
    if (!m_elementSet || m_cnodeCount + m_branchCount <= 0)
        return;

    if (m_bCanCache) {
        if (!m_elementSet->b()->isChanged() && !m_elementSet->matrix()->isChanged())
            return;
        cacheAndUpdate();
        updateNodalVoltages();
        m_elementSet->b()->setUnchanged();
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

void Circuit::stepReactive()
{
    ElementList::iterator listEnd = m_elementList.end();
    for (ElementList::iterator it = m_elementList.begin(); it != listEnd; ++it) {
        Element *const e = *it;
        if (e && e->isReactive())
            (static_cast<Reactive *>(e))->time_step();
    }
}

void Circuit::updateNodalVoltages()
{
    CNode **_cnodes = m_elementSet->cnodes();

    const PinList::iterator endIt = m_pinList.end();
    for (PinList::iterator it = m_pinList.begin(); it != endIt; ++it) {
        Pin *const node = *it;
        int i = node->eqId();
        if (i == -1)
            node->setVoltage(0.);
        else {
            const double v = _cnodes[i]->v;
            node->setVoltage(std::isfinite(v) ? v : 0.);
        }
    }
}

void Circuit::updateCurrents()
{
    ElementList::iterator listEnd = m_elementList.end();
    for (ElementList::iterator it = m_elementList.begin(); it != listEnd; ++it) {
        (*it)->updateCurrents();
    }
}

void Circuit::displayEquations()
{
    m_elementSet->displayEquations();
}
// END class Circuit

// BEGIN class LogicCacheNode

LogicCacheNode::LogicCacheNode()
{
    low = nullptr;
    high = nullptr;
    data = nullptr;
}

LogicCacheNode::~LogicCacheNode()
{
    delete low;
    delete high;
    if (data)
        delete data;
}
// END class LogicCacheNode
