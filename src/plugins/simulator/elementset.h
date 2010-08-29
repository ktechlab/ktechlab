/*
    KTechLab, an IDE for electronics
    Copyright (C) 2010 Zoltan Padrah

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef ELEMENTSET_H
#define ELEMENTSET_H

#include "interfaces/simulator/ielementset.h"

namespace KTechLab {

class PinGroup;

class IPin;

class IWire;

class Matrix;
class QuickVector;

/**
 \brief implementation of IElementSet interface

 For introduction, see \ref IElementSet
 
 At construction time, the following are created:
 \li the list of elements in the set
 \li the IDs for the equations and voltage sources
 \li the matrixes that will store the equations

 */
class ElementSet : public IElementSet
{
    public:
        /** constructor */
        ElementSet(IElement *start,
                   QList<IElement*> elements,
                   QList<PinGroup*> pinGroups);
                   
        /** destructor */
        virtual ~ElementSet();

        /**
         Try to solve the equations of the contained set of elements.
         If the internal data structures are invalidated, then the
         IElementSet tries to rebuild them first, by calling
         IElement::fillMatrixCoefficients()
         on the IElements
         */
        virtual void solveEquations();

        #define CUI const unsigned int

        /// access to G, node::node sub-matrix.
        ///implementation provided in the simulator
        virtual double & A_g(CUI i, CUI j);
        /// access to B, node::branch
        ///implementation provided in the simulator
        virtual double & A_b(CUI i, CUI j);
        /// access to C, branch::node
        ///implementation provided in the simulator
        virtual double & A_c(CUI i, CUI j);
        /// access to D, branch::branch
        ///implementation provided in the simulator
        virtual double & A_d(CUI i, CUI j);

        /// access to the column vector I, of current sources connected the nodes
        ///implementation provided in the simulator
        virtual double & b_i(CUI i);
        /// access to the column vector V, of the independent voltage sources
        ///implementation provided in the simulator
        virtual double & b_v(CUI i);

        /// access to the column vector V, of the unknown node voltages
        ///implementation provided in the simulator
        virtual double x_v(CUI i);
        /// access to the column vector J, of the current through
        ///     the independent voltage sources
        ///implementation provided in the simulator
        virtual double x_j(CUI i);

        #undef CUI

    private:
        /// build the list of elements in the ElementSet
        void buildElementList();
        /// create the matrix for calculations
        void allocateMatrixes();
        /// assing IDs for nodes and voltage sources
        void assignNodeAndSourceIds();
        Matrix *m_a;
        QuickVector *m_x;
        QuickVector *m_b;
};

}

#endif // ELEMENTSET_H
