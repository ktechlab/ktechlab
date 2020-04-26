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

#include <QDebug>

//BEGIN class Element
Element::Element()
{
	b_status = false;
	p_eSet = nullptr;
	b_componentDeleted = false;

	for ( int i = 0; i < MAX_CNODES; i++ )
		p_cnode[i] = nullptr;
	
	resetCurrents();
	
	for ( int i = 0; i < MAX_CBRANCHES; i++ )
		p_cbranch[i] = nullptr;
	
	m_numCBranches = 0;
	m_numCNodes = 0;
}

Element::~ Element()
{
}

void Element::resetCurrents()
{
	for ( int i=0; i<8; i++ )
		m_cnodeI[i] = 0.0;
}

void Element::setElementSet( ElementSet *c )
{
	assert(!b_componentDeleted);
	assert(!p_eSet);
	if (!c) return elementSetDeleted();
	p_eSet = c;
	updateStatus();
}

void Element::componentDeleted()
{
	b_componentDeleted = true;
	b_status = false;

	p_eSet = nullptr;
	setCNodes();
	setCBranches();
}

void Element::elementSetDeleted()
{
	if (b_componentDeleted) return delete this;

	b_status = false;
// 	qDebug() << "Element::elementSetDeleted(): Setting b_status to false, this="<<this<<endl;
	
	p_eSet = nullptr;
	setCNodes();
	setCBranches();
}


void Element::setCNodes( const int n0, const int n1, const int n2, const int n3 )
{
	if ( !p_eSet )
	{
// 		cerr << "Element::setCNodes: can't set nodes without circuit!"<<endl;
		for ( int i=0; i<MAX_CNODES; i++ )
			p_cnode[i] = nullptr;
		return;
	}

	p_cnode[0] = (n0>-1)?p_eSet->cnodes()[n0]:(n0==-1?p_eSet->ground():nullptr);
	p_cnode[1] = (n1>-1)?p_eSet->cnodes()[n1]:(n1==-1?p_eSet->ground():nullptr);
	p_cnode[2] = (n2>-1)?p_eSet->cnodes()[n2]:(n2==-1?p_eSet->ground():nullptr);
	p_cnode[3] = (n3>-1)?p_eSet->cnodes()[n3]:(n3==-1?p_eSet->ground():nullptr);
	updateStatus();
}

void Element::setCBranches( const int b0, const int b1, const int b2, const int b3 )
{
	if ( !p_eSet )
	{
// 		cerr << "Element::setCBranches: can't set branches without circuit!"<<endl;
		for ( int i=0; i<4; i++ ) p_cbranch[i] = nullptr;
		return;
	}
	p_cbranch[0] = (b0>-1)?p_eSet->cbranches()[b0]:nullptr;
	p_cbranch[1] = (b1>-1)?p_eSet->cbranches()[b1]:nullptr;
	p_cbranch[2] = (b2>-1)?p_eSet->cbranches()[b2]:nullptr;
	p_cbranch[3] = (b3>-1)?p_eSet->cbranches()[b3]:nullptr;
	updateStatus();
}

bool Element::updateStatus()
{
	// First, set status to false if all nodes in use are ground
	b_status = false;
	for ( int i=0; i<m_numCNodes; i++ )
	{
		b_status |= p_cnode[i]?!p_cnode[i]->isGround:false;
	}
	
	// Set status to false if any of the nodes are not set
	for ( int i=0; i<m_numCNodes; i++ )
	{
		if (!p_cnode[i]) b_status = false;
	}
	
	// Finally, set status to false if not all the required branches are set
	for ( int i=0; i<m_numCBranches; i++ )
	{
		if (!p_cbranch[i]) b_status = false;
	}
	
	// Finally, check for various pointers
	if (!p_eSet) b_status = false;
	
	if (!b_status) {
		resetCurrents();
	}

	// And return the status :-)
// 	qDebug() << "Element::updateStatus(): Setting b_status to "<<(b_status?"true":"false")<<" this="<<this<<endl;
	return b_status;
}

double Element::cbranchCurrent( const int branch )
{
	if ( !b_status || branch<0 || branch>=m_numCBranches ) return 0.;
	return (*p_cbranch)[branch].i;
}

double Element::cnodeVoltage( const int node )
{
	if ( !b_status || node<0 || node>=m_numCNodes ) return 0.;
	return (*p_cnode)[node].v;
}

//END class Element



