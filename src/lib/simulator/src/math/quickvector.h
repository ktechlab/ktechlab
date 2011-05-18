/***************************************************************************
 *   Copyright (C) 2006 by Alan Grimes   *
 *   agrimes@speakeasy.net   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *(at your option) any later version.                                   *
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

#ifndef QUICKVECTOR_H
#define QUICKVECTOR_H

/** 
 * \def CUI 
 * Constant Unsigned Int
 */
#ifndef CUI
#define CUI	const unsigned int
#endif

/**
 * \def EPSILON
 * if |x| < EPSILON then x = 0, by convention
 */

#define EPSILON	0.000001

/**
 * \class QuickVector
 * is a class that stores an array of \c double values and defines some operations on them
 * Operations:
 * \li accessing by index
 * \li adding a value to an element specified by its index
 * \li filling the vector with zeroes
 * \li swapping elements
 * \li mathematical operations with a vector and a scalar, or between 2 vectors
 * \li getting and setting a \c changed flag on the vector (is this really needed?)
 * \li dupming the elements of the vector to the standard output
 */

class QuickVector {
public :
    // constructor
    /**
    * \short creates a new vector of size m_in
    * @param m_in the size of the new vector to be created
    */
    QuickVector(CUI m_in);

    /// ye olde copy constructor.
    QuickVector(const QuickVector *old); 

    /// simple destructor
    ~QuickVector();

    /**
     * @return the size of the vector
     */
    inline unsigned int size() const { 
        return m; 
    }


    // accessors
    // we use accessors so that we can provide range checking.
    // we use Smalltalk style naming conventions.
    /**
     * get element from a given position
     * @param m_a the index of the element
     * @return the \c double at the position
     */
    double at(CUI m_a) const;

    /**
     * set the element at a given position; also set the \c changed flag
     * @param m_a the index of elements
     * @param val the new value
     * @return true if the operation was successful (basically if the index had a valid value), false otherwise
     */
    bool atPut(CUI m_a, const double val);

    /**
     *  add a value to an existing element identified by its index in the vector; also sets the \c changed flag
     * @param m_a the index
     * @param val the value to add to the existing one
     * @return true, if the index is in valid range, false otherwise
     */
    bool atAdd(CUI m_a, const double val);

    // utility functions:
    //	void fillWithRandom();
    /**
     * fill the vector with 0 values
     */
    void fillWithZeros();

    /**
     * swap the elements defined by positions m_a and m_b ; also sets the \c changed flag
     * @param m_a index 1
     * @param m_b index 2
     * @return true, if both indexes were in valid range, false otherwise
     */
    bool swapElements(CUI m_a, CUI m_b);

    // operators

    // accessors by index, as operators
    /**
     * get element at position i
     * @param i the index of the element to get 
     * @return the element from the 
     */
    // FIXME what about these operators?
    inline double &operator[]( const int i) { 
        changed = true; 
        return values[i]; 
    }

    inline double operator[]( const int i) const { 
        return values[i]; 
    }

    // Vector arithmetic.
    /**
     * set the values of the vector from vector \c y and sets the \c changed flag. Equal size of the vectors is asserted.
     * @param y stores the new values
     * @return the old vector, filled with new values
     */
    QuickVector &operator=(const QuickVector &y);

    /**
     *  multiply all the elements of the current vector with a constant: a[i] = a[i] * y
     * @param y the other vector
     * @return the vector with changed values
     */
    QuickVector &operator*=(const double y);

    /**
     * multiply all the values in the current vector with the corresponding values in vector \c y : a[i] = a[i] * y[i] 
     * and set the \c changed flag
     * the size of the 2 vectors is asserted equal
     * @param y vector with new values
     * @return the old vector, with changed values
     */
    QuickVector &operator*=(const QuickVector &y);

    /**
     * add to all the values in the current vector the corresponding values in vector \c y : a[i] = a[i] + y[i] 
     * and set the \c changed flag
     * the size of the 2 vectors is asserted equal
     * @param y vector with new values
     * @return the old vector, with changed values
     */
    QuickVector &operator+=(const QuickVector &y);

    // FIXME is this operator correctly defined?
    QuickVector &operator-(const QuickVector &y) const;

    // flag-related functions
    /**
    * @returns true if the vector has changed since setUnchanged was last called
    */
    inline bool isChanged() const { return changed; }

    /**
    * Sets the changed status to false.
    */
    inline void setUnchanged() { changed=false; }

    // debugging
    /**
     *   dump all the elements of the vector to cout
     */
    void dumpToAux() const;

private :
    /// We don't have a default vector size so therefore we lock the default constructor.
    QuickVector() {};
    /// the size of the vector
    unsigned int m;
    /// the flag
    bool changed;
    /// pointer to the actual values
    double *values;
};

#endif
