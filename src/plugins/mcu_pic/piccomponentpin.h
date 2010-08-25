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
#ifndef NO_GPSIM

#include "logic.h"
#include "gpsim/stimuli.h"

#include <qstring.h>

/**
@short Controls a pin on the PIC component
@author David Saxton
 */
class PICComponentPin : public CallbackClass, public stimulus {
public:
    PICComponentPin(PICComponent * picComponent, PicPin picPin);
    ~PICComponentPin();
    /**
     * Attach this to gpsim
     */
    void attach(IOPIN * iopin);
    /**
     * Called when the IOPIN this class is associated with changes state.
     * Updates the associated LogicOut / LogicIn / etc according to what
     * type of pin this is.
     */
    virtual void set_nodeVoltage(double v);
    /**
     * Called from our logic pin when the logic changes state.
     */
    void logicCallback(bool state);
    /**
     * Sets the output (if has one) to low. Called when the user stops the
     * PIC.
     */
    void resetOutput();

    virtual double get_Vth();

protected:
    // Conductance of pin in different configurations
    double m_gOutHigh;
    double m_gOutLow;

    PicPin m_picPin;
    IOPIN *m_pIOPIN;

// TODO: consider splitting this class into input and bi-dir pins for
// sake of simplicity.
    LogicOut m_pLogicOut;
    LogicIn m_pLogicIn;
    PICComponent *m_pPICComponent;
    Stimulus_Node *m_pStimulusNode;
    const QString m_id;
};

#endif
#endif
