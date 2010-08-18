/*
    KTechLab, and IDE for electronics
    IPin class, used in simulations, as model for a PinItem or Junction
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

#ifndef IPIN_H
#define IPIN_H

#include "../ktlinterfacesexport.h"

#include <QObject>

namespace KTechLab {

class IElement;
class IWire;
class QVariantMap;

/**
    \brief Interface class between the model and the simulator

    Corresponds 1:1 to a pin or a junction from a circuit.
    It is used to construct the internal simulator structures and to
    transfer back the calculated voltage and current to the circuit.

    It's different from CNode as that one is used in the simplified
    model of the circuit
 */
class KTLINTERFACES_EXPORT IPin : public QObject
{
    Q_OBJECT
    public:
        /*
          create an IPin, unassociated to anything
          */
        // IPin();
        /**
          create an IPin, based on the data stored in a component or connector
            model, having the name in te model pinName
            \param parentInModel the model from which the pin is created,
                a component or a connector
            \param pinName the name of the pin in the model
          */
        IPin(QVariantMap *parentInModel, QString pinName);
        /**
          virtual destructor
          */
        virtual ~IPin();

        // related to the model
        /**
          \return the element corresponding in the model to this pin:
            a component or a connector
          */
        QVariantMap * parentInModel() const ;        

        /**
          set the voltage and current from this node into the model
          */
        void transferStatusToModel();
        
        // state related methods
        /**
          set the voltage of this pin, in Volts
          \param voltage the new voltage of this pin
          */
        void setVoltage(double voltage);
        /**
          \return the voltage of the pin, in Volts
          */
        double voltage() const;

        /**
          set the current flowing into this pin. If the pin belongs to
          a component/element, then this method will signal an error for
          non-null parameter value
          \param current the new current flowing in the pin, in Ampers
          */
        void setCurrentIn(double current);
        /**
          \return the current flowing in the pin, in Ampers. If the pin doesn't
            belong to a component/element, then this method should return zero
          */
        double currentIn() const;

        // connection related
        /**
          set the parent element of the pin. it can be null
          \param parent the new parent of the node, it can be null
          */
        void setParent(IElement * parent);
        /**
          \return the parent of the pin; it might be null
          */
        IElement * parent() const;

        /**
          \return the list of connected wires. Pointers to the wires are just
            stored. They are not freed at pin distruction.
          */
        QList<IWire *> connections() const ;

        /**
          connect a wire to the pin
          \param wire the wire that will be connected
          */
        void connectWire(IWire * wire);
        
        /**
          disconnect one or all wires from the pin
          \param wire the wire to disconnect. If the wire is null,
                then all wires should be disconnected
          */
        void disconnectWire(IWire * wire = NULL);
        
    private:
        /** the name of the pin, used in the data transfer to model */
        QString m_name;
        /** the component/connector from where this pin came */
        QVariantMap *m_parentInModel;
        /** the parent of this element, if it doesn't correspond
            to a junction node */
        IElement *m_parent;
        /** voltage on the pin */
        double m_voltage;
        /** current flowing in the node; should be 0 if the pin belongs
            to an element */
        double m_currentIn;
        /** the wires connected to the pin */
        QList<IWire *> m_wires;
};

}

#endif // IPIN_H
