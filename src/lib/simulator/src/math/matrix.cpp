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

#include <cassert>
#include <cmath>
#include <cstring>

#include <iostream> 

/// Minimum value before an entry is deemed "zero"
const double epsilon = 1e-40;

Matrix::Matrix(CUI size)
	: max_k(0)
{
	m_mat = new QuickMatrix(size);
	m_lu  = new QuickMatrix(size);

	m_y     = new double[size];
	m_inMap = new int[size];

	for(unsigned int i = 0; i < size; i++ )
		m_inMap[i] = i;
}

Matrix::~Matrix()
{
	delete   m_mat;
	m_mat = 0;
	delete   m_lu;
	m_lu = 0;
	delete[] m_y;
	m_y = 0;
	delete[] m_inMap;
	m_inMap = 0;
}

double& Matrix::g(CUI i, CUI j) 
{
    const unsigned int mapped_i = m_inMap[i];
    if(mapped_i < max_k) max_k = mapped_i;

    if(j < max_k) max_k = j;

        // I think I need the next line...
    if(max_k > 0) max_k--;

    return(*m_mat)[mapped_i][j];
}
        
void Matrix::swapRows(CUI a, CUI b)
{
	if(a == b) return;
	m_mat->swapRows(a, b);
	m_lu->swapRows(a, b);

	const int old = m_inMap[a];
	m_inMap[a] = m_inMap[b];
	m_inMap[b] = old;

	max_k = 0;
}

void Matrix::performLU()
{
	unsigned int n = m_mat->size_m();
	if(n == 0 || max_k == n) return;

	if(n == 1) {
// degenerate case
		m_lu->atPut(0, 0, m_mat->at(0, 0));
		max_k = 1; 
		return;
	}

	// Copy the affected segment to LU
	unsigned tmp = n - max_k;
	for(unsigned int i = max_k; i < n; i++) {
		memcpy( (*m_lu )[i] + max_k,
			(*m_mat)[i] + max_k,
			(tmp) * sizeof(double));
	}

	// LU decompose the matrix, and store result back in matrix
	for(unsigned int k = 0; k < n-1; k++) {

		// do row permutations; 
		unsigned foo;
		if(k >= max_k) {
			double max = std::abs(m_mat->at(k,k));
			unsigned int row = k;
			for(unsigned int j = k + 1; j < n; j++) {
				double val = std::abs(m_mat->at(j,k));
				if(val > max) {
					max = val;
					row = j;
				}
			}

			if(row != k) swapRows(k,row);

			foo = k; 
		} else foo = max_k;

		foo++;

		double *const lu_K_K = &(*m_lu)[k][k];

// detect singular matrixes...
		double lu_K_K_val = *lu_K_K; // have a local copy of the data at the pointer. 
		if(std::abs(lu_K_K_val) < 1e-10) {
			if(lu_K_K_val < 0.) *lu_K_K = -1e-10;
			else *lu_K_K = 1e-10;

			lu_K_K_val = *lu_K_K;
		}
// #############

		for(unsigned int i = foo; i < n; i++) {
			(*m_lu)[i][k] /= lu_K_K_val;
		}

		for(unsigned int i = std::max(k, max_k) + 1; i < n; i++) {
			const double lu_I_K = (*m_lu)[i][k];
			if(std::abs(lu_I_K) > 1e-12) {
				m_lu->partialSAF(k, i, foo, -lu_I_K);
			}
		}
	}

	max_k = n;
}

void Matrix::fbSub(QuickVector *b)
{
	unsigned int size = m_mat->size_m();

	for(unsigned int i = 0; i < size; i++) {
		m_y[i] = (*b)[m_inMap[i]];
	}

	// Forward substitution
	for(unsigned int i = 1; i < size; i++) {
		double sum = 0;
		const double *m_lu_i = (*m_lu)[i];
		for(unsigned int j = 0; j < i; j++ ) {
			sum += m_lu_i[j] * m_y[j];
		}
		m_y[i] -= sum;
	}

	// Back substitution
	m_y[size - 1] /= (*m_lu)[size - 1][size - 1];
	for(int i = size - 2; i >= 0; i-- ) {
		double sum = 0;
		const double *m_lu_i = (*m_lu)[i];
		for(unsigned int j = i + 1; j < size; j++) {
			sum += m_lu_i[j] * m_y[j];
		}

		{
			double foo = m_y[i] - sum;
			m_y[i] = foo / (*m_lu)[i][i];
		}
	}

// I think we don't need to reverse the mapping because we only permute rows, not columns. 
	for(unsigned int i = 0; i < size; i++ )
		(*b)[i] = m_y[i];
}

void Matrix::multiply(const QuickVector *rhs, QuickVector *result)
{
	if(!rhs || !result) return;
	result->fillWithZeros();

	unsigned int size = m_mat->size_m();
	for(unsigned int _i = 0; _i < size; _i++) {
		unsigned int i = m_inMap[_i];
/* hmm, we should move the resolution of pointers involving i out of the inner loop but
there doesn't appear to be a way to obtain direct pointers into our classes inner structures.
uintWhile it is a good safety feature of our classes, it doesn't facilitate optimization in this
instance... Furthermore, our matrix class has an accelerator for this operation however it is
ignorant of row permutations and it allocates new memory for the result matrix, breaking the
interface of this method. 
*/
		for(unsigned int j = 0; j < size; j++) {
			result->atAdd(_i, (*m_mat)[i][j] * (*rhs)[j]);
		}
	}
}

void Matrix::displayMatrix(std::ostream &outstream) const
{
	unsigned int n = m_mat->size_m();
	for(unsigned int _i = 0; _i < n; _i++) {

		unsigned int i = m_inMap[_i];
		for(unsigned int j = 0; j < n; j++) {
			if(j > 0 && (*m_mat)[i][j] >= 0 ) outstream << "+";
			outstream << (*m_mat)[i][j] << "("<<j<<")";
		}
		outstream << std::endl;
	}
}

void Matrix::displayLU(std::ostream &outstream) const
{
	unsigned int n = m_mat->size_m();
	for(unsigned int _i = 0; _i < n; _i++) {
		unsigned int i = m_inMap[_i];

		for(unsigned int j = 0; j < n; j++ ) {
			if ( j > 0 && (*m_lu)[i][j] >= 0 ) outstream << "+";
			outstream << (*m_lu)[i][j] << "(" << j << ")";
		}
		outstream << std::endl;
	}

	outstream << "m_inMap:    ";
	for(unsigned int i = 0; i < n; i++ ) {
		outstream << i << "->" << m_inMap[i] << "  ";
	}

	outstream << std::endl;
}

/*!
    \fn Matrix::validateLU()

check the validity of LU factorization.
It's an expensive procedure only for debugging.

CAUTION: SUSPECT BUGS IN THIS FUNCTION JUST AS QUICKLY AS BUGS IN THE ABOVE. 
 */
double Matrix::validateLU() const 
{
	unsigned int size = m_mat->size_m();

//assert(max_k == size); // sanity check, it doesn't pay to misuse this function. =P 

	QuickMatrix *A_check = new QuickMatrix(size);

// Try to make a copy of A from m_lu
	for(unsigned int i = 0; i < size; i++) {
		for(unsigned int j = 0; j < size; j++) {
			double sum = (j >= i) ? m_lu->at(i,j) : 0;
			for(unsigned int k = 0; k < i && k <= j; k++) 
				sum += m_lu->at(i,k) * m_lu->at(k, j);

			A_check->atPut(i, j, -sum);
		}
	}

// see whether it's actually a copy. =P 
	*A_check += m_mat;  // betchya were wondering why we put the -sum there instead of the +. ;) 

	double error = 0;
	for(unsigned int i = 0; i < size; i++) {
		error += A_check->absrowsum(i);
	}

	if(error > 1e-4 || !std::isfinite(error) ) {
// TIP: copy output into ooffice spreadsheet, make sure to select "space" as the delimiter
		std::cout << "A" << std::endl;
		m_mat->dumpToAux();
		std::cout << "LU" << std::endl; 
		m_lu->dumpToAux();
		std::cout << "errors" << std::endl;
		A_check->dumpToAux();
		std::cout << "Total Error: " << error << std::endl;
	}

// clean things up and return
	delete A_check;
	return error;
}

/*!
    \fn Matrix::validate()
If this here function fails, the matrix is no good and 
no further computation should be done. This method should be used before performLU.
 */
bool Matrix::validate() const
{
	unsigned int n = m_mat->size_m();

	for(unsigned int j = 0; j < n; j++) {
		bool isValid = false;
		for(unsigned int i = 0; i < n; i++) {
			const double val = m_mat->at(i,j);

			if(!std::isfinite(val)) return false;

			if(std::abs(val) > epsilon) {
				isValid = true; 
				break; 
			}
		}

		if(!isValid) return false;
	}

	return true;
}
