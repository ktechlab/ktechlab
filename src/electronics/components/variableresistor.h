/***************************************************************************
 *   Copyright (C) 2006 by William Hillerby - william.hillerby@ntlworld.com*
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef VARIABLERESISTOR_H
#define VARIABLERESISTOR_H

#include <component.h>

class Resistance;
class QSlider;

/**
@author William Hillerby
*/

class VariableResistor : public Component
{
public:
    VariableResistor(ICNDocument *icnDocument, bool newItem, const QString &id = nullptr);
    ~VariableResistor() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

    void sliderValueChanged(const QString &id, int newValue) override;

private:
    void dataChanged() override;
    void drawShape(QPainter &p) override;

    Resistance *m_pResistance;
    QSlider *m_pSlider;
    double m_minResistance;
    double m_maxResistance;
    double m_currResistance;
    double m_tickValue;
};

#endif
