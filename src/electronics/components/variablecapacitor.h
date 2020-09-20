/***************************************************************************
 *   Copyright (C) 2006 by William Hillerby                                *
 *   william.hillerby@ntlworld.com                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VARIABLECAPACITOR_H
#define VARIABLECAPACITOR_H

#include "component.h"

class Capacitance;
class QSlider;

/**
@short Variable Capacitor
@author William Hillerby
*/

class VariableCapacitor : public Component
{
public:
    VariableCapacitor(ICNDocument *icnDocument, bool newItem, const QString &id = nullptr);
    ~VariableCapacitor() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

    void sliderValueChanged(const QString &id, int newValue) override;

private:
    void dataChanged() override;
    void drawShape(QPainter &p) override;

    Capacitance *m_pCapacitance;
    QSlider *m_pSlider;

    double m_minCapacitance;
    double m_maxCapacitance;
    double m_currCapacitance, m_tickValue;
};

#endif
