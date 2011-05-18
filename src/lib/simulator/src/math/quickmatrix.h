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

#ifndef QUICKMATRIX_H
#define QUICKMATRIX_H

#include "quickvector.h"

// FIXME if we want to optimize memory allocations, we should not allocate
// memory in methods, but pass the needed data structure to it as a parameter

/**
 * @class QuickMatrix
 * @short a class to store a matrix and allow some special operations on it
 * Responsabilities:
 * \li store a matrix
 * \li scale row, column or entire matrix
 * \li <>
 * 
 * In some places the existence/range check condition is insufficient...
 **/
class QuickMatrix {
public :
    // constructor/destructor
    /**
     * Create a matrix of given size
     * @param m_in number of rows (Y coord.)
     * @param n_in number of columns (X coord)
     */
    QuickMatrix(CUI m_in, CUI n_in);

    /**
     * Createa a square matrix of given size
     * @param m_in dimension of matrix
     */
    QuickMatrix(CUI m_in);

    /**
     * ye olde copy constructor.
     * @param old original matrix, from which the copy is made
     */
    QuickMatrix(const QuickMatrix *old); 

    /**
     * Standard destructor
     */
    ~QuickMatrix();

    // accessors
        // we use accessors so that we can provide range checking.
        // we use Smalltalk style naming conventions.
    /**
     * get an element from a given position
     * @param m_a "m" position index, row ( Y coord.)
     * @param n_a "n" position index, columnt ( X coord.)
     * @return the value from that position
     */
    double at(CUI m_a, CUI n_a) const;

    /**
     * Set an element on a given position
     * @param m_a "m" position index, row ( Y coord.)
     * @param n_a "n" position index, column ( X coord.)
     * @param val the new value
     * @return true, if the operation was successful (the indexes are in valid range), false otherwise
     */
    bool atPut(CUI m_a, CUI n_a, const double val);

    /**
     * Add a value to an element of the matrix
     * @param m_a "m" position index, row ( Y coord.)
     * @param n_a "n" position index, column ( X coord.)
     * @param val the value to add
     * @return true, if the operation was successful (the indexes are in valid range), false otherwise
     */
    bool atAdd(CUI m_a, CUI n_a, const double val); // just give a negative val to subtract. =)

    /**
     * @return "m" size of the matrix == number of rows == max. Y coordinate
     */
    inline unsigned int size_m() const { return m; }

    /**
     * @return "n" size of the matrix == number of columns == max. X coordinate
     */
    inline unsigned int size_n() const { return n; }

    // operators
        // FIXME I don't like these operators; thely look very error-prone
    inline double *&operator[](const int i) { return values[i]; }
    inline const double *operator[](const int i) const { return values[i]; }

        // Matrix arithmetic.
        // FIXME The declaration of some operators is incorrect. Should return reference instead of pointer
    QuickMatrix *operator +=(const QuickMatrix *othermat);
    QuickMatrix *operator *=(const double y);
    QuickMatrix *operator =(const double y); // sets the diagonal to a constant.
   // QuickMatrix *operator =(const QuickMatrix *oldmat);
    QuickMatrix *operator *(const QuickMatrix *operandmat) const;

    QuickVector *operator *(const QuickVector *operandvec) const;

    // status info
    /**
     * @return true, if the matrix is square, false in other cases
     */
    bool isSquare() const;
    
    /**
     * @param m number of row == Y coordinate of row elements
     * @return sum of the elements on the given row
     */
    double rowsum(CUI m);   // FIXME look at implementation; it will always return NAN, because m >= m

    /**
     * @param m number of row == Y coordinate of row elements
     * @return sum of the module of elements on the given row
     */
    double absrowsum(CUI m);

    // functions for some elementary row operations.
        // these are actually procedures because they operate on the current matrix rather than
        // producing a results matrix.  
    /**
     * multiply all the elements on a given row with a constant
     * @param m_a the row number == Y coord. of elements
     * @param scalor the constant
     * @return true, if the operation was successful (row number in range), false otherwise
     */
    bool scaleRow(CUI m_a, const double scalor);

    /**
     * Add one row multiplied with a constant to another row. So: a[m_a][i] = a[m_a][i] + scalor * a[m_b][i], for all j
     * @param m_a row index 1 == Y coord. 1
     * @param m_b row index 2 == Y coord. 2
     * @param scalor constant to multiply with
     * @return true, if the operation was successful (row number in range), false otherwise
     */
    bool scaleAndAdd(CUI m_a, CUI m_b, const double scalor);

    // FIXME bug in the body of this method, see note there
    bool partialScaleAndAdd(CUI m_a, CUI m_b, const double scalor);

    /**
     * Add one row multiplied with a constant to another row. So: a[m_a][i] = a[m_a][i] + scalor * a[m_b][i], 
     * for \c from <= \c j \c < \c n
     * @param m_a row index 1 == Y coord. 1
     * @param m_b row index 2 == Y coord. 2
     * @param from start column index == start X coord.
     * @param scalor constant to multiply with
     * @return true, if the operation was successful (row number in range), false otherwise
     */
    bool partialSAF(CUI m_a, CUI m_b, CUI from, const double scalor);
    
    /**
     * Swap two rows in the matrix, identified by their indexes
     * @param m_a row index 1 == Y coord. 1
     * @param m_b row index 2 == Y coord. 2
     * @return true, if the operation was successful (row numbers in range), false otherwise
     */
    bool swapRows(CUI m_a, CUI m_b);

    // functions that accelerate certain types of
    // operations that would otherwise require millions of at()'s
    /**
     * @return values[row][pos] * values[pos][col]
     * note: no error checking performed
     */
    double multstep(CUI row, CUI pos, CUI col) const;
    
    /**
     * calculates an element of matrix multiplication (see implmentation)
     * @param row start row 
     * @param col start column
     * @param lim number of elements to add
     * @return sum a[row][j]*a[j][col], for j=0..lim-1
     */
    double multRowCol(CUI row, CUI col, CUI lim) const;

    /**
     * Allocates a new matrix. This creates a square matrix from a rectangular matrix. It is useful for
data analysis where you might have M values for N variables. 
     * @return @todo document
     */
    QuickMatrix *transposeSquare() const; 

    // Multiplies self by transpose.
    QuickVector *transposeMult(const QuickVector *operandvec) const;

    // utility functions:
//    void fillWithRandom();  ///< no implementation?

    /**
     * Fills the matrix with zeroes
     */
    void fillWithZero();

    /**
     * For each row, calculatex maximum and nomalizes the elements, such that all elements will be in the range [-1,1]
     * @return newly allocated the scalars used for division in order to normalize the matrix rows
     */
    QuickVector *normalizeRows();

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
