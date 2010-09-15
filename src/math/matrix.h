/**************************************************************************
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

#include <ostream>

#include "quickmatrix.h"

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
@short Matrix manipulation class tailored for circuit equations
@author David Saxton
*/
// TODO clean up and update documentation, as the upper statements aren't valid anymore

class Matrix
{
public:
       /**
        * Creates a size x size square matrix m, with all values zero,
        * and a right side vector x of size m+n
        */
        Matrix(CUI size);

       /**
        * Destructor
        */
        ~Matrix();

       /**
        * Returns true if the matrix is changed since last calling performLU()
        * - i.e. if we do need to call performLU again.
        */
        inline bool isChanged() const { return max_k < m_mat->size_m(); }

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
        void fbSub(QuickVector *x);

       /**
        * Prints the matrix to an output stream
        * @param outstream the stream to print to
        */
        void displayMatrix(std::ostream &outstream) const;

        /**
        * Prints the LU-decomposed matrix to an output stream
         * @param outstream the stream to print to
        */
        void displayLU(std::ostream &outstream) const;

        /**
         * Set the matrix to identity
         */
        void identity() { *m_mat = 1; max_k = 0; }

        /**
         * @todo DOCUMENT THIS
         * @param i 
         * @param j 
         * @return 
         * 
         * It seems this function is used to get / set elements in the matrix
         */
        double &g(CUI i, CUI j);

        /**
         * @todo DOCUMENT THIS 2
         * @param i 
         * @param j 
         * @return 
         */
        /*
        double  g(CUI i, CUI j) const { return m_mat->at(m_inMap[i], j); }
        */

        /**
        * Multiplies this matrix by the Vector rhs, and places the result
        * in the vector pointed to by result. Will fail if wrong size.
         * @param rhs vector to multiply the vector with
         * @param result matrix * rhs, vector @todo check if the order is right
         */
        void multiply(const QuickVector *rhs, QuickVector *result );

        /**
         * \fn Matrix::validateLU()
         * check the validity of LU factorization.
         * It's an expensive procedure only for debugging.
         * @return sum of absolute error between the original matrix and the decomposition
         * 
         * CAUTION: SUSPECT BUGS IN THIS FUNCTION JUST AS QUICKLY AS BUGS IN THE ABOVE. 
         */
        double validateLU() const;

        /**
         * @fn Matrix::validate()
         * If this here function fails, the matrix is no good and 
         * no further computation should be done. This method should be used before performLU.
         * @return true, if the matrix is valid, false otherwise
         */ 
        bool validate() const;

private:
        /**
        * Swaps around the rows in the (a) the matrix; and (b) the mappings
        */
        void swapRows(CUI a, CUI b);
        unsigned int max_k; // optimization variable, allows partial L_U re-do. 
        int *m_inMap; // Rowwise permutation mapping from external reference to internal storage

        QuickMatrix *m_mat;
        QuickMatrix *m_lu;
        double *m_y; // Avoids recreating it lots of times
};

#endif
