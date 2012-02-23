/***************************************************************************
 *    IElement interface.                                                  *
 *       A common interface for all compoent models in KTechLab            *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef IELEMENT_H
#define IELEMENT_H

#include "../ktlinterfacesexport.h"

#include <QObject>
#include <qvariant.h>

namespace KTechLab {

class IElementSet;
class IPin;

    /**
       \brief common interface for all the models used by a simulator

       This is a base class for all the component models, used in simulation

     */
    class KTLINTERFACES_EXPORT IElement : public QObject {
      Q_OBJECT

      public:

            /**
             create an element corresponding to a component in the model,
             in a given IElementSet
             \param parentInModel the component associated to this IElement
             \param numNodes number of the nodes of the element
             \param numVoltageSources number of the voltage sources
                in the model
             */
            IElement(QVariantMap & parentInModel,
                     int numPins, int numNodes, int numVoltageSources,
                     QList<QString> pinNames);
            /**
              virtual destructor
              */
            virtual ~IElement();

#if 0
            // parent related
            /**
             \return the parent of the element, in the model of the circuit
             */
            QVariantMap parentInModel() const;

            /**
             set the element set where this element belongs
             \param elemSet the IElementSet where this element belongs.
             All the elements in an IElementSet have a common MNA equation.
             */
            void setElementSet(IElementSet *elemSet);

            /**
             \return the IElementSet to which this element belongs
             */
            IElementSet * elementSet() const;

            // list of pins
            /**
             \return the number of pins of the element
             */
            int pinCount() const ;

            /**
              \return the pin belonging to the element, identified by the
                number of the pin, given as parameter
              \param number the number of the pin of the element
              */
            IPin *pin(int number) const;

            /**
             \return the pin that is identified by a given name;
                if the pin doesn't exist, return NULL
             \param nodeName the name of the node, in the element (ex. VCC, GND)
             */
            IPin* pinByName(QString nodeName);

            /**
             \return the list of pins of the element
             */
            const QList<IPin*> pins() const;

            // for the simulation model
            /**
             on each simulation step, this method is called once.
             The IElement should act in any way it  wants.
             This method must be overridden in derived classes.
             */
            virtual void actOnSimulationStep(double currentTime) = 0;
            /**
             place the values of MNA coefficients in the matrixes.
             should be owerridden by the specific types of elements
             */
            virtual void fillMatrixCoefficients() = 0;
            /**
              \return the number of voltage sources used in the model of the element
              */
            int numVoltageSources() const;
            /**
              \return the number of the nodes used in the model of the element
              */
            int numNodes() const;

                // for less typing
            #define CI const int

            /**
             set the ID of a given node. By this method, the position of a
             node in the common matrix is defined.
             \param nodeNumber the number of the node [0..numNodes()-1]
                in the model
             \param position the position of the node in the matrix.
                This vaule should be used as parameter in the calls to
                IElementSet::A_g
                IElementSet::A_b
                IElementSet::A_c
                IElementSet::b_i
                IElementSet::x_v
                where an ID of a node is required.
             */
            void setNodeID(CI nodeNumber, CI position);
            /**
             \return the ID of a given node
             \param nodeNumber the number of a node belonging to this element,
                in the range [0..numNodes()-1]
             */
            int nodeID(CI nodeNumber) const;
            /**
             Set the ID of a given voltge source. By this method, the position
             of a voltage source in the common matrix is defined.
             \param sourceNumber number of the voltage source, in the range
                [0..numVoltageSources()-1]
             \param position the position of the voltage source in the common
                matrix. This value should be used in call so
                IElementSet::A_b
                IElementSet::A_c
                IElementSet::A_d
                IElementSet::b_v
                IElementSet::x_i
                when an ID of a voltage source is required.
             */
            void setVoltageSourceID(CI sourceNumber, CI position);
            /**
             \return the ID of a voltage source in the model, of a given number
             \param sourceNumber the number of the voltage source, in the range
                [0..numVoltageSources()-1]
             */
            int voltageSourceID(CI sourceNumber);

        protected:
            // math related, matrix access

            /* simulation is done by solving the equation
                A*x = b,
                where:
                    A - matrix
                    x - vector of unknown value for current and voltage
                    b - vector

                The equation also has the form
                [ G B ] * [  ] = [ i ]
                [ C D ]   [  ]   [ v ]
                */
            /**
                access to G, node::node sub-matrix
                these methods are performance critical, so in order to
                increase the speed, instead of indirectly calling
                the associated IElementSet's methods, direct pointers
                should be cached in the IElement interfcace
                */
            double & A_g(CI i, CI j);
            /// access to B, node::branch, \see IElement::A_g
            double & A_b(CI i, CI j);
            /// access to C, branch::node, \see IElement::A_g
            double & A_c(CI i, CI j);
            /// access to D, branch::branch, \see IElement::A_g
            double & A_d(CI i, CI j);

            /** access to the column vector I, of current sources connected the nodes,
                \see IElement::A_g */
            double & b_i(CI i);
            /** access to the column vector V, of the independent voltage sources,
                \see IElement::A_g
                */
            double & b_v(CI i);

            #undef CI

            /// number of pins of this element
            const int m_numPins;

            /// number of nodes in the model
            const int m_numNodes;

            /// number of voltage sources in the model
            const int m_numVoltageSources;

            /// parent in the circuit model
            QVariantMap & m_parentInModel;

            /// the list of pins associated with this element
            QList<IPin*> m_pins;

        private:

            /// the name of the pins
            QMap<QString, IPin*> m_nameToPin;

            /// the elementSet where the element belongs
            IElementSet *m_elemSet;

            /** an array of node IDs storing the position of the element's
                nodes in the MNA equation matrix.
                Simple array is used, as access to this matrix is
                speed-critical */
            int *m_nodeIDs;

            /** array of source IDs storing the position of the element's
                voltage sources in the MNA equation matrix.
                Simple array is used, as access to this matrix is
                speed-critical */
            int *m_voltageSourceIDs;

            /** local cache for matrixes.
                This is a 2-dimensional matix of pointers in the A matrix */
            double ***m_ag;
            double ***m_ab;
            double ***m_ac;
            double ***m_ad;
            double **m_bi;
            double **m_bv;
#endif
    };
}

#endif // IELEMENT_H
