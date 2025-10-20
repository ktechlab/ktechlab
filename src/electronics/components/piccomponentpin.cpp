/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "config.h"
#if HAVE_GPSIM

#include "micropackage.h"
#include "piccomponent.h"
#include "piccomponentpin.h"

#include <ktechlab_debug.h>

void PICComponentPin_logicCallback(void *objV, bool state) {
    PICComponentPin *objT = static_cast<PICComponentPin*>(objV);
    objT->logicCallback(state);
}

PICComponentPin::PICComponentPin(PICComponent *picComponent, PicPin picPin)
    : m_id(picPin.pinID)
{
    m_gOutHigh = 0.0;
    m_gOutLow = 0.0;
    m_picPin = picPin;
    m_pPICComponent = picComponent;
    m_pLogicOut = nullptr;
    m_pLogicIn = nullptr;
    m_pIOPIN = nullptr;
    m_pStimulusNode = nullptr;
    Zth = 0.0;
    Vth = 0.0;

    switch (picPin.type) {
    case PicPin::type_input: {
        m_pLogicIn = picComponent->createLogicIn(picComponent->ecNodeWithID(picPin.pinID));
        break;
    }
    case PicPin::type_bidir: {
        m_pLogicOut = picComponent->createLogicOut(picComponent->ecNodeWithID(picPin.pinID), false);
        m_gOutHigh = 0.004;
        m_gOutLow = 0.004;
        break;
    }
    case PicPin::type_open: {
        m_pLogicOut = picComponent->createLogicOut(picComponent->ecNodeWithID(picPin.pinID), false);
        m_pLogicOut->setOutputHighVoltage(0.0);
        m_pLogicOut->setOutputHighConductance(0.0);
        m_gOutHigh = 0.0;
        m_gOutLow = 0.004;
        break;
    }
    case PicPin::type_vss:
    case PicPin::type_vdd:
    case PicPin::type_mclr:
    case PicPin::type_osc:
    default:
        break;
    }

    if (m_pLogicIn) {
        //m_pLogicIn->setCallback(this, (CallbackPtr)(&PICComponentPin::logicCallback));
        m_pLogicIn->setCallback2(PICComponentPin_logicCallback, this);
    }
    if (m_pLogicOut) {
        //m_pLogicOut->setCallback(this, (CallbackPtr)(&PICComponentPin::logicCallback));
        m_pLogicOut->setCallback2(PICComponentPin_logicCallback, this);
    }
}

PICComponentPin::~PICComponentPin()
{
    if (m_pLogicIn) {
        //m_pLogicIn->setCallback(nullptr, (CallbackPtr) nullptr);
        m_pLogicIn->setCallback2(nullptr, nullptr);
    }
    if (m_pLogicOut) {
        //m_pLogicOut->setCallback(nullptr, (CallbackPtr) nullptr);
        m_pLogicOut->setCallback2(nullptr, nullptr);
    }

    delete m_pStimulusNode;
}

void PICComponentPin::attach(IOPIN *iopin)
{
    if (!iopin) {
        qCWarning(KTL_LOG) << " iopin is nullptr";
        return;
    }

    if (m_pStimulusNode) {
        qCWarning(KTL_LOG) << " Already have a node stimulus";
        return;
    }

    if (m_pIOPIN) {
        qCWarning(KTL_LOG) << " Already have an iopin";
        return;
    }

    m_pIOPIN = iopin;
    m_pStimulusNode = new Stimulus_Node(m_id.toLatin1().constData());
    m_pStimulusNode->attach_stimulus(iopin);
    m_pStimulusNode->attach_stimulus(this);

    // We need to tell the iopin whether or not we are high
    if (m_pLogicOut)
        logicCallback(m_pLogicOut->isHigh());
    else if (m_pLogicIn)
        logicCallback(m_pLogicIn->isHigh());
}

double PICComponentPin::get_Vth()
{
    if (!m_pIOPIN)
        return 0.0;

    if (m_pIOPIN->get_direction() == IOPIN::DIR_INPUT)
        return Vth;
    else
        return m_pIOPIN->get_Vth();
}

void PICComponentPin::set_nodeVoltage(double v)
{
    Q_UNUSED(v);

    if (!m_pLogicOut || !m_pIOPIN)
        return;

    if (m_pIOPIN->get_direction() == IOPIN::DIR_INPUT) {
        m_pLogicOut->setOutputHighConductance(0.0);
        m_pLogicOut->setOutputLowConductance(0.0);
    } else {
        m_pLogicOut->setHigh(m_pIOPIN->getDrivingState());
        m_pLogicOut->setOutputHighConductance(m_gOutHigh);
        m_pLogicOut->setOutputLowConductance(m_gOutLow);
    }
}

void PICComponentPin::logicCallback(bool state)
{
    if (!m_pIOPIN)
        return;

    Vth = state ? 5e10 : 0;
    bDrivingState = state;

    if (m_pIOPIN->get_direction() == IOPIN::DIR_INPUT) {
        Zth = 1e5;

        m_pIOPIN->setDrivenState(state);
        if (m_pStimulusNode)
            m_pStimulusNode->update();
    } else
        Zth = 0;
}

void PICComponentPin::resetOutput()
{
    if (m_pLogicOut)
        m_pLogicOut->setHigh(false);
}

#endif
