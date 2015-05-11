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

#include <Qt/qpointer.h>
#include "Qt/qstringlist.h"
#include "Qt/qlist.h"

#include "elementset.h"

class CircuitDocument;
class Wire;
class Pin;
class Element;
class LogicOut;

typedef QList<QPointer<Pin> > PinList;
typedef QList<Element*> ElementList;


class LogicCacheNode
{
public:
	LogicCacheNode();
	~LogicCacheNode();

	LogicCacheNode * high;
	LogicCacheNode * low;
	QuickVector * data;
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

	void addPin( Pin *node );
	void addElement( Element *element );

	bool contains( Pin *node );
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

	void createMatrixMap();
	/**
		* This will identify the ground node and non-ground nodes in the given set.
		* Ground will be given the eqId -1, non-ground of 0.
		* @param highest The highest ground type of the groundnodes found. If no
		ground nodes were found, this will be (gt_never-1).
		* @returns the number of ground nodes. If all nodes are at or below the
		* 			gt_never threshold, then this will be zero.
		*/
	static int identifyGround( PinList nodeList, int *highest = 0l );

	void setNextChanged( Circuit * circuit, unsigned char chain ) { m_pNextChanged[chain] = circuit; }
	Circuit * nextChanged( unsigned char chain ) const { return m_pNextChanged[chain]; }
	void setCanAddChanged( bool canAdd ) { m_bCanAddChanged = canAdd; }
	bool canAddChanged() const { return m_bCanAddChanged; }

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
	static bool recursivePinAdd( Pin *node, PinList *unassignedNodes, PinList *associated, PinList *nodes );

	int m_cnodeCount;
	int m_branchCount;
	int m_prepNLCount; // Count until next m_elementSet->prepareNonLinear() is called

	PinList m_pinList;
	ElementList m_elementList;
	ElementSet *m_elementSet;

	//Stuff for caching
	bool m_bCanCache;
	LogicCacheNode * m_pLogicCacheBase;
	unsigned m_logicOutCount;
	LogicOut ** m_pLogicOut;

	bool m_bCanAddChanged;
	Circuit * m_pNextChanged[2];
};

#endif
