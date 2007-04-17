/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VEC_H
#define VEC_H

typedef unsigned uint;

/**
@short Vector of doubles, faster than STL vector
@author David Saxton
*/
class Vector
{
public:
	Vector( const int size );
	~Vector();
	
	double & operator[]( const int i ) { b_changed=true; return m_vec[i]; }
	double operator[]( const int i ) const { return m_vec[i]; }
	int size() const { return m_size; }
	/**
	 * Resets all values to 0
	 */
	void reset();
	/**
	 * Adds the Vector rhs to this
	 */
	Vector & operator+=( const Vector & rhs );
	/**
	 * Subtracts the Vector rhs from this
	 */
	Vector & operator-=( const Vector & rhs );
	/**
	 * Multiplies this Vector by the given scaler constant
	 */
	Vector & operator*=( double s );
	/**
	 * Sets this vector equal to the given vector
	 */
	Vector & operator=( const Vector & v );
	/**
	 * Copies the negative values of the given vector to this vector.
	 * (i.e. sets this = -rhs )
	 */
	void negative( const Vector & rhs );
	/**
	 * Returns the absolute value of this vector, defined as the squareroot
	 * of the sum of the square of each element of the vector
	 */
	double abs() const;
	/**
	 * Returns true if the vector has changed since setUnchanged was last called
	 * Note that this will return true if the vector has just been read, due to
	 * limitations with the [] operator.
	 */
	bool isChanged() const { return b_changed; }
	/**
	 * Sets the changed status to false.
	 */
	void setUnchanged() { b_changed = false; }

private:
	Vector( const Vector & );
	
	bool b_changed;
	double *m_vec;
	int m_size;
};

/**
@short Container for Vector of Vectors
@author David Saxton
*/
class matrix
{
public:
	matrix( const uint size );
	~matrix();
	
	Vector & operator[]( const uint i ) { return *(m_mat[i]); }
	const Vector & operator[]( const uint i ) const { return *(m_mat[i]); }
	/**
	 * Swaps the pointers to the given rows
	 */
	void swapRows( const uint a, const uint b );
	
private:
	matrix( const matrix & );
	matrix & operator= ( const matrix & );
	
	Vector **m_mat;
	uint m_size;
};

#endif
