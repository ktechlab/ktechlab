/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef OSCILLOSCOPEDATA_H
#define OSCILLOSCOPEDATA_H

#include <QColor>
#include <QObject>
#include <stdint.h>
#include <vector>

#define DATA_CHUNK_SIZE (8192 / sizeof(T))

/*
#define DATA_CHUNK_ARRAY_SIZE ((8192-sizeof(uint32_t))/sizeof(DataChunk<T>*))

// Allow a minimum of 64 megabytes of stored data (67108864 bytes)
/// \todo The maximum allowed amount of stored data should be configurable or
/// more intelligent (e.g. taking into account the number of probes or the
/// amount of physical memory on the system).
#define DCARRAY_ARRAY_SIZE ((67108864/(8192*DATA_CHUNK_ARRAY_SIZE))+1)
*/

/** max number of samples that a probe can hold */
#define MAX_PROBE_DATA_SIZE (1 * 1024 * 1024)
// TODO ^ should be configurable

/**
For use in LogicProbe: Every time the input changes state, the new input state
is recorded in value, along with the simulator time that it occurs at.
 */
class LogicDataPoint
{
public:
    LogicDataPoint()
        : value(0)
        , time(0)
    {
    }
    LogicDataPoint(bool v, uint64_t t)
        : value(v)
        , time(t)
    {
    }

    bool value : 1;
    uint64_t time : 63;
};

/**
@author David Saxton
 */
class ProbeData : public QObject
{
    Q_OBJECT
public:
    ProbeData(int id);
    ~ProbeData() override;

    /**
     * @returns unique id for oscilloscope, set on construction
     */
    int id() const
    {
        return m_id;
    }
    /**
     * Set the proportion (0 = top, 1 = bottom) of the way down the
     * oscilloscope view that the probe output is drawn. If the proportion
     * is out of range ( <0, or >1), then the drawPosition is set to 0/1
     */
    void setDrawPosition(float drawPosition)
    {
        m_drawPosition = drawPosition;
    }
    /**
     * Returns the draw position. Default is 0.5.
     * @see setDrawPosition
     */
    float drawPosition() const
    {
        return m_drawPosition;
    }
    /**
     * Set the colour that is used to display the probe in the oscilloscope.
     * Default is black.
     */
    void setColor(QColor color);
    /**
     * @returns the colour that is used to display the probe in the oscilloscope
     */
    QColor color() const
    {
        return m_color;
    }
    // 		/**
    // 		 * Will not record any data when paused
    // 		 */
    // 		void setPaused( bool isPaused) { b_isPaused = isPaused; }
    /**
     * Returns the time (in Simulator time) that this probe was created at,
     * or last reset.
     */
    uint64_t resetTime() const
    {
        return m_resetTime;
    }
    /**
     * Erases all recorded data, and sets m_resetTime to the current
     * simulator time.
     */
    virtual void eraseData() = 0;
    /**
     * Searches for and returns the position of the last DataPoint that was
     * added before or at the given Simulator time. If no DataPoints were
     * were recorded before the given time, then will return the one closest
     * to the given time. Will return 0 if no DataPoints have been recorded
     * yet.
     */
    virtual uint64_t findPos(uint64_t time) const = 0;

Q_SIGNALS:
    /**
     * Emitted when an attribute that affects how the probe is drawn in the
     * oscilloscope is changed.
     */
    void displayAttributeChanged();

protected:
    const int m_id;
    float m_drawPosition;
    uint64_t m_resetTime;
    QColor m_color;
};

/**
@author David Saxton
*/
class LogicProbeData : public ProbeData
{
public:
    LogicProbeData(int id);
    ~LogicProbeData() override
    {
        delete m_data;
    }

    /**
     * Appends the data point to the set of data.
     */
    void addDataPoint(LogicDataPoint data); /* {
       m_data->push_back(data);
   } */                                     // 2016.05.06 - moved to cpp

    void eraseData() override;
    uint64_t findPos(uint64_t time) const override;

    bool isEmpty() const
    {
        return m_data->size() == 0;
    }

protected:
    std::vector<LogicDataPoint> *m_data;
    friend class OscilloscopeView;
};

/**
@author David Saxton
*/
class FloatingProbeData : public ProbeData
{
public:
    enum Scaling { Linear, Logarithmic };

    FloatingProbeData(int id);

    /**
     * Appends the data point to the set of data.
     */
    void addDataPoint(float data); // { m_data->push_back(data); } // 2016.05.06 - moved to cpp
    /**
     * Converts the insert position to a Simulator time.
     */
    uint64_t toTime(uint64_t at) const;
    /**
     * Sets the scaling to use in the oscilloscope display.
     */
    void setScaling(Scaling scaling);
    /**
     * @return the scaling used for the oscilloscope display.
     */
    Scaling scaling() const
    {
        return m_scaling;
    }
    /**
     * Sets the value to use as the upper absolute value in the display.
     */
    void setUpperAbsValue(double upperAbsValue);
    /**
     * @return the upper absolute value to use in the display.
     */
    double upperAbsValue() const
    {
        return m_upperAbsValue;
    }
    /**
     * Sets the value to use as the lower absolute value in the display
     * (this is only used with logarithmic scaling).
     */
    void setLowerAbsValue(double lowerAbsValue);
    /**
     * @return the lower absolute value to use in the display (this is
     * only used with logarithmic scaling).
     */
    double lowerAbsValue() const
    {
        return m_lowerAbsValue;
    }

    void eraseData() override;
    uint64_t findPos(uint64_t time) const override;

    bool isEmpty() const
    {
        return m_data->size() == 0;
    }

protected:
    Scaling m_scaling;
    double m_upperAbsValue;
    double m_lowerAbsValue;
    std::vector<float> *m_data;
    friend class OscilloscopeView;
};

#endif
