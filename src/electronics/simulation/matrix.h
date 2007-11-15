/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include "vec.h"

#include <vector>

typedef std::vector<std::vector<uint> > ETMap;

/**
This class performs matrix storage, lu decomposition, forward and backward
substitution, and a few other useful operations. Steps in using class:
(1) Create an instance of this class with the correct size
(2) Define the matrix pattern as neccessary:
	(1) Call zero (unnecessary after initial ceration) to reset the pattern
		& matrix
	(2) Call setUse to set the use of each element in the matrix
	(3) Call createMap to generate the row-wise permutation mapping for use
		in partial pivoting
(3) Add the values to the matrix
(4) Call performLU, and get the results with fbSub
(5) Repeat 2, 3, 4 or 5 as necessary.
@todo We need to allow createMap to work while the matrix has already been initalised
@short Matrix manipulation class tailored for circuit equations
@author David Saxton
*/
class Matrix
{
public:
	/**
	 * Creates a size x size square matrix m, with all values zero,
	 * and a right side vector x of size m+n
	 */
	Matrix( uint n, uint m );
	~Matrix();
	/**
	 * Sets all elements to zero
	 */
	void zero();

	/**
	 * Returns true if the matrix is changed since last calling performLU()
	 * - i.e. if we do need to call performLU again.
	 */
	inline bool isChanged() const { return max_k < m_size; }
	/**
	 * Performs LU decomposition. Going along the rows,
	 * the value of the decomposed LU matrix depends only on
	 * the previous values.
	 */
	void performLU();
	/**
	 * Applies the right side vector (x) to the decomposed matrix,
	 * with the solution returned in x.
	 */
	void fbSub( Vector* x );
	/**
	 * Prints the matrix to stdout
	 */
	void displayMatrix();
	/**
	 * Prints the LU-decomposed matrix to stdout
	 */
	void displayLU();
	/**
	 * Sets the element matrix at row i, col j to value x
	 */
	double& g( uint i, uint j )
	{
		i = m_inMap[i];
		if ( i<max_k ) max_k=i;
		if ( j<max_k ) max_k=j;
		
		// I think I need the next line...
		if ( max_k>0 ) max_k--;
		
		return (*m_mat)[i][j];
	}
	double& b( uint i, uint j ) { return g( i, j+m_n ); }
	double& c( uint i, uint j ) { return g( i+m_n, j ); }
	double& d( uint i, uint j ) { return g( i+m_n, j+m_n ); }
	const double g( uint i, uint j ) const { return (*m_mat)[m_inMap[i]][j]; }
	const double b( uint i, uint j ) const { return g( i, j+m_n ); }
	const double c( uint i, uint j ) const { return g( i+m_n, j ); }
	const double d( uint i, uint j ) const { return g( i+m_n, j+m_n ); }
	/**
	 * Returns the value of matrix at row i, col j.
	 */
	const double m( uint i, uint j ) const
	{
		return (*m_mat)[m_inMap[i]][j];
	}
	/**
	 * Multiplies this matrix by the Vector rhs, and places the result
	 * in the vector pointed to by result. Will fail if wrong size.
	 */
	void multiply( Vector *rhs, Vector *result );
	/**
	 * Sets the values of this matrix to that of the given matrix
	 */
	void operator=( Matrix *const m );
	/**
	 * Adds the values of the given matrix to this matrix
	 */
	void operator+=( Matrix *const m );
	
private:
	/**
	 * Swaps around the rows in the (a) the matrix; and (b) the mappings
	 */
	void swapRows( const uint a, const uint b );
	
// 	// Generates m_outMap from m_inMap
// 	void genOutMap();
	
	uint m_n;
	uint m_m;
	uint m_size;
	uint max_k;
	
	int *m_inMap; // Rowwise permutation mapping from external reference to internal storage

	matrix *m_mat;
	matrix *m_lu;
	double *m_y; // Avoids recreating it lots of times
};


/**
This class provides a very simple, lightweight, 2x2 matrix solver.
It's fast and reliable. Set the values for the entries of A and b:

A x = b

call solve() (which returns true if successful - i.e. exactly one solution to the
matrix), and get the values of x with the appropriate functions.

@short 2x2 Matrix
@author David Saxton
*/
class Matrix22
{
public:
	Matrix22();
	
	double &a11() { return m_a11; }
	double &a12() { return m_a12; }
	double &a21() { return m_a21; }
	double &a22() { return m_a22; }
	
	double &b1() { return m_b1; }
	double &b2() { return m_b2; }
	
	/**
	 * Solve the matrix. Returns true if successful (i.e. non-singular), else
	 * false. Get the solution with x1() and x2().
	 */
	bool solve();
	/**
	 * Resets all entries to zero
	 */
	void reset();
	
	double x1() const { return m_x1; }
	double x2() const { return m_x2; }
	
private:
	double m_a11, m_a12, m_a21, m_a22;
	double m_b1, m_b2;
	double m_x1, m_x2;
};


#endif
