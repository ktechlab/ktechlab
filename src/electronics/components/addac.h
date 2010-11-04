/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef ADDAC_H
#define ADDAC_H

#include "component.h"
#include "voltagepoint.h"

class Pin;
class LogicIn;
class LogicOut;
class VoltagePoint;

const int max_ADDAC_bits = 32;

/**
@author David Saxton
*/
class ADDAC : public Component {
public:
    ADDAC();
    ~ADDAC();

protected:
    virtual void propertyChanged(Property& theProperty,
                                 QVariant newValue, QVariant oldValue );
    // void dataChanged();
    /**
     * Add / remove pins according to the number of outputs the user has requested
     */
    virtual void initPins() = 0;

    int m_numBits;
    double m_range;
};

/**
@author David Saxton
 */
class ADC : public ADDAC {
public:
    ADC();
    ~ADC();

    virtual void stepNonLogic();
    virtual bool doesStepNonLogic() const { return true; }

protected:
    /**
     * Add / remove pins according to the number of outputs the user has requested
     */
    virtual void initPins();

    LogicOut *m_logic[max_ADDAC_bits];
    Pin *m_realNode;
};

/**
@author David Saxton
 */
class DAC : public ADDAC {
public:
    DAC();
    ~DAC();

    virtual void stepNonLogic();
    virtual bool doesStepNonLogic() const { return true; }

protected:
    /**
     * Add / remove pins according to the number of outputs the user has requested
     */
    virtual void initPins();

    LogicIn *m_logic[max_ADDAC_bits];
    VoltagePoint m_voltagePoint;
};

#endif
