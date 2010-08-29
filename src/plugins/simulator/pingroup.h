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

#ifndef KTECHLAB_PINGROUP_H
#define KTECHLAB_PINGROUP_H

#include <QObject>


namespace KTechLab {

class IWire;

class IPin;

/**
 \brief Class representing the mapping from several connected pins to one node
 A group of connected pins (\ref IPin) has to be mapped into one
 voltage node used in the Modified Nodal Analysis.
 The instances of this class are constant during their lifetime:
 all the pins and wires are added at creation time, and
 no new pins can be added later.

 This class holds a reference to:
 \li all the pins that are connected together
 \li all the wires that connect the nodes

 The set of pins in a group can be divided in two parts:
 \li external pins: such pins belong to an element, the current
    flowing in them is coming from the element (\ref IElement).
 \li internal pins: these pins don't belong to an element, but
    they are connected only to wires

 A pin can belong only to one pingroup.

 The wires cannot form a loop, because that would make impossible to
 calculat the currents through the wires.
 */
class PinGroup : public QObject
{
    Q_OBJECT

    public:
        /**
         Constructor.
         Creates a group of connected pins on a graph formed by
         a set of pins and a set of wires,
         starting from a given pin.
         */
        PinGroup(
            QList<IPin*> pins,
            QList<IWire*> wires,
            IPin * startPin
            );
        /**
         destrcutor
         */
        virtual ~PinGroup();

        /**
         \return true, if a pin can be found in this group,
            false otherwise
         \param pin the pin about which the information is queried
         */
        bool containsPin(IPin *pin) const;
        /**
         \return the list of pins in the group
         */
        const QList<IPin*> pins() const;
        
        /**
         \return true, if a wire can be found in this group,
            false otherwise
         \param wire the wire about which the information is queried
         */
        bool containsWire(IWire *wire) const;

        /**
         Set the voltage for all the pins and wires in the group.
         Because everything in the group is connected, all the
         pins and wires are on the same potential /
         have the same voltage
         \param voltage the voltage on the entire group
         */
        void setVoltage(double voltage);

        /**
         calculates the currents through all the wires inside the group.
         This method supposes that all the currents flowing into the pins,
         from their parent elements, is known.
         */
        void calculateInternalCurrents();

    private:
        QList<IPin*> m_pins;
        QList<IWire*> m_wires;
};

}

#endif // KTECHLAB_PINGROUP_H
