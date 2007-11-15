/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "matrix.h"

#include <kdebug.h>

#include <assert.h>

#include <cmath>
#include <iostream>
#include <vector>

/// Minimum value before an entry is deemed "zero"
const double epsilon = 1e-50;

Matrix::Matrix( uint n, uint m )
{
	m_n = n;
	m_m = m;
	m_size = m_n+m_m;
	
	m_mat = new matrix(m_size);
	m_lu = new matrix(m_size);
	m_y = new double[m_size];
	m_inMap = new int[m_size];
	zero();
}

Matrix::~Matrix()
{
	delete m_mat;
	delete m_lu;
	delete [] m_y;
	delete [] m_inMap;
}

void Matrix::zero()
{
	for ( uint i=0; i<m_size; i++ )
	{
		for ( uint j=0; j<m_size; j++ )
		{
			(*m_mat)[i][j] = 0.;
			(*m_lu)[i][j] = 0.;
		}
		m_inMap[i] = i;
	}
	
	max_k = 0;
}

void Matrix::swapRows( const uint a, const uint b )
{
	if ( a == b ) return;
	m_mat->swapRows( a, b );
	
	const int old = m_inMap[a];
	m_inMap[a] = m_inMap[b];
	m_inMap[b] = old;
	
	max_k = 0;
}

void Matrix::operator=( Matrix *const m )
{
	for ( uint _i=0; _i<m_size; _i++ )
	{
		uint i = m_inMap[_i];
		for ( uint j=0; j<m_size; j++ )
		{
			(*m_mat)[i][j] = m->m(i,j);
		}
	}
	
	max_k = 0;
}

void Matrix::operator+=( Matrix *const m )
{
	for ( uint _i=0; _i<m_size; _i++ )
	{
		uint i = m_inMap[_i];
		for ( uint j=0; j<m_size; j++ )
		{
			(*m_mat)[i][j] += m->m(i,j);
		}
	}
	
	max_k = 0;
}

void Matrix::performLU()
{
	uint n = m_size;
	if ( n == 0 ) return;
	
	// Copy the affected segment to LU
	for ( uint i=max_k; i<n; i++ )
	{
		for ( uint j=max_k; j<n; j++ )
		{
			(*m_lu)[i][j] = (*m_mat)[i][j];
		}
	}
	
	// LU decompose the matrix, and store result back in matrix
	for ( uint k=0; k<n-1; k++ )
	{
		double * const lu_K_K = &(*m_lu)[k][k];
		if ( std::abs(*lu_K_K) < 1e-10 )
		{
			if ( *lu_K_K < 0. ) *lu_K_K = -1e-10;
			else *lu_K_K = 1e-10;
		}
		for ( uint i=std::max(k,max_k)+1; i<n; i++ )
		{
			(*m_lu)[i][k] /= *lu_K_K;
		}
		for ( uint i=std::max(k,max_k)+1; i<n; i++ )
		{
			const double lu_I_K = (*m_lu)[i][k];
			if ( std::abs(lu_I_K) > 1e-12 )
			{
				for ( uint j=std::max(k,max_k)+1; j<n; j++ )
				{
					(*m_lu)[i][j] -= lu_I_K*(*m_lu)[k][j];
				}
			}
		}
	}
	
	max_k = n;
}

void Matrix::fbSub( Vector* b )
{
	if ( m_size == 0 ) return;
	
	for ( uint i=0; i<m_size; i++ )
	{
		m_y[m_inMap[i]] = (*b)[i];
	}
	
	// Forward substitution
	for ( uint i=1; i<m_size; i++ )
	{
		double sum = 0;
		for ( uint j=0; j<i; j++ )
		{
			sum += (*m_lu)[i][j]*m_y[j];
		}
		m_y[i] -= sum;
	}
	
	// Back substitution
	m_y[m_size-1] /= (*m_lu)[m_size-1][m_size-1];
	for ( int i=m_size-2; i>=0; i-- )
	{
		double sum = 0;
		for ( uint j=i+1; j<m_size; j++ )
		{
			sum += (*m_lu)[i][j]*m_y[j];
		}
		m_y[i] -= sum;
		m_y[i] /= (*m_lu)[i][i];
	}
	
	for ( uint i=0; i<m_size; i++ )
		(*b)[i] = m_y[i];
}

void Matrix::multiply( Vector *rhs, Vector *result )
{
	if ( !rhs || !result ) return;
	result->reset();
	for ( uint _i=0; _i<m_size; _i++ )
	{
		uint i = m_inMap[_i];
		for ( uint j=0; j<m_size; j++ )
		{
			(*result)[_i] += (*m_mat)[i][j] * (*rhs)[j];
		}
	}
}

void Matrix::displayMatrix()
{
	uint n = m_size;
	for ( uint _i=0; _i<n; _i++ )
	{
		uint i = m_inMap[_i];
		for ( uint j=0; j<n; j++ )
		{
			if ( j > 0 && (*m_mat)[i][j] >= 0 ) kdDebug() << "+";
			kdDebug() << (*m_mat)[i][j] << "("<<j<<")";
		}
		kdDebug()  << endl;
	}
}

void Matrix::displayLU()
{
	uint n = m_size;
	for ( uint _i=0; _i<n; _i++ )
	{
		uint i = m_inMap[_i];
// 		uint i = _i;
		for ( uint j=0; j<n; j++ )
		{
			if ( j > 0 && (*m_lu)[i][j] >= 0 ) std::cout << "+";
			std::cout << (*m_lu)[i][j] << "("<<j<<")";
		}
		std::cout << std::endl;
	}
	std::cout << "m_inMap:    ";
	for ( uint i=0; i<n; i++ )
	{
		std::cout << i<<"->"<<m_inMap[i]<<"  ";
	}
	std::cout << std::endl;
	/*cout << "m_outMap:   ";
	for ( uint i=0; i<n; i++ )
	{
		cout << i<<"->"<<m_outMap[i]<<"  ";
	}
	cout << endl;*/
}

Matrix22::Matrix22()
{
	reset();
}

bool Matrix22::solve()
{
	const double old_x1 = m_x1;
	const double old_x2 = m_x2;
	
	const bool e11 = std::abs((m_a11))<epsilon;
	const bool e12 = std::abs((m_a12))<epsilon;
	const bool e21 = std::abs((m_a21))<epsilon;
	const bool e22 = std::abs((m_a22))<epsilon;
	
	if (e11)
	{
		if ( e12||e21 )
			return false;
		m_x2 = m_b1/m_a12;
		m_x1 = (m_b2-(m_a22*m_x2))/m_a21;
	}
	else if (e12)
	{
		if ( e11||e22 )
			return false;
		m_x1 = m_b1/m_a11;
		m_x2 = (m_b2-(m_a21*m_x1))/m_a22;
	}
	else if (e21)
	{
		if ( e11||e22 )
			return false;
		m_x2 = m_b2/m_a22;
		m_x1 = (m_b1-(m_a12*m_x2))/m_a11;
	}
	else if (e22)
	{
		if ( e12||e21 )
			return false;
		m_x1 = m_b2/m_a21;
		m_x2 = (m_b1-(m_a11*m_x1))/m_a12;
	}
	else
	{
		m_x2 = (m_b2-(m_a21*m_b1/m_a11))/(m_a22-(m_a21*m_a12/m_a11));
		m_x1 = (m_b1-(m_a12*m_x2))/m_a11;
	}
	if ( !std::isfinite(m_x1) || !std::isfinite(m_x2) )
	{
		m_x1 = old_x1;
		m_x2 = old_x2;
		return false;
	}
	return true;
}

void Matrix22::reset()
{
	m_a11=m_a12=m_a21=m_a22=0.;
	m_b1=m_b2=0.;
	m_x1=m_x2=0.;
}

