/***************************************************************************
 *   Copyright (C) 2005-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef JFET_H
#define JFET_H

#include "nonlinear.h"

class JFETState
{
public:
    JFETState();
    void reset();

    JFETState operator-(const JFETState &s) const;

    double A[3][3];
    double I[3];
};

class JFETSettings
{
public:
    JFETSettings();

    double V_Th; ///< zero-bias threshold voltage
    double beta; ///< transconductance parameter
    double I_S;  ///< gate-junction saturation current
    double N;    ///< gate pn emission coeffecient
    double N_R;  ///< Isr emission coefficient
};

/**
@author David Saxton
 */
class JFET : public NonLinear
{
public:
    enum JFET_type { nJFET, pJFET };

    JFET(JFET_type type);
    ~JFET() override;

    Type type() const override
    {
        return Element_JFET;
    }
    void update_dc() override;
    void add_initial_dc() override;
    JFETSettings settings() const
    {
        return m_jfetSettings;
    }
    void setJFETSettings(const JFETSettings &settings);

protected:
    void updateCurrents() override;
    /**
     * Calculates the new JFETState from the voltages on the nodes.
     */
    void calc_eq();

    void calcIg(double V_GS, double V_GD, double V_DS, double *I_GS, double *I_GD, double *I_DS, double *g_GS, double *g_GD, double *g_DS, double *g_m) const;

    enum OpRegion { NormalCutoff, NormalSaturation, NormalLinear, InverseCutoff, InverseSaturation, InverseLinear };
    OpRegion getOpRegion(double V_DS, double V_GST, double V_GDT) const;

    void updateLim();

    JFETState m_os;
    JFETState m_ns;
    int m_pol;
    double V_GS_prev, V_GD_prev;
    double V_lim;
    JFETSettings m_jfetSettings;

    static const uint PinD, PinG, PinS;
};

#endif
