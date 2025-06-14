/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "oscilloscopedata.h"
#include "oscilloscope.h"

using namespace std;

// BEGIN class ProbeData
ProbeData::ProbeData(int id)
    : m_id(id)
    , m_drawPosition(0.5)
    , m_resetTime(Simulator::self()->time())
    , m_color(Qt::black)
{
}

ProbeData::~ProbeData()
{
    unregisterProbe(m_id);
}

void ProbeData::setColor(QColor color)
{
    m_color = color;
    Q_EMIT displayAttributeChanged();
}
// END class ProbeData

// BEGIN class LogicProbeData
LogicProbeData::LogicProbeData(int id)
    : ProbeData(id)
{
    m_data = new vector<LogicDataPoint>;
}

void LogicProbeData::addDataPoint(LogicDataPoint data)
{
    if (m_data->size() < MAX_PROBE_DATA_SIZE) {
        m_data->push_back(data);
    }
}

void LogicProbeData::eraseData()
{
    bool lastValue = false;
    bool hasLastValue = false;

    if (!m_data->empty()) {
        lastValue = m_data->back().value;
        hasLastValue = true;
    }

    delete m_data;
    m_data = new vector<LogicDataPoint>;

    m_resetTime = Simulator::self()->time();

    if (hasLastValue)
        addDataPoint(LogicDataPoint(lastValue, m_resetTime));
}

uint64_t LogicProbeData::findPos(uint64_t time) const
{
    unsigned int pos = m_data->size();
    if (!time || !pos)
        return 0;

    // binary search

    // TODO: test and debug this code.
    unsigned int top = pos;
    pos >>= 1;
    unsigned int bottom = 0;

    do {
        uint64_t datatime = (*m_data)[pos].time;
        if (datatime == time)
            return pos;

        if (datatime >= time) {
            top = pos;
            pos -= (top - bottom) >> 1;
        } else {
            bottom = pos;
            pos += (top - bottom) >> 1;
        }
        // try to avoid infinite loop when top-bottom==1
        if (top - bottom == 1) {
            if (datatime >= time)
                bottom = top;
            else
                top = bottom;
        }

    } while (top != bottom && pos != bottom);

    return pos;
}
// END class LogicProbeData

// BEGIN class FloatingProbeData
FloatingProbeData::FloatingProbeData(int id)
    : ProbeData(id)
{
    m_data = new vector<float>;
    m_scaling = Linear;
    m_upperAbsValue = 10.0;
    m_lowerAbsValue = 0.1;
}

void FloatingProbeData::addDataPoint(float data)
{
    if (m_data->size() < MAX_PROBE_DATA_SIZE) {
        m_data->push_back(data);
    }
}

void FloatingProbeData::eraseData()
{
    delete m_data;
    m_data = new vector<float>;

    m_resetTime = Simulator::self()->time();
}

uint64_t FloatingProbeData::findPos(uint64_t time) const
{
    if (time <= 0 || uint64_t(time) <= m_resetTime)
        return 0;

    uint64_t at = uint64_t((time - m_resetTime) * double(LINEAR_UPDATE_RATE) / double(LOGIC_UPDATE_RATE));

    if (m_data->size() <= at) { // index is out of bound
        if (at > 0) {
            --at;
        }
    }

    return at;
}

uint64_t FloatingProbeData::toTime(uint64_t at) const
{
    return uint64_t(m_resetTime + (at * LOGIC_UPDATE_RATE * LINEAR_UPDATE_PERIOD));
}

void FloatingProbeData::setScaling(Scaling scaling)
{
    if (m_scaling == scaling)
        return;

    m_scaling = scaling;
    Q_EMIT displayAttributeChanged();
}

void FloatingProbeData::setUpperAbsValue(double upperAbsValue)
{
    if (m_upperAbsValue == upperAbsValue)
        return;

    m_upperAbsValue = upperAbsValue;
    Q_EMIT displayAttributeChanged();
}

void FloatingProbeData::setLowerAbsValue(double lowerAbsValue)
{
    if (m_lowerAbsValue == lowerAbsValue)
        return;

    m_lowerAbsValue = lowerAbsValue;
    Q_EMIT displayAttributeChanged();
}
// END class FloatingProbeData

#include "moc_oscilloscopedata.cpp"
