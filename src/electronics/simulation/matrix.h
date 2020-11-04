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

#include <math/qmatrix.h>

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
    Matrix(CUI n, CUI m);
    ~Matrix();

    /**
     * Returns true if the matrix is changed since last calling performLU()
     * - i.e. if we do need to call performLU again.
     */
    inline bool isChanged() const
    {
        return max_k < m_mat->size_m();
    }
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
     * Prints the LU-decomposed matrix to stdout
     */
    void displayLU();
    /**
     * Sets the element matrix at row i, col j to value x
     */
    double &g(CUI i, CUI j)
    {
        const unsigned int mapped_i = m_inMap[i];
        if (mapped_i < max_k)
            max_k = mapped_i;
        if (j < max_k)
            max_k = j;

        // I think I need the next line...
        if (max_k > 0)
            max_k--;

        return (*m_mat)[mapped_i][j];
    }

    double g(CUI i, CUI j) const
    {
        return (*m_mat)[m_inMap[i]][j];
    }

    double &b(CUI i, CUI j)
    {
        return g(i, j + m_n);
    }
    double &c(CUI i, CUI j)
    {
        return g(i + m_n, j);
    }
    double &d(CUI i, CUI j)
    {
        return g(i + m_n, j + m_n);
    }

    double b(CUI i, CUI j) const
    {
        return g(i, j + m_n);
    }
    double c(CUI i, CUI j) const
    {
        return g(i + m_n, j);
    }
    double d(CUI i, CUI j) const
    {
        return g(i + m_n, j + m_n);
    }
    /**
     * Returns the value of matrix at row i, col j.
     */
    double m(CUI i, CUI j) const
    {
        return (*m_mat)[m_inMap[i]][j];
    }

private:
    /**
     * Swaps around the rows in the (a) the matrix; and (b) the mappings
     */
    void swapRows(CUI a, CUI b);

    unsigned int m_n;   // number of cnodes.
    unsigned int max_k; // optimization variable, allows partial L_U re-do.

    int *m_inMap; // Rowwise permutation mapping from external reference to internal storage

    QuickMatrix *m_mat;
    QuickMatrix *m_lu;
    double *m_y; // Avoids recreating it lots of times
};

#endif
