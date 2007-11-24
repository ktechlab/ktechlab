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

#include <math/qvector.h>

typedef unsigned uint;

/**
@short Container for Vector of Vectors
@author David Saxton
*/
class matrix
{
public:
	matrix( const uint size );
	~matrix();
	
	QuickVector & operator[]( const uint i ) { return *(m_mat[i]); }
	const QuickVector & operator[]( const uint i ) const { return *(m_mat[i]); }
	/**
	 * Swaps the pointers to the given rows
	 */
	void swapRows( const uint a, const uint b );
	
private:
	matrix( const matrix & );
	matrix & operator= ( const matrix & );
	
	QuickVector **m_mat;
	uint m_size;
};

#endif
