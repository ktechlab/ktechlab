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

#include "matrix.h"
#include "simulatorexport.h"

class CBranch;
class Circuit;
class CNode;
class Element;
class ElementSet;
class LogicIn;
class NonLinear;
class QuickVector;
class Matrix;

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

class SIMULATOR_EXPORT ElementSet {

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

	bool AChanged() const;
	double &Ag(CUI i, CUI j);

	bool bChanged() const;
	void bUnchanged() const;
	double &bValue(unsigned int index);

	void setXLoc(CUI loc, double val);
	double &xValue(unsigned int index);
	void loadX(const QuickVector *other);
	/** special function to cache our x vector, use only for that! */
	QuickVector *xForCache() const ;

	/**
	 * @return if we have any nonlinear elements (e.g. diodes, tranaistors).
	 */
	bool containsNonLinear() const;

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

	CBranch *cBranch(unsigned int i);
	CNode *cNode(int i);

	unsigned int vectorsize() const;

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

