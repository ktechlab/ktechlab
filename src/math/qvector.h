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

#ifndef QVECTOR_H__KTECHLAB_
#define QVECTOR_H__KTECHLAB_

#ifndef CUI
#define CUI const unsigned int
#endif

#define EPSILON 0.000001

class QuickVector
{
public:
    QuickVector(CUI m_in);
    ~QuickVector();
    QuickVector(const QuickVector *old); // ye olde copy constructor.

    double &operator[](const int i)
    {
        changed = true;
        return values[i];
    }
    double operator[](const int i) const
    {
        return values[i];
    }

    // accessors
    // we use accessors so that we can provide range checking.
    // we use Smalltalk style naming conventions.
    double at(CUI m_a) const;
    bool atPut(CUI m_a, const double val);
    bool atAdd(CUI m_a, const double val);

    unsigned int size() const
    {
        return m;
    }

    // utility functions:
    //	void fillWithRandom();
    void fillWithZeros();
    bool swapElements(CUI m_a, CUI m_b);

    // Vector arithmetic.
    QuickVector &operator=(const QuickVector &y);
    QuickVector &operator*=(const double y);
    QuickVector &operator*=(const QuickVector &y);
    QuickVector &operator+=(const QuickVector &y);
    QuickVector &operator-(const QuickVector &y) const;

    // debugging
    void dumpToAux() const;

    /**
     * Returns true if the vector has changed since setUnchanged was last called
     */
    inline bool isChanged() const
    {
        return changed;
    }
    /**
     * Sets the changed status to false.
     */
    inline void setUnchanged()
    {
        changed = false;
    }

private:
    // We don't have a default vector size so therefore we lock the default constructor.
    QuickVector() {};
    unsigned int m;
    bool changed;
    double *values;
};

#endif // QVECTOR_H__KTECHLAB_
