/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "bjt.h"
#include "circuit.h"
#include "elementset.h"
#include "element.h"
#include "logic.h"
#include "nonlinear.h"

#include <qdebug.h>

#include <cmath>
#include <iostream>
#include <cassert>

ElementSet::ElementSet(Circuit *circuit, const int n, const int m)
		:  m_cb(m), m_cn(n), m_pCircuit(circuit) {
	int tmp = m_cn + m_cb;

	if (tmp) {
		p_A = new Matrix(tmp);
		p_b = new QuickVector(tmp);
		p_x = new QuickVector(tmp);
	} else {
		p_A = 0;
		p_x = p_b = 0;
	}

	m_cnodes = new CNode*[m_cn];
	for (uint i = 0; i < m_cn; i++) {
		m_cnodes[i] = new CNode(i);
	}

	m_cbranches = new CBranch*[m_cb];
	for (uint i = 0; i < m_cb; i++) {
		m_cbranches[i] = new CBranch(i + m_cn);
	}

	m_ground = new CNode();
	m_ground->setGround();
}

ElementSet::~ElementSet() {
	const ElementList::iterator end = m_elementList.end();
	for (ElementList::iterator it = m_elementList.begin(); it != end; ++it) {
		// Note: By calling setElementSet(0l), we might have deleted it (the Element will commit
		// suicide when both the the ElementSet and Component to which it belongs have deleted
		// themselves). So be very careful it you plan to do anything with the (*it) pointer
		if (*it)(*it)->elementSetDeleted();
	}

	for (uint i = 0; i < m_cn; i++) delete m_cnodes[i];
	for (uint i = 0; i < m_cb; i++) delete m_cbranches[i];

	delete[] m_cbranches;
	delete[] m_cnodes;
	delete m_ground;

	if (p_A) delete p_A;
	p_A = 0; 
	if (p_b) delete p_b;
	p_b = 0;
	if (p_x) delete p_x;
	p_x = 0;
}

void ElementSet::setCacheInvalidated() {
	m_pCircuit->setCacheInvalidated();
}

void ElementSet::addElement(Element *e) {
	assert(e);

	e->setElementSet(this);
	m_elementList.insert(e);

	if (e->isNonLinear()) {
		m_cnonLinearList.insert(static_cast<NonLinear*>(e));
	}

	if (LogicIn *in = dynamic_cast<LogicIn*>(e))
		p_logicIn.insert(in);
}

void ElementSet::doNonLinear(const int maxIterations, const double maxErrorV, const double maxErrorI) {
    if(m_cn + m_cb <= 0)
        return;

	QuickVector *p_x_prev = new QuickVector(p_x);

	// And now tell the cnodes and cbranches about their new voltages & currents
	updateInfo();

	const NonLinearList::iterator end = m_cnonLinearList.end();

	int k = 0;
	do {
		// Tell the nonlinear elements to update its J, A and b from the newly calculated x
		for (NonLinearList::iterator it = m_cnonLinearList.begin(); it != end; ++it)
			(*it)->update_dc();

		*p_x = *p_b;  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifdef BITCHY
if(!p_A->validate()) {
std::cout << "ERROR: invalid matrix!!!" << std::endl;
break;
}
#endif

// we burn most of our CPU cycles when we make these calls.
		p_A->performLU();

#ifdef BITCHY
// if this test fails, the calculation engine under the simulator is broken =( -- and it is.
assert(p_A->validateLU() < 1e-4); 
#endif
		p_A->fbSub(p_x);
// #########################

		updateInfo();

		// Now, check for convergence
		bool converged = true;

		for (unsigned i = 0; i < m_cn; ++i) {
			double diff = std::abs((*p_x_prev)[i] - (*p_x)[i]);

			if (diff > maxErrorI) {
				converged = false;
				break;
			}
		}

		if (converged) {
			for (unsigned i = m_cn; i < m_cn + m_cb; ++i) {
				double diff = std::abs((*p_x_prev)[i] - (*p_x)[i]);

				if (diff > maxErrorV) {
					converged = false;
					break;
				}
			}
		}

		*p_x_prev = *p_x; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		if (converged) break;
	} while (++k < maxIterations);

	delete p_x_prev;
}

void ElementSet::loadX(const QuickVector *other) {
	assert(other->size() == p_x->size());
	
	*p_x = *other;
}

bool ElementSet::doLinear(bool performLU) {
    if(m_cn + m_cb <= 0)
        return false;

	if(!m_cnonLinearList.empty() || (!p_b->isChanged() && !p_A->validate()))
		return false;

	if (performLU) {
		p_A->performLU();
// if this test fails, the calculation engine under the simulator is broken =( -- and it is.
#ifdef BITCHY
assert(p_A->validateLU() < 1e-4); 
#endif
	}

	*p_x = *p_b;   // <<< why does this code work, when I try it, I always get the default shallow copy.

	p_A->fbSub(p_x);

	updateInfo();
	p_b->setUnchanged();

	return true;
}

void ElementSet::updateInfo() {
    if(m_cn + m_cb <= 0)
        return;

//update voltage
	for (uint i = 0; i < m_cn; i++) {
		const double v = (*p_x)[i];

		if (std::isfinite(v)) {
			m_cnodes[i]->setVoltage(v);
		} else {
			(*p_x)[i] = 0.;
			m_cnodes[i]->setVoltage(0);
		}
	}

// update current.
	for (uint i = 0; i < m_cb; i++) {
		// NOTE: I've used lowercase and uppercase "I" here, so be careful!
		const double I = (*p_x)[i + m_cn];

		if (std::isfinite(I)) {
			m_cbranches[i]->setCurrent(I);
		} else {
// TODO: More advanced error checking, I think this is where our circuits stall out.
			(*p_x)[i + m_cn] = 0.;
			m_cbranches[i]->setCurrent(0);
		}
	}

	// Tell logic to check themselves
	{
	LogicInSet::iterator lend = p_logicIn.end();
	for (LogicInSet::iterator i = p_logicIn.begin(); i != lend; ++i) {
		(*i)->check();
	}
	}
}

void ElementSet::displayEquations() {
    if(m_cn + m_cb <= 0){
        std::cout << "ElementSet::displayEquations: all vectors and matrices are null.\n";
        return;
    }

    std::cout.setf(std::ios_base::fixed);
	std::cout.precision(5);
	std::cout.setf(std::ios_base::showpoint);
	std::cout << "A x = b :" << std::endl;

	for (uint i = 0; i < m_cn + m_cb; i++) {
		std::cout << "( ";

		for (uint j = 0; j < m_cn + m_cb; j++) {
			const double value = p_A->g(i, j);
// 			if	( value > 0 ) cout <<"+";
// 			else if ( value == 0 ) cout <<" ";
			std::cout.width(10);
			std::cout << value << " ";
		}

		std::cout << ") ( " << (*p_x)[i] << " ) = ( ";
		std::cout << (*p_b)[i] << " )" << std::endl;
	}

	std::cout << "A_LU:" << std::endl;
	p_A->displayLU(std::cout);
}

