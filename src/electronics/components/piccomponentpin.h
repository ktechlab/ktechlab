/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PICCOMPONENTPIN_H
#define PICCOMPONENTPIN_H

#include "config.h"
#if HAVE_GPSIM

#include "gpsim/stimuli.h"
#include "logic.h"

#include <QString>

/**
@short Controls a pin on the PIC component
@author David Saxton
 */
class PICComponentPin : public CallbackClass, public stimulus
{
public:
    PICComponentPin(PICComponent *picComponent, PicPin picPin);
    ~PICComponentPin() override;
    /**
     * Attach this to gpsim
     */
    void attach(IOPIN *iopin);
    /**
     * Called when the IOPIN this class is associated with changes state.
     * Updates the associated LogicOut / LogicIn / etc according to what
     * type of pin this is.
     */
    void set_nodeVoltage(double v) override;
    /**
     * Called from our logic pin when the logic changes state.
     */
    void logicCallback(bool state);
    /**
     * Sets the output (if has one) to low. Called when the user stops the
     * PIC.
     */
    void resetOutput();

    double get_Vth() override;

protected:
    // Conductance of pin in different configurations
    double m_gOutHigh;
    double m_gOutLow;

    PicPin m_picPin;
    IOPIN *m_pIOPIN;
    LogicOut *m_pLogicOut;
    LogicIn *m_pLogicIn;
    PICComponent *m_pPICComponent;
    Stimulus_Node *m_pStimulusNode;
    const QString m_id;
};

#endif
#endif
