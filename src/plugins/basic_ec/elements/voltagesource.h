/***************************************************************************
 *    Voltage source model                                                     *
 *       used to simulate voltage sources in KTechLab                          *
 *    Copyright (c) 2010 Zoltan Padrah                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VOLTAGESOURCE_H
#define VOLTAGESOURCE_H

#include "interfaces/simulator/ielement.h"

/**
 @short Voltage Source
 CNode n0 is the negative terminal, CNode n1 is the positive terminal
 */
namespace KTechLab {

class VoltageSource : public IElement {

public:
    VoltageSource(QVariantMap parent);
    // VoltageSource(const double voltage = 5);

    virtual ~VoltageSource();

    void setVoltage(const double v);

        /**
         action to be taken on a new simulation step
         */
        virtual void actOnSimulationStep(double currentTime);
        /**
         fill the matrix coefficients in the MNA matrix
         */
        virtual void fillMatrixCoefficients();

protected:
    virtual void updateCurrents();
    virtual void add_initial_dc();

private:
    double m_v; // Voltage
};

}
#endif
