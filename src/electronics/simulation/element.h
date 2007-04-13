/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ELEMENT_H
#define ELEMENT_H

#include "elementset.h"
#include "matrix.h"

using namespace std;
class ElementSet;

/* FIXME Too many responsibilities.

Seems to cover many different things such as physics, wires, the base class for
parts, simulation-related things... Either a necessary kludge or a prime target for refactoring. 

*/

const double T = 300.; // Temperature in Kelvin
const double K = 1.3806503e-23; // Boltzmann's constant
const double q = 1.602176462e-19; // Charge on an electron
const double V_T = K*T/q; // Thermal voltage
const double gmin = 1e-12; // Minimum parallel conductance used in dc domain

class CNode
{
public:
	CNode() : v(0.0), isGround(false), m_n(0) {}
	CNode(const uint32_t n) : v(0.0), isGround(false), m_n(n) {}
	void set_n( const uint32_t n ) { m_n=n; }
	uint32_t n() { return m_n; }
	double v; // Voltage on node. This is set from the last calculated voltage.
	bool isGround; // True for ground nodes. Obviously, you should ignore n and v if this is true
private:
	uint32_t m_n; // CNode number
};

class CBranch
{
public:
	CBranch() : i(0.0), m_n(0) {}
	CBranch(const uint32_t n) : i(0.0), m_n(n) {}
	void set_n( const uint32_t n ) { m_n=n; }
	uint32_t n() { return m_n; }
	double i; // Current flowing through branch. This is set from the last calculated current.
private:
	uint32_t m_n; // CBranch number
};

const int MAX_CNODES = 4;

// Default node number that represents no node (remember that
// Ground node is -1, and the rest are numbered from 0 to n-1
const int noCNode = -2;
// Likewise for branch (although there is no "ground" branch;
// it is merely -2 for likeness with noCNode)
const int noBranch = -2;

/**
@short Represents a circuit element (such as resistance)
@author David Saxton
*/
class Element
{
public:

// FIXME Base classes should be stupid. --
// information hiding is good programming but bad government.
	enum Type
	{
		Element_BJT,
		Element_Capacitance,
		Element_CCCS,
		Element_CCVS,
		Element_CurrentSignal,
		Element_CurrentSource,
		Element_Diode,
		Element_Inductance,
		Element_LogicIn,
		Element_LogicOut,
		Element_OpAmp,
		Element_Resistance,
		Element_VCCS,
		Element_VCVS,
		Element_VoltagePoint,
		Element_VoltageSignal,
		Element_VoltageSource
	};
	
	Element();
	virtual ~Element();
	/**
	 * This must be called when the circuit is changed. The function will get
	 * all the required pointers from ElementSet
	 */
	virtual void setElementSet( ElementSet *c );
	/**
	 * Returns a pointer to the current element set
	 */
	ElementSet *elementSet() { return p_eSet; }
	/**
	 * Tells the element which nodes to use. Remember that -1 is ground. You
	 * should refer to the individual elements for which nodes are used for what.
	 */
	void setCNodes( const int n0 = noCNode, const int n1 = noCNode, const int n2 = noCNode, const int n3 = noCNode );
	/**
	 * Tells the element it's branch numbers (if it should have one). Not
	 * all elements use this.
	 */
	void setCBranches( const int b0 = noBranch, const int b1 = noBranch, const int b2 = noBranch, const int b3 = noBranch );
	/**
	 * Returns a pointer to the given CNode
	 */
	CNode *cnode( const uint32_t num ) { return p_cnode[num]; }
	/**
	 * Returns a pointer to the given CNode
	 */
	CBranch *cbranch( const uint32_t num ) { return p_cbranch[num]; }
	/**
	 * Returns the number of branches used by the element
	 */
	int numCBranches() { return m_numCBranches; }
	/**
	 * Returns the number of circuit nodes used by the element
	 */
	int numCNodes() { return m_numCNodes; }
	/**
	 * Call this function to tell the element to calculate the
	 * current flowing *into* it's cnodes *from* the element. You
	 * can get the currents with m_cnodeI. Child class must implement this function.
	 */
	virtual void updateCurrents() = 0;
	/**
	 * Returns true for reactive elements that need stepping for numerical-integration
	 * (such as capacitors)
	 */
	virtual bool isReactive() { return false; }
	/**
	 * Returns true for NonLinear elements that need iteration to converge to a solution
	 * as the matrix A is a function of x.
	 */
	virtual bool isNonLinear() { return false; }
	/**
	 * Returns the type of element
	 */
	virtual Type type() const = 0;
	/**
	 * Call this function to tell the element to add its map to the matrix in use
	 */
	virtual void add_map() {};
	/**
	 * Does the required MNA stuff. This should be called from ElementSet when necessary.
	 */
	virtual void add_initial_dc() = 0;
	/**
	 * This is called from the Component destructor. When elementSetDeleted has
	 * also been called, this class will delete itself.
	 */
	void componentDeleted();
	void elementSetDeleted();

// what is this actually used for? is it necessary?	

	double cbranchCurrent( const int branch );
	double cnodeVoltage( const int node );

	double nodeCurrent(unsigned int i) const { return m_cnodeI[i]; }
	double checkCurrents() const; // returns error current.

protected:
	/**
	 * Resets all calculated currents in the nodes to 0
	 */
	void resetCurrents();
	double m_cnodeI[8]; ///< Current flowing into the cnodes from the element

	inline double & A_g( uint32_t i, uint32_t j );
	inline double & A_b( uint32_t i, uint32_t j );
	inline double & A_c( uint32_t i, uint32_t j );
	inline double & A_d( uint32_t i, uint32_t j );
	
	inline double & b_i( uint32_t i );
	inline double & b_v( uint32_t i );


	
	ElementSet *p_eSet;
	Matrix *p_A;
	QuickVector *p_b;

// maintained by elementset.
/* TODO: design issue, I don't think these pointers should be leaked out of
  elementset. Elements should access their nodes by index. This would allow use of
STL vector class in element set, though that may not yield much benefit.
*/

	CNode *p_cnode[MAX_CNODES];
	CBranch *p_cbranch[4];
	
	/**
	 * True when the element can do add_initial_dc(), i.e. when it has
	 * pointers to the circuit, and at least one of its nodes is not ground.
	 */
	bool b_status;
	/**
	 * Update the status, returning b_status
	 */
	virtual bool updateStatus();
	/**
	 * Set by child class - the number of branches that the element uses
	 * Typically, this is 0, but could be 1 (e.g. independent voltage source)
	 * or 2 (e.g. cccs)
	 */
	unsigned int m_numCBranches;
	/**
	 * Set by child class - the number of circuit nodes that the element uses
	 */
	unsigned int m_numCNodes;
	
private:
	bool b_componentDeleted;
	bool b_eSetDeleted; 
	double m_temp;
};

double & Element::A_g( uint32_t i, uint32_t j )
{
	if ( p_cnode[i]->isGround || p_cnode[j]->isGround )
		return m_temp;
	return p_A->g( p_cnode[i]->n(), p_cnode[j]->n() );
}

double & Element::A_b( uint32_t i, uint32_t j )
{
	if ( p_cnode[i]->isGround )
		return m_temp;
	return p_A->b( p_cnode[i]->n(), p_cbranch[j]->n() );
}

double & Element::A_c( uint32_t i, uint32_t j )
{
	if ( p_cnode[j]->isGround ) return m_temp;
	return p_A->c( p_cbranch[i]->n(), p_cnode[j]->n() );
}

double & Element::A_d( uint32_t i, uint32_t j )
{
	return p_A->d( p_cbranch[i]->n(), p_cbranch[j]->n() );
}

double & Element::b_i( uint32_t i )
{
	if ( p_cnode[i]->isGround ) return m_temp;
	
	return (*p_b)[ p_cnode[i]->n() ];
}

double & Element::b_v( uint32_t i )  {
	return (*p_b)[ p_eSet->cnodeCount() + p_cbranch[i]->n() ];
}

#endif
