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
#include "matrix.h"
#include "nonlinear.h"

#include <qdebug.h>

#include <cmath>
#include <iostream>
#include <cassert>

ElementSet::ElementSet( Circuit * circuit, const int n, const int m )
	:  m_cb(m), m_cn(n), m_pCircuit(circuit)
{

	int tmp = m_cn + m_cb;

	p_logicIn = 0;

	if( tmp) {
		p_A = new Matrix( m_cn, m_cb );
		p_b = new QuickVector(tmp);
		p_x = new QuickVector(tmp);
	} else {
		p_A = 0;
		p_x = p_b = 0;
	}

	m_cnodes = new CNode*[m_cn];
	for ( uint i=0; i<m_cn; i++ ) {
		m_cnodes[i] = new CNode(i);
	}

	m_cbranches = new CBranch*[m_cb];
	for ( uint i=0; i<m_cb; i++ ) {
		m_cbranches[i] = new CBranch(i);
	}

	m_ground = new CNode();
	m_ground->isGround = true;
	b_containsNonLinear = false;
}

ElementSet::~ElementSet()
{
	const ElementList::iterator end = m_elementList.end();
	for ( ElementList::iterator it = m_elementList.begin(); it != end; ++it ) {
		// Note: By calling setElementSet(0l), we might have deleted it (the Element will commit
		// suicide when both the ElementSet and Component to which it belongs have deleted
		// themselves). So be very careful it you plan to do anything with the (*it) pointer
		if (*it) (*it)->elementSetDeleted();
	}

	for ( uint i=0; i<m_cn; i++ ) delete m_cnodes[i];
	for ( uint i=0; i<m_cb; i++ ) delete m_cbranches[i];

	delete[] m_cbranches;
	delete[] m_cnodes;
	delete[] p_logicIn;
	delete m_ground;
	if(p_A) delete p_A;
	if(p_b) delete p_b;
	if(p_x) delete p_x;
}


void ElementSet::setCacheInvalidated()
{
	m_pCircuit->setCacheInvalidated();
}


void ElementSet::addElement( Element *e )
{
	if ( !e || m_elementList.contains(e) ) return;
	e->setElementSet(this);
	m_elementList.append(e);
	if ( e->isNonLinear() )
	{
		b_containsNonLinear = true;
		m_cnonLinearList.append( static_cast<NonLinear*>(e) );
	}
}


void ElementSet::createMatrixMap()
{
// mapping nolonger done, overly ambitious optimization... 

	// And do our logic as well...
	
	m_clogic = 0;
	ElementList::iterator end = m_elementList.end();
	for ( ElementList::iterator it = m_elementList.begin(); it != end; ++it )
	{
		if ( dynamic_cast<LogicIn*>(*it) )
			m_clogic++;
	}
	
	p_logicIn = new LogicIn*[m_clogic];
	int i=0;
	for ( ElementList::iterator it = m_elementList.begin(); it != end; ++it )
	{
		if ( LogicIn * in = dynamic_cast<LogicIn*>(*it) )
			p_logicIn[i++] = in;
	}
}


void ElementSet::doNonLinear( int maxIterations, double maxErrorV, double maxErrorI )
{
	QuickVector *p_x_prev = new QuickVector(p_x);

	// And now tell the cnodes and cbranches about their new voltages & currents
	updateInfo();
	
	const NonLinearList::iterator end = m_cnonLinearList.end();
	
	int k = 0;
	do {
		// Tell the nonlinear elements to update its J, A and b from the newly calculated x
		for ( NonLinearList::iterator it = m_cnonLinearList.begin(); it != end; ++it )
			(*it)->update_dc();

		*p_x = *p_b;  // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		p_A->performLU();
		p_A->fbSub(p_x);
		updateInfo();
		
		// Now, check for convergence
		bool converged = true;
		for ( unsigned i = 0; i < m_cn; ++i )
		{
			double diff = std::abs( (*p_x_prev)[i] - (*p_x)[i] );
			if ( diff > maxErrorI )
			{
				converged = false;
				break;
			}
		}
		if ( converged ) {
			for ( unsigned i = m_cn; i < m_cn+m_cb; ++i )
			{
				double diff = std::abs( (*p_x_prev)[i] - (*p_x)[i] );
				if ( diff > maxErrorV )
				{
					converged = false;
					break;
				}
			}
		}

		*p_x_prev = *p_x; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		if ( converged ) break;
	}
	while ( ++k < maxIterations );

    delete p_x_prev;
}


bool ElementSet::doLinear( bool performLU )
{
	if ( b_containsNonLinear || (!p_b->isChanged() && ((performLU && !p_A->isChanged()) || !performLU)) )
		return false;
	
	if (performLU)
		p_A->performLU();

	*p_x = *p_b;   // <<< why does this code work, when I try it, I always get the default shallow copy.


	p_A->fbSub(p_x);
	updateInfo();
	p_b->setUnchanged();
	
	return true;
}

void ElementSet::updateInfo()
{
	for ( uint i=0; i<m_cn; i++ )
	{
		const double v = (*p_x)[i];
		if (std::isfinite(v)) {
			m_cnodes[i]->v = v;
		} else {
			(*p_x)[i] = 0.;
			m_cnodes[i]->v = 0.;
		}
	}

	for ( uint i=0; i<m_cb; i++ )
	{
		// NOTE: I've used lowercase and uppercase "I" here, so be careful!
		const double I = (*p_x)[i+m_cn];
		if (std::isfinite(I)) {
			m_cbranches[i]->i = I;
		} else {
			(*p_x)[i+m_cn] = 0.;
			m_cbranches[i]->i = 0.;
		}
	}

	// Tell logic to check themselves
	for ( uint i=0; i<m_clogic; ++i )
	{
		p_logicIn[i]->check();
	}
}

void ElementSet::displayEquations()
{
	std::cout.setf(std::ios_base::fixed);
	std::cout.precision(5);
	std::cout.setf(std::ios_base::showpoint);
	std::cout << "A x = b :"<<std::endl;
	for ( uint i=0; i<m_cn+m_cb; i++ )
	{
		std::cout << "( ";
		for ( uint j=0; j<m_cn+m_cb; j++ )
		{
			const double value = p_A->g(i,j);
// 			if	( value > 0 ) cout <<"+";
// 			else if ( value == 0 ) cout <<" ";
			std::cout.width(10);
			std::cout << value<<" ";
		}
		std::cout << ") ( "<<(*p_x)[i]<<" ) = ( ";
		std::cout<<(*p_b)[i]<<" )"<<std::endl;
	}
	std::cout << "A_LU:"<<std::endl;
	p_A->displayLU();
}

