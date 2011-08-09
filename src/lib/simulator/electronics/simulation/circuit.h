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

#include "simulatorexport.h"
#include "qstringlist.h"
#include "elementset.h"

class Component;
class Pin;
class Element;
class ElementMap;
class ElementSet;
class LogicOut;
class QuickVector;

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
class SIMULATOR_EXPORT Circuit
{
public:
    /**
     Create an empty circuit
     */
	Circuit();
	~Circuit();

    /**
     Add a pin to the circuit.
     */
	void addPin(Pin *node);

    /**
     Remove a pin from thge circuit
     \return 0 on success, negative on error
     */
    int removePin(Pin *pin);

    /**
     \return the set of pins in the current circuit
     */
	PinSet *getPins() { return &m_pinList; }

    /**
     \return true, if the circuit contains at least one nonlinear element;
        false otherwise
     */
	bool containsNonLinear() const { return m_elementSet->containsNonLinear(); }

    /**
     Assigns equations and initializes ElmentSet-s.
     Call this method before simulating and after altering the structure of
     the Circuit
     */
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

    /**
     Displays the equations of the current simulation on standard output
     */
	void displayEquations();

    /**
     After solving the circuit equations in doLogic() or doNonLogic() methods,
     this method can calculate all the currents flowing in the circuit
     */
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

    /**
     Method used by the Simulator class to check if the circuit has to
     be solved or not.
     Sets the changed flag on the circuit.
     */
	void setChanged() { m_isSetChanged = true; }

	/**
     Method to clear the changed flag on the circuit. Used by the simulator
     */
	void clearChanged() { m_isSetChanged = false; }

	/**
     \return the changed flag of the circuit. Used by the simulator.
     */
	inline bool isChanged() { return m_isSetChanged; }

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
	void addComponent(Component *comp);

    /**
     Remove a component from the circuit
     \param comp the component to be removed
    */
    void removeComponent(Component *comp);

    /**
     \return the list of components in the circuit
    */
    const QList<Component*> components() const;

    /**
     \return the number of equations in the circuit
     */
    int equationCount() const;

protected:

    /**
     * Add an element to the circuit.
     * This method is called only internally, so it should be protected
     */
    void addElement(Element *element);

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

    /**
     * \return a list of pin sets (lists), that are connected between them
     */
    QList<QList<Pin *> > getConnectedPinGroups();

	PinSet m_pinList;
	ElementList m_elementList;
	ElementSet *m_elementSet;

	//Stuff for caching
	bool m_bCanCache;
	LogicCacheNode *m_pLogicCacheBase;

	logicOutVec m_pLogicOut;

    std::set<ElementMap*> m_elementMapSet;

    /// components in this circuit. the circuit doesn't own the components
    QList<Component*> m_components;

private:
	bool m_isSetChanged;
};

#endif
