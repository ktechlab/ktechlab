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

// #ifndef QVECTOR_H
#include "qvector.h"
// #endif

#include <cstdlib> // for null 
#include <cmath>
#include <cassert>
#include <qdebug.h>
#include <iostream>



using namespace std;

/*
#ifndef BADRNG_H
#include "badrng.h"
#endif
*/

// ######################################

double QuickVector::at(CUI m_a) const {
//	if(!m_a || m_a > m) return NAN;

	return values[m_a];
}

// #####################################

bool QuickVector::atPut(CUI m_a, const double val) {
	if(m_a >= m) return false;

	values[m_a] = val;
	changed = true;
	return true;
}

// #####################################

bool QuickVector::atAdd(CUI m_a, const double val) {
	if(m_a >= m) return false;

	values[m_a] += val;
	changed = true;
	return true;
}

// #####################################

QuickVector::QuickVector(CUI m_in)
	: m(m_in), changed(true)
{
	assert(m);
	values = new double[m];
	memset(values, 0, sizeof(double) *m);
}

// #####################################

QuickVector::QuickVector(const QuickVector *old)
	: m(old->m), changed(old->changed) {
	assert(m);
	values = new double[m];

	for(unsigned int j = 0; j < m; j++) 
		values[j] = old->values[j];
}

// #####################################

QuickVector::~QuickVector() {
	delete[] values;
}

// #####################################

/*
void QuickVector::fillWithRandom() {
	for(unsigned int j = 0; j < m; j++)
		values[j] = drng();
}
*/

// #####################################

void QuickVector::fillWithZeros() {
	memset(values, 0, m*sizeof(double));
	changed = true;
}

// #####################################

QuickVector &QuickVector::operator-(const QuickVector &y) const {
//	if(y.m != m) abort();

	QuickVector *ret;
	ret = new QuickVector(m);

	for(unsigned int i = 0; i < m; i++) ret->values[i] = values[i] - y.values[i];

	return *ret;
}

// ####################################

void QuickVector::dumpToAux() const {
	for(unsigned int i = 0; i < m; i++) cout << values[i] << ' ';
	cout << endl;
}

// ####################################

bool QuickVector::swapElements(CUI m_a, CUI m_b) {
	if(m_a >= m || m_b >= m) return false;

	double temp = values[m_a];
	values[m_a] = values[m_b];
	values[m_b] = temp;
	changed = true;

	return true;
}

// ###################################

QuickVector &QuickVector::operator=(const QuickVector &y) {
	assert(y.m == m);

	for(unsigned int i = 0; i < m; i++) values[i] = y.values[i];
	changed = true;
	return *this;
}

// ###################################

QuickVector &QuickVector::operator *=(const QuickVector &y) {
//	if(y.m != m) return nullptr;

	for(unsigned int i = 0; i < m; i++) values[i] *= y.values[i];
	changed = true;
	return *this;
}

// ###################################

QuickVector &QuickVector::operator +=(const QuickVector &y) {
//	if(y.m != m) return nullptr;

	for(unsigned int i = 0; i < m; i++) values[i] += y.values[i];
	changed = true;
	return *this;
}

// ###################################

