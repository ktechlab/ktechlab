/***************************************************************************
 *   Copyright (C) 2006 by Alan Grimes   *
 *   agrimes@speakeasy.net   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef QMATRIX_H__KTECHLAB_
#define QMATRIX_H__KTECHLAB_

//#ifndef QVECTOR_H
#include "qvector.h"
//#endif

// class QuickVector;

class QuickMatrix {
public :
	 QuickMatrix(CUI m_in, CUI n_in);
	 QuickMatrix(CUI m_in);
	 QuickMatrix(const QuickMatrix *old); // ye olde copy constructor.
	~QuickMatrix();

// accessors
// we use accessors so that we can provide range checking.
// we use Smalltalk style naming conventions.
	double at(CUI m_a, CUI n_a) const;
	bool atPut(CUI m_a, CUI n_a, const double val);
	bool atAdd(CUI m_a, CUI n_a, const double val); // just give a negative val to subtract. =)

	bool isSquare() const;

	double *&operator[]( const int i) { return values[i]; }
	const double *operator[]( const int i) const { return values[i]; }

	unsigned int size_m() const;
	unsigned int size_n() const;

// functions for some elementary row operations.
// these are actually procedures because they operate on the current matrix rather than
// producing a results matrix.  
	bool scaleRow(CUI m_a, const double scalor);
		// changes B by adding A.
	bool addRowToRow(CUI m_a, CUI m_b);
		// changes B by adding the result of A times a scalor 
	bool scaleAndAdd(CUI m_a, CUI m_b, const double scalor);
	bool partialScaleAndAdd(CUI m_a, CUI m_b, const double scalor);
	bool partialSAF(CUI m_a, CUI m_b, CUI from, const double scalor);
	bool swapRows(CUI m_a, CUI m_b);

// functions that accelerate certain types of
// operations that would otherwise require millions of at()'s
	double multstep(CUI row, CUI pos, CUI col) const;
	double multRowCol(CUI row, CUI col, CUI lim) const;

	QuickMatrix *transposeSquare() const; // Multiplies self by transpose.
	QuickVector *transposeMult(const QuickVector *operandvec) const;

// utility functions:
	void fillWithRandom();
	void fillWithZero();
	double rowsum(CUI m);
	double absrowsum(CUI m);
	QuickVector *normalizeRows();

// Matrix arithmetic.
	QuickMatrix *operator +=(const QuickMatrix *othermat);
	QuickMatrix *operator *=(const double y);
	QuickMatrix *operator =(const double y); // sets the diagonal to a constant.
//	QuickMatrix *operator =(const QuickMatrix *oldmat);
	QuickMatrix *operator *(const QuickMatrix *operandmat) const;

	QuickVector *operator *(const QuickVector *operandvec) const;

// debugging
	void dumpToAux() const;

private :
// We don't have a default matrix size so therefore we lock the default constructor. 
	QuickMatrix() {};

	void allocator();

	unsigned int m, n;
	double **values;
};

#endif
