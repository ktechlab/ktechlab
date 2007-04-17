/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "vec.h"

#include <assert.h>
#include <cmath>
#include <string.h>
using namespace std;

Vector::Vector( const int size )
{
	m_size = size;
	m_vec = new double[m_size];
	reset();
	b_changed = true;
}


Vector::~Vector()
{
	// Hmm...this looks like it's the correct format, although valgrind complains
	// about improper memory use. Interesting. "delete m_vec" definitely leaks
	// memory, so this seems like the lesser of two evils. I miss C memory allocation
	// somtimes, with a nice simple free :p
	delete [] m_vec;
}


void Vector::reset()
{
	for ( int i=0; i<m_size; i++ )
	{
		m_vec[i] = 0.;
	}
	b_changed = true;
}


Vector & Vector::operator += ( const Vector & rhs )
{
	assert( size() == rhs.size() );
	
	for ( int i=0; i<m_size; i++ )
		m_vec[i] += rhs[i];
	
	b_changed = true;
	return *this;
}


Vector & Vector::operator -= ( const Vector & rhs )
{
	assert( size() == rhs.size() );
	
	for ( int i=0; i<m_size; i++ )
		m_vec[i] -= rhs[i];
	
	b_changed = true;
	return *this;
}


Vector & Vector::operator *=( double s )
{
	for ( int i=0; i<m_size; i++ )
		m_vec[i] *= s;
	
	b_changed = true;
	return *this;
}

	
Vector & Vector::operator = ( const Vector & v )
{
	assert( size() == v.size() );
	memcpy( m_vec, v.m_vec, m_size * sizeof( double ) );
	b_changed = true;
	return *this;
}


void Vector::negative( const Vector & rhs )
{
	assert( rhs.size() == size() );
	
	for ( int i=0; i<m_size; i++ )
		m_vec[i] = -rhs[i];
	
	b_changed = true;
}


double Vector::abs() const
{
	double s=0;
	for ( int i=0; i<m_size; i++ )
	{
		s += m_vec[i]*m_vec[i];
	}
	return sqrt(s);
}



// matrix stuff...

matrix::matrix( const uint size )
{
	m_size = size;
	m_mat = new Vector*[m_size];
	for ( uint i=0; i<m_size; ++i )
	{
		m_mat[i] = new Vector(m_size);
	}
}

matrix::~matrix()
{
	for ( uint i=0; i<m_size; ++i )
	{
		delete m_mat[i];
	}
	delete [] m_mat;
}


void matrix::swapRows( const uint a, const uint b )
{
	if ( a == b ) return;
	Vector *v = m_mat[a];
	m_mat[a] = m_mat[b];
	m_mat[b] = v;
}



