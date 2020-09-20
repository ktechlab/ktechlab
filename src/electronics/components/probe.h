/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef PROBE_H
#define PROBE_H

#include <component.h>

class LogicProbeData;
class ProbeData;
class FloatingProbeData;

/**
@author David Saxton
*/
class Probe : public Component
{
public:
    Probe(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~Probe() override;

protected:
    void dataChanged() override;

    ProbeData *p_probeData; // As obtained via registering with the oscilloscope
    QColor m_color;
};

/**
@author David Saxton
 */
class FloatingProbe : public Probe
{
public:
    FloatingProbe(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~FloatingProbe() override;

    bool doesStepNonLogic() const override
    {
        return true;
    }

protected:
    void dataChanged() override;
    void drawShape(QPainter &p) override;

    FloatingProbeData *m_pFloatingProbeData;
};

/**
@author David Saxton
 */
class VoltageProbe : public FloatingProbe
{
public:
    VoltageProbe(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~VoltageProbe() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

    void stepNonLogic() override;

protected:
    Pin *m_pPin1;
    Pin *m_pPin2;
};

/**
@author David Saxton
 */
class CurrentProbe : public FloatingProbe
{
public:
    CurrentProbe(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~CurrentProbe() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

    void stepNonLogic() override;

protected:
    VoltageSource *m_voltageSource;
};

/**
@author David Saxton
 */
class LogicProbe : public CallbackClass, public Probe
{
public:
    LogicProbe(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~LogicProbe() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

    void logicCallback(bool value);

protected:
    void drawShape(QPainter &p) override;

    LogicProbeData *p_logicProbeData;
    LogicIn *m_pIn;
    Simulator *m_pSimulator;
};

#endif
