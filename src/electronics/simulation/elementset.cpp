/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "elementset.h"
#include "bjt.h"
#include "circuit.h"
#include "element.h"
#include "logic.h"
#include "matrix.h"
#include "nonlinear.h"

#include <QDebug>

#include <cassert>
#include <cmath>
#include <iostream>

ElementSet::ElementSet(Circuit *circuit, const uint n, const uint m)
    : m_cbranches(m)
    , m_cnodes(n)
    , m_pCircuit(circuit)
{
    uint tmp = n + m;
    Q_ASSERT(tmp);
    p_A = new Matrix(n, m);
    p_b = new QuickVector(tmp);
    p_x = new QuickVector(tmp);

    for (uint i = 0; i < n; i++) {
        m_cnodes[i].num = i;
    }

    for (uint i = 0; i < m; i++) {
        m_cbranches[i].num = i;
    }

    m_ground.num = CNode::GROUND;
}

ElementSet::~ElementSet()
{
    const ElementList::iterator end = m_elementList.end();
    for (ElementList::iterator it = m_elementList.begin(); it != end; ++it) {
        // Note: By calling setElementSet(nullptr), we might have deleted it (the Element will commit
        // suicide when both the ElementSet and Component to which it belongs have deleted
        // themselves). So be very careful it you plan to do anything with the (*it) pointer
        if (*it)
            (*it)->elementSetDeleted();
    }

    delete p_A;
    delete p_b;
    delete p_x;
}

void ElementSet::setCacheInvalidated()
{
    m_pCircuit->setCacheInvalidated();
}

void ElementSet::addElement(Element *e)
{
    if (!e || m_elementList.contains(e))
        return;
    e->setElementSet(this);
    m_elementList.append(e);
    if (e->isNonLinear()) {
        m_cnonLinearList.append(static_cast<NonLinear *>(e));
    }
}

void ElementSet::removeElement(Element *e)
{
    m_elementList.removeAll(e);
    if (e->isNonLinear()) {
        m_cnonLinearList.removeAll(static_cast<NonLinear *>(e));
        b_containsNonLinear = m_cnonLinearList.size() > 0;
    }
    if (LogicIn* it = dynamic_cast<LogicIn *>(e)) {
        // TODO: Cleanup if LogicIn
        // m_logicIn.removeAll(it);
    }
}

void ElementSet::createMatrixMap()
{
    // mapping nolonger done, overly ambitious optimization...

    // And do our logic as well...

    uint m_clogic = 0;
    ElementList::iterator end = m_elementList.end();
    for (ElementList::iterator it = m_elementList.begin(); it != end; ++it) {
        if (dynamic_cast<LogicIn *>(*it))
            m_clogic++;
    }

    m_logicIn.resize(m_clogic);
    int i = 0;
    for (ElementList::iterator it = m_elementList.begin(); it != end; ++it) {
        if (LogicIn *in = dynamic_cast<LogicIn *>(*it))
            m_logicIn[i++] = in;
    }
}

void ElementSet::doNonLinear(int maxIterations, double maxErrorV, double maxErrorI)
{
    const uint m_cn = m_cnodes.size();
    const uint m_cb = m_cbranches.size();
    QuickVector *p_x_prev = new QuickVector(p_x);

    // And now tell the cnodes and cbranches about their new voltages & currents
    updateInfo();

    const NonLinearList::iterator end = m_cnonLinearList.end();

    int k = 0;
    do {
        // Tell the nonlinear elements to update its J, A and b from the newly calculated x
        for (NonLinearList::iterator it = m_cnonLinearList.begin(); it != end; ++it)
            (*it)->update_dc();

        *p_x = *p_b; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        p_A->performLU();
        p_A->fbSub(p_x);
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

        if (converged)
            break;
    } while (++k < maxIterations);

    delete p_x_prev;
}

bool ElementSet::doLinear(bool performLU)
{
    if (containsNonLinear() || (!p_b->isChanged() && ((performLU && !p_A->isChanged()) || !performLU)))
        return false;

    if (performLU)
        p_A->performLU();

    *p_x = *p_b; // <<< why does this code work, when I try it, I always get the default shallow copy.

    p_A->fbSub(p_x);
    updateInfo();
    p_b->setUnchanged();

    return true;
}

void ElementSet::updateInfo()
{
    const uint m_cn = m_cnodes.size();
    const uint m_cb = m_cbranches.size();
    for (uint i = 0; i < m_cn; i++) {
        const double v = (*p_x)[i];
        if (std::isfinite(v)) {
            m_cnodes[i].v = v;
        } else {
            (*p_x)[i] = 0.;
            m_cnodes[i].v = 0.;
        }
    }

    for (uint i = 0; i < m_cb; i++) {
        // NOTE: I've used lowercase and uppercase "I" here, so be careful!
        const double I = (*p_x)[i + m_cn];
        if (std::isfinite(I)) {
            m_cbranches[i].i = I;
        } else {
            (*p_x)[i + m_cn] = 0.;
            m_cbranches[i].i = 0.;
        }
    }

    // Tell logic to check themselves
    for (LogicIn *e: std::as_const(m_logicIn)) {
        e->check();
    }
}

void ElementSet::displayEquations()
{
    const uint m_cn = m_cnodes.size();
    const uint m_cb = m_cbranches.size();
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
    p_A->displayLU();
}
