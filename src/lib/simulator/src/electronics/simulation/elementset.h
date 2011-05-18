/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ELEMENTSET_H
#define ELEMENTSET_H

#include <set>
#include <cassert>
#include "math/matrix.h"

class CBranch;
class Circuit;
class CNode;
class Element;
class ElementSet;
class LogicIn;
class NonLinear;

typedef std::set<Element*> ElementList;
typedef std::set<NonLinear*> NonLinearList;
typedef std::set<LogicIn*> LogicInSet;

/**
Steps in simulation of a set of elements:
(1) Create this class with given number of nodes "n" and voltage sources "m"
(2) Add the various elements with addElement.
(3) Call performDC()
(4) Get the nodal voltages and voltage currents with x()
(5) Repeat steps 3 and 4 if necessary for further transient analysis.

This class shouldn't be confused with the Circuit class, but considered a helper class to Circuit.
Circuit will handle the simulation of a set of components over time. This just finds the DC-operating
point of the circuit for a given set of elements.

@short Handles a set of circuit elements
@author David Saxton
*/

class ElementSet {

public:
	/**
	 * Create a new circuit, with "n" nodes and "m" voltage sources.
	 * After creating the circuit, you must call setGround to specify
	 * the ground nodes, before adding any elements.
	 */
	ElementSet(Circuit *circuit, const int n, const int m);
	/**
	 * Destructor. Note that only the matrix and supporting data is deleted.
	 * i.e. Any elements added to the circuit will not be deleted.
	 */
	~ElementSet();
	Circuit *circuit() const { return m_pCircuit; }

	void addElement(Element *e);
	void setCacheInvalidated();

	inline bool AChanged() const {    return p_A->isChanged(); }
	inline double &Ag(CUI i, CUI j) { return p_A->g(i, j);     }

	inline bool bChanged() const { return p_b->isChanged();    }
	inline void bUnchanged() const {      p_b->setUnchanged(); }
	inline double &bValue(unsigned int index) { return (*p_b)[index]; }

	inline void setXLoc(CUI loc, double val) { (*p_x)[loc] = val; }
	inline double &xValue(unsigned int index) { return (*p_x)[index]; }
	void loadX(const QuickVector *other);
	/** special function to cache our x vector, use only for that! */
	inline QuickVector *xForCache() const { return p_x; }

	/**
	 * @return if we have any nonlinear elements (e.g. diodes, tranaistors).
	 */
	bool containsNonLinear() const { return !m_cnonLinearList.empty(); }

	/**
	 * Solves for nonlinear elements, or just does linear if it doesn't contain
	 * any nonlinear.
	 */
	void doNonLinear(const int maxIterations, const double maxErrorV = 1e-9, const double maxErrorI = 1e-12);
	/**
	 * Solves for linear and logic elements.
	 * @returns true if anything changed
	 */
	bool doLinear(bool performLU);

	inline CBranch *cBranch(unsigned int i) { assert(i < m_cb); return m_cbranches[i]; }
	inline CNode *cNode(int i) {
		if(i == -1) return m_ground;
		return m_cnodes[i]; }

	inline unsigned int vectorsize() const { return m_cn + m_cb; }

	/**
	 * Displays the matrix equations Ax=b and J(dx)=-r
	 */
	void displayEquations();

	/**
	 * Update the nodal voltages and branch currents from the x vector
	 */
	void updateInfo();

private:
// calc engine stuff
	Matrix *p_A;
	QuickVector *p_x;
	QuickVector *p_b;
// end calc engine stuff.

	ElementList m_elementList;
	NonLinearList m_cnonLinearList;

	unsigned int m_cb;
	CBranch **m_cbranches; // Pointer to an array of cbranches

	unsigned int m_cn;
	CNode **m_cnodes; // Pointer to an array of cnodes
	CNode *m_ground;

	LogicInSet p_logicIn;
	Circuit *m_pCircuit;
};
#endif

