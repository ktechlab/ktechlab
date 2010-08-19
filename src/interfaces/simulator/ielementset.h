/*
    KTechLab, and IDE for electronics
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

#ifndef IELEMENTSET_H
#define IELEMENTSET_H

#include "../ktlinterfacesexport.h"

#include <QObject>

namespace KTechLab {

class IElement;
    
/**
    \brief Storage for MNA equations of several elements

    Because elements first have to be created in order to know the
    number of nodes and voltage sources in each element, the IElementSet
    dynamically allocates matrixes after all components are added,
    and distrubutes positions in the matrix to the IElements.
    
    The implmenetation of this interface can be found in the simulator.
*/
class KTLINTERFACES_EXPORT IElementSet : public QObject
{
    Q_OBJECT

    public:
        /**
         create a new IElementSet with no elements inside
         */
        IElementSet();
        /**
         destructor
         */
        virtual ~IElementSet();

        /**
         number of nodes in the system of equations
         */
        int nodeCount() const ;
        /**
         number of independent voltage sources in the system of equations
         */
        int voltageSourceCount() const ;

        // element related
        /**
         \return the list of elements associated with this IElementSet
         */
        QList<IElement *> elements() const ;

        /**
         Add a new element to this elementSet.
         This method possibly invalidates the status of internal data
         structures.
         \param element the element that is added to the IElementSet
        */
        virtual void addElement(IElement *element) = 0;
        
        /**
         Remove an element from the IElementSet.
         This method possibly invalidates the status of internal data
         structures.
         \param element the element that is removed from IElementSet
         */
        virtual void removeElement(IElement *element) = 0;

        /**
         Try to solve the equations of the contained set of elements.
         If the internal data structures are invalidated, then the
         IElementSet tries to rebuild them first, by calling
         IElement::fillMatrixCoefficients()
         on the IElements
         */
        virtual void solveEquations() = 0;

        
        // access to the equation matrixes
        
        #define CUI const unsigned int
        
        /// access to G, node::node sub-matrix.
        ///implementation provided in the simulator
        virtual double & A_g(CUI i, CUI j) = 0 ;
        /// access to B, node::branch
        ///implementation provided in the simulator
        virtual double & A_b(CUI i, CUI j) = 0 ;
        /// access to C, branch::node
        ///implementation provided in the simulator
        virtual double & A_c(CUI i, CUI j) = 0 ;
        /// access to D, branch::branch
        ///implementation provided in the simulator
        virtual double & A_d(CUI i, CUI j) = 0 ;

        /// access to the column vector I, of current sources connected the nodes
        ///implementation provided in the simulator
        virtual double & b_i(CUI i) = 0 ;
        /// access to the column vector V, of the independent voltage sources
        ///implementation provided in the simulator
        virtual double & b_v(CUI i) = 0 ;

        /// access to the column vector V, of the unknown node voltages
        ///implementation provided in the simulator
        virtual double x_v(CUI i) = 0 ;
        /// access to the column vector J, of the current through
        ///     the independent voltage sources
        ///implementation provided in the simulator
        virtual double x_j(CUI i) = 0 ;
        
        #undef CUI

    protected:
        /// the list of IElements associated with this set
        QList<IElement*> m_elements;

        /// the number of nodes in the element set
        int m_numNodes;
        
        /// the number of independent voltage sources in the element set
        int m_numVoltageSources;
};

}

#endif // IELEMENTSET_H
