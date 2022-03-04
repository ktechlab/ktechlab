/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef DISCRETELOGIC_H
#define DISCRETELOGIC_H

#include "component.h"
#include "logic.h"

class Simulator;

/**
@short Boolean NOT
@author David Saxton
*/
class Inverter : public CallbackClass, public Component
{
public:
    Inverter(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~Inverter() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

protected:
    void inStateChanged(bool newState);
    void drawShape(QPainter &p) override;

    LogicIn *m_pIn;
    LogicOut *m_pOut;
};

/**
@short Buffer
@author David Saxton
*/
class Buffer : public CallbackClass, public Component
{
public:
    Buffer(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~Buffer() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

private:
    void inStateChanged(bool newState);
    void drawShape(QPainter &p) override;

    LogicIn *m_pIn;
    LogicOut *m_pOut;
};

/**
@short Boolean logic input
@author David Saxton
*/
class ECLogicInput : public Component
{
public:
    ECLogicInput(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~ECLogicInput() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

    void buttonStateChanged(const QString &id, bool state) override;

private:
    void dataChanged() override;
    void drawShape(QPainter &p) override;

    LogicOut *m_pOut;
    bool b_state;
};

/**
@short Boolean logic output
@author David Saxton
*/
class ECLogicOutput : public CallbackClass, public Component
{
public:
    ECLogicOutput(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~ECLogicOutput() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

public: // internal interface
    void inStateChanged(bool newState);
protected:
    void drawShape(QPainter &p) override;

    unsigned long long m_lastDrawTime;
    unsigned long long m_lastSwitchTime;
    unsigned long long m_highTime;
    bool m_bLastState;

    double m_lastDrawState;
    LogicIn *m_pIn;
    Simulator *m_pSimulator;
};

#endif
