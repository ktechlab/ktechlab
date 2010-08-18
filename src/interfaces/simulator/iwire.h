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

#ifndef IWIRE_H
#define IWIRE_H

#include "../ktlinterfacesexport.h"

#include <QObject>

namespace KTechLab {

class QVariantMap;

class IPin;

/**
  \brief interface class for the simulator, used for connections

  corresponds to the Connectors, but it is used by the simulator
  */
class KTLINTERFACES_EXPORT IWire : public QObject
{
    Q_OBJECT

    public:
        /**
          create a wire between two pins, and associate it with a connector
          from the model
          \param start the starting pin of the wire
          \param end the end pin of the wire
          \param parentInModel the parent of the wire in the circuit model,
            which should be a connector 
          */
        IWire( IPin *start, IPin *end, QVariantMap *parentInModel);
        /**
          destructor
          */
        virtual ~IWire();

        // status related
        /**
          set the flag if the current on the wire is known
          \param known it's true of the current on the wire is known, so the
            current() method returns valid results
          */
        void setCurrentIsKnown(bool known);
        /**
          \return if the current is known on the wire
          */
        bool currentIsKnown() const;

        /**
          set the current flowing through the connector, in Amperes
          \param current the new value of the current, in Amperes
          */
        void setCurrent(double current);
        /**
          \return the current flowing through the connector, in Amperes
          */
        double current() const;

        /**
          \return the voltage on the wire, which should be equal on
            both pins at the wire's end
          */
        double voltage() const;

        // model related
        /**
          \return the component from the model, correspoding to this wire
          */
        QVariantMap * parentInModel() const;
        /**
          transfer the current / voltage value of this wire to the model
          */
        void transferDataToModel();

        // connection related
        /**
          \return the start pin of the wire
          */
        IPin * start() const;
        /**
          \return the end pin of the wire
          */
        IPin * end() const;
        
    private:
        /** start pin of the wire */
        IPin *m_start;
        /** end pin of the wire */
        IPin *m_end;
        /** is the current value valid/calculated on the wire ? */
        bool m_currentKnown;
        /** value of the current */
        double m_current;
        /** parent of the wire in the model */
        QVariantMap *m_parentInModel;
};

}

#endif // IWIRE_H
