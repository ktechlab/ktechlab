/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H

#include "component.h"

/**
@short Double Pole Double Throw
@author David Saxton
*/
class ECDPDT : public Component
{
public:
    ECDPDT(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~ECDPDT() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

    void buttonStateChanged(const QString &id, bool state) override;
    void dataChanged() override;

private:
    void drawShape(QPainter &p) override;

    Switch *m_switch1;
    Switch *m_switch2;
    Switch *m_switch3;
    Switch *m_switch4;
    bool pressed;
};

/**
@short Double Pole Single Throw
@author David Saxton
*/
class ECDPST : public Component
{
public:
    ECDPST(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~ECDPST() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

    void buttonStateChanged(const QString &id, bool state) override;
    void dataChanged() override;

private:
    void drawShape(QPainter &p) override;

    Switch *m_switch1;
    Switch *m_switch2;
    bool pressed;
};

/**
@short Single Pole Double Throw
@author David Saxton
*/
class ECSPDT : public Component
{
public:
    ECSPDT(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~ECSPDT() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

    void buttonStateChanged(const QString &id, bool state) override;
    void dataChanged() override;

private:
    void drawShape(QPainter &p) override;

    Switch *m_switch1;
    Switch *m_switch2;
    bool pressed;
};

/**
@short Single-Pole Single-Throw Switch
@author David Saxton
*/
class ECSPST : public Component
{
public:
    ECSPST(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~ECSPST() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

    void buttonStateChanged(const QString &id, bool state) override;
    void dataChanged() override;

private:
    void drawShape(QPainter &p) override;
    Switch *m_switch;
    bool pressed;
};

#endif
