/***************************************************************************
 *   Copyright (C) 2003-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <vector> 
#include <set>
#include <cassert>

#include "qstringlist.h"
#include "elementset.h"

class Component;
class Pin;
class Element;
class ElementMap;
class ElementSet;
class LogicOut;

typedef std::set<Pin *> PinSet;
typedef std::set<Element*> ElementList;
typedef std::vector<LogicOut *> logicOutVec;

class LogicCacheNode
{
public:
	LogicCacheNode();
	~LogicCacheNode();
	LogicCacheNode *addOrGetHigh();
	LogicCacheNode *addOrGetLow();
	QuickVector *getData() const;
	void setData(const ElementSet *newData);

private: 
	LogicCacheNode *high;
	LogicCacheNode *low;
	QuickVector *data;
};

/**
Usage of this class (usually invoked from CircuitDocument):
(1) Add Wires, Pins and Elements to the class as appropriate
(2) Call init to initialize the simulation
(3) Control the simulation with step()

This class can be considered a bridge between the gui-tainted CircuitDocument - specific
to this implementation, and the pure untainted ElementSet. Please keep it that way.

@short Simulates a collection of components
@author David Saxton
*/
class Circuit
{
public:
	Circuit();
	~Circuit();

    /**
     Add a pin to the circuit.
     */
	void addPin(Pin *node);
	PinSet *getPins() { return &m_pinList; }

    /**
     * Add an element to the circuit.
     * This method is called only internally, so it should be protected
    */
    void addElement(Element *element);

	bool containsNonLinear() const { return m_elementSet->containsNonLinear(); }

	void init();
	/**
	* Called after everything else has been setup - before doNonLogic or
	* doLogic are called for the first time. Preps the circuit.
	*/
	void initCache();
	/**
	* Marks all cached results as invalidated and removes them.
	*/
	void setCacheInvalidated();

	/**
	* Solves for non-logic elements
	*/
	void doNonLogic();

	/**
	* Solves for logic elements (i.e just does fbSub)
	*/
	void doLogic() { m_elementSet->doLinear(false); }

	void displayEquations();
	void updateCurrents();

	/**
	* This will identify the ground node and non-ground nodes in the given set.
	* Ground will be given the eqId -1, non-ground of 0.
	* @param highest The highest ground type of the groundnodes found. If no
	ground nodes were found, this will be (gt_never-1).
	* @returns the number of ground nodes. If all nodes are at or below the
	* 		gt_never threshold, then this will be zero.
	*/
	static int identifyGround(PinSet nodeList, int *highest = 0);

	void setChanged() { isSetChanged = true; }
	void clearChanged() { isSetChanged = false; }
	inline bool isChanged() { return isSetChanged; }

    /**
     * Add an ElementMap to the list of ElementMaps in the circuit.
     * Element maps are needed for current calculation.
     * This method also tries to add the Element and Pins associated with the element map.
     * Element's won't be duplicated in the circuit.
     * @param em the ElementMap to be added
     */
    void addElementMap(ElementMap *em);

    /**
     * Remove an ElementMap from the list of ElementMaps in the circuit.
     * Element maps are needed for current calculation.
     * The Element and Pins associated with the ElementMap are also removed.
     * @param em the ElementMap to be removed
     */
    void removeElementMap(ElementMap *em);

    /**
     * Add a component to the circuit
     * @param comp the component to be added
     */
	void addComponent(const Component& comp);

protected:
	void cacheAndUpdate();
	/**
	* Update the nodal voltages from those calculated in ElementSet
	*/
	void updateNodalVoltages();
	/**
	* Step the reactive elements.
	*/
	void stepReactive();
	/**
	* Returns true if any of the nodes are ground
	*/
	static bool recursivePinAdd(Pin *node, PinSet *unassignedNodes, PinSet *associated, PinSet *nodes);

	PinSet m_pinList;
	ElementList m_elementList;
	ElementSet *m_elementSet;

	//Stuff for caching
	bool m_bCanCache;
	LogicCacheNode *m_pLogicCacheBase;

	logicOutVec m_pLogicOut;

    std::set<ElementMap*> m_elementMapSet;

private: 
	bool isSetChanged;
};

#endif
