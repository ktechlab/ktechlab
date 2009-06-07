//
// C++ Implementation: mathtest
//
// Description: 
//
//
// Author: Alan Grimes <agrimes@speakeasy.net>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <iostream>
#include <cmath>
#include "../src/math/matrix.h"

using namespace std;

int main(int argc, char **argv) {

// default to 100 and 1e-6
	unsigned int size = 100; 
	double epsilon = 1e-6;

// check arguments for [matrix size] and [error tolerance]

// TODO(optional)


// create random matrix A and vector b.
	Matrix *A = new Matrix(size);
	QuickVector *b = new QuickVector(size);

// TODO randomize matrix and vector.

	A->identity();

	for(unsigned i = 0; i < size; i++) {
		b->atPut(i, i);
	}

// create LU for A. 
	A->performLU();

// solve for x:  Ax = b
	QuickVector *x = new QuickVector(b);
	A->fbSub(x);

// multiply Ax = c
	QuickVector *c = new QuickVector(size);
	A->multiply(x,c);

// compute scalar e = sum(abs(b - c))
	double e = 0; 

	for(unsigned int i = 0; i < size; i++) {
		e += abs(b->at(i) - c->at(i));
	}

// collect garbage
	delete A;
	delete b;
	delete c;
	delete x;

// report e and PASS/FAIL based on acceptability of e. 
	cout << e;

	if(e < epsilon) { 
	// PASS
		cout << "  PASS =)" << endl;
		return 0;	
	} else {
	// FAIL
		cout << "  FAIL =(" << endl;
		return 1;
	}
}

