/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "element.h"
#include "elementset.h"

#include <cassert>
#include <cmath>
#include <kdebug.h>

Element::Element()
	:  p_eSet(0), b_status(false), m_temp(0)
{
	b_componentDeleted = false;

	for(int i=0; i<MAX_CNODES; i++) cnodes[i] = -1;

	resetCurrents();

	for(int i=0; i<MAX_CBRANCHES; i++) cbranches[i] = -1;

	m_numCBranches = 0;
	m_numCNodes = 0;
}

// may be used in subclasses,
Element::~Element() {}

void Element::resetCurrents()
{
	for( int i=0; i<MAX_CNODES; i++) m_cnodeI[i] = 0.0;
}

void Element::setElementSet(ElementSet *c)
{
	assert(!b_componentDeleted);
	assert(!p_eSet);

	if(!c) return elementSetDeleted();

	p_eSet = c;
 status_changed = true;
}

void Element::componentDeleted()
{
	if(b_componentDeleted) {
		// Something strange happened here....
// FIXME missing code. 
	}

	b_componentDeleted = true;
	b_status = false;
// 	kdDebug() << "Element::componentDeleted(): Setting b_status to false, this="<<this<<endl;

// we only point to these, right??????
	p_eSet = 0;
// ???????????????????????????????????

	setCNodes();
	setCBranches();
}

void Element::elementSetDeleted()
{
	if(b_componentDeleted) return delete this;

	b_status = false;
// 	kdDebug() << "Element::elementSetDeleted(): Setting b_status to false, this="<<this<<endl;
	
	p_eSet = 0;
	setCNodes();
	setCBranches();
}

void Element::setCNodes(const int n0, const int n1, const int n2, const int n3)
{
	// MAX_CNODES-1 should match the last array index below.

	cnodes[0] = n0;
	cnodes[1] = n1;
	cnodes[2] = n2;
	cnodes[3] = n3;

/*
	p_cnode[0] = (n0>-1) ? p_eSet->cnodes(n0) : (n0==-1?p_eSet->ground() : 0);
	p_cnode[1] = (n1>-1) ? p_eSet->cnodes(n1) : (n1==-1?p_eSet->ground() : 0);
	p_cnode[2] = (n2>-1) ? p_eSet->cnodes(n2) : (n2==-1?p_eSet->ground() : 0);
	p_cnode[3] = (n3>-1) ? p_eSet->cnodes(n3) : (n3==-1?p_eSet->ground() : 0);
*/
	status_changed = true;
}

void Element::setCBranches( const int b0, const int b1, const int b2, const int b3)
{
	cbranches[0] = b0;
	cbranches[1] = b1;
	cbranches[2] = b2;
	cbranches[3] = b3;

	status_changed = true;
}

bool Element::updateStatus()
{
	// First, set status to false if all nodes in use are ground
	b_status = false;

if(!m_numCNodes) {
// we do the double check so that we can usefully set a breakpoint.
Type foo = type();
kdError() << foo; 
assert(m_numCNodes);
}

	if(p_eSet) {
	for(unsigned int i=0; i < m_numCNodes; i++) {
		if(cnodes[i] == -2) return b_status = false;
		else b_status |= !(cnodes[i] == -1);
	}

	// Finally, set status to false if not all the required branches are set
	for(unsigned int i=0; i<m_numCBranches; i++) {
		if(cbranches[i] == -1) b_status = false;
	}
	}

	// check for various pointers

	if(!b_status) resetCurrents();

	// And return the status :-)
// 	kdDebug() << "Element::updateStatus(): Setting b_status to "<<(b_status?"true":"false")<<" this="<<this<<endl;
	status_changed = false;
	return b_status;
}

double Element::cbranchCurrent( const int branch)
{
	if( !getStatus() || branch<0 || branch>=m_numCBranches) return 0.;
	return p_eSet->cbranches(cbranches[branch]).i;
}

double Element::cnodeVoltage(const int node)
{
	if( !getStatus() || node<0 || node>=m_numCNodes) return 0.;
	return p_eSet->cnodes(cnodes[node]).v;
}

double Element::checkCurrents() const {
	double sum = 0;
	for(unsigned int i = 0; i < m_numCNodes ; i++) {
		sum += m_cnodeI[i];
	}

	return sum;
}

