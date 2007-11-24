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

// matrix stuff...

matrix::matrix( const uint size )
{
	m_size = size;
	m_mat = new QuickVector*[m_size];
	for ( uint i=0; i<m_size; ++i )
	{
		m_mat[i] = new QuickVector(m_size);
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
	QuickVector *v = m_mat[a];
	m_mat[a] = m_mat[b];
	m_mat[b] = v;
}



