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

//#ifndef QMATRIX_H
#include "qmatrix.h"
//#endif

#include <cstdlib> // for NULL
#include <cmath>
#include <cassert>
#include <iostream>
#include <cstring>



/*
#ifndef BADRNG_H
#include "badrng.h"
#endif
*/

using namespace std;

// ####################################

bool QuickMatrix::isSquare() const {
	return m == n;
} 

// ####################################

// Ideally, this should be inlined, however I am reluctant to put code in the header files which may be included in numerous
// object files locations.

unsigned int QuickMatrix::size_m() const {
	return m;
}

// ####################################

unsigned int QuickMatrix::size_n() const {
	return n;
}

// ####################################

void QuickMatrix::allocator() {
//	assert(!values);
	assert(m);
	assert(n);

	values = new double*[m];
	for(unsigned int i = 0; i < m; i++) {
		values[i] = new double[n];
	}
}

// ####################################

QuickMatrix *QuickMatrix::transposeSquare() const {
	QuickMatrix *newmat = new QuickMatrix(n);

	for(unsigned int i= 0; i < n; i++) {
		for(unsigned int j = 0; j < n; j++) {
			newmat->values[i][j] = 0;
			for(unsigned int k = 0; k < m; k++) {
				newmat->values[i][j] += values[k][i] * values[k][j];
			}
		}
	}

	return newmat;
}

// #####################################

QuickVector *QuickMatrix::transposeMult(const QuickVector *operandvec) const {
	if(operandvec->size() != m) return NULL;

	QuickVector *ret = new QuickVector(n);

	for(unsigned int i = 0; i < n; i++) {
		double sum = 0;
		for(unsigned int j = 0; j < m; j++)
			sum += values[j][i] * (*operandvec)[j];
		(*ret)[i] = sum;
	}

	return ret;
}

// ####################################

QuickMatrix::QuickMatrix(CUI m_in, CUI n_in)
	: m(m_in), n(n_in) {
	allocator();
	fillWithZero();
}

// ####################################

QuickMatrix::QuickMatrix(CUI m_in)
	: m(m_in), n(m_in) {
	allocator();
	fillWithZero();
}

// ####################################

QuickMatrix::QuickMatrix(const QuickMatrix *old)
	: m(old->m), n(old->n) {
	allocator();

	for(unsigned int j = 0; j < m; j++) {
		memcpy(values[j], old->values[j], n*sizeof(double)); // fastest method. =)
	}
}

// ####################################

QuickMatrix::~QuickMatrix() {
	for(unsigned int i = 0; i < m; i++) delete[] values[i];
	delete[] values;
}

// ####################################

double QuickMatrix::at(CUI m_a, CUI n_a) const
{
	if(m_a >= m || n_a >= n) return NAN;

	return values[m_a][n_a];
}

// ####################################

double QuickMatrix::multstep(CUI row, CUI pos, CUI col) const
{
	return values[row][pos] * values[pos][col];
}

// ####################################

double QuickMatrix::multRowCol(CUI row, CUI col, CUI lim) const
{
	const double *rowVals = values[row];

	double sum = 0;
	for(unsigned int i = 0; i < lim; i++)
		sum += rowVals[i] * values[i][col];
	return sum;
}

// ####################################

bool QuickMatrix::atPut(CUI m_a, CUI n_a, const double val) {
	if(m_a >= m || n_a >= n) return false;

	values[m_a][n_a] = val;
	return true;
}

// ####################################

bool QuickMatrix::atAdd(CUI m_a, CUI n_a, const double val) {
	if(m_a >= m || n_a >= n) return false;

	values[m_a][n_a] += val;
	return true;
}

// ####################################

bool QuickMatrix::swapRows(CUI m_a, CUI m_b) {
	if(m_a >= m || m_b >= m) return false;

	double *temp = values[m_a];
	values[m_a] = values[m_b];
	values[m_b] = temp;

	return true;
}

// ####################################

bool QuickMatrix::scaleRow(CUI m_a, const double scalor) {
	if(m_a >= m) return false;

	double *arow = values[m_a];

// iterate over n columns. 
	for(unsigned int j = 0; j < n; j++) arow[j] *= scalor;

	return true;
}

// ####################################

double QuickMatrix::rowsum(CUI m) {
	if(m >= m) return NAN;

	double *arow = values[m];

	double sum = 0.0;

// iterate over n columns. 
	for(unsigned int j = 0; j < n; j++) sum += arow[j];

	return sum;
}

// ####################################

double QuickMatrix::absrowsum(CUI m) {
	if(m >= m) return NAN;

	double *arow = values[m];

	double sum = 0.0;

// iterate over n columns. 
	for(unsigned int j = 0; j < n; j++) sum += fabs(arow[j]);

	return sum;
}

// ####################################

// behaves oddly but doesn't crash if m_a == m_b
bool QuickMatrix::scaleAndAdd(CUI m_a, CUI m_b, const double scalor) {
	if(m_a >= m || m_b >= m) return false;

	const double *arow = values[m_a];
	double *brow = values[m_b];

// iterate over n columns. 
	for(unsigned int j = 0; j < n; j++)
		brow[j] += arow[j] * scalor;

	return true;
}

// ####################################

// behaves oddly but doesn't crash if m_a == m_b
bool QuickMatrix::partialScaleAndAdd(CUI m_a, CUI m_b, const double scalor) {
	if(m_a >= m || m_b >= m) return false;

	const double *arow = values[m_a];
	double *brow = values[m_b];

// iterate over n - m_a columns.
	for(unsigned int j = m_a; j < n; j++)
		brow[j] += arow[j] * scalor;

	return true;
}

// ########################################

bool QuickMatrix::partialSAF(CUI m_a, CUI m_b, CUI from, const double scalor) {
	if(m_a >= m || m_b >= m) return false;

	const double *arow = values[m_a];
	double *brow = values[m_b];

// iterate over n - m_a columns.
	for(unsigned int j = from; j < n; j++)
		brow[j] += arow[j] * scalor;

	return true;
}

// ####################################
/*
void QuickMatrix::fillWithRandom() {
	for(unsigned int j = 0; j < m; j++) {
		double *row = values[j];
		for(unsigned int i = 0; i < n; i++) row[i] = drng();
	}
}
*/

// ####################################

QuickVector *QuickMatrix::normalizeRows() {
	QuickVector *ret = new QuickVector(m);

	for(unsigned int j = 0; j < m; j++) {
		double *row = values[j];
		unsigned int max_loc = 0;

		for(unsigned int i = 0; i < n; i++) {
			if(fabs(row[max_loc]) < fabs(row[i])) max_loc = i;
		}

		double scalar = 1.0 / row[max_loc];

		(*ret)[j] = scalar;

		for(unsigned int i = 0; i < n; i++) row[i] *= scalar;
	}

	return ret;
}

// ####################################

QuickVector *QuickMatrix::operator *(const QuickVector *operandvec) const {
	if(operandvec->size() != n) return NULL;

	QuickVector *ret = new QuickVector(m);

	for(unsigned int i = 0; i < m; i++) {
		double sum = 0;
		for(unsigned int j = 0; j < n; j++)
			sum += values[i][j] * (*operandvec)[j];
		(*ret)[i] = sum;
	}

	return ret;
}

// ####################################

QuickMatrix *QuickMatrix::operator +=(const QuickMatrix *operandmat) {
	if(operandmat->n != n || operandmat->m != m) return NULL;

	for(unsigned int i = 0; i < m; i++) {
		for(unsigned int j = 0; j < n; j++)
			values[i][j] += operandmat->values[i][j];
	}

	return this;
}

// ####################################

QuickMatrix *QuickMatrix::operator *(const QuickMatrix *operandmat) const {
	if(operandmat->m != n) return NULL;

	QuickMatrix *ret = new QuickMatrix(m,operandmat->n);

	for(unsigned int i = 0; i < m; i++) {
		for(unsigned int j = 0; j < operandmat->n; j++) {
			double sum = 0;
			for(unsigned int k = 0; k < n; k++) 
				sum += values[i][k] * operandmat->values[k][j];
			ret->values[i][j] = sum;
		}
	}

	return ret;
}

// ###################################

void QuickMatrix::dumpToAux() const {
	for(unsigned int j = 0; j < m; j++) {
		for(unsigned int i = 0; i < n; i++)
			cout << values[j][i] << ' ';
		cout << endl;
	} 
}

// ###################################

void QuickMatrix::fillWithZero() {
	for(unsigned int j = 0; j < m; j++) {
		memset(values[j], 0, n*sizeof(double)); // fastest method. =)
	}
}

// ###################################
// sets the diagonal to a constant.
QuickMatrix *QuickMatrix::operator =(const double y) {
	fillWithZero();
	unsigned int size = n;
	if(size > m) size = m;

	for(unsigned int i = 0; i < size; i++) values[i][i] = y;
	return this;
}

