/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef RAM_H
#define RAM_H

#include "component.h"
#include "logic.h"

#include <QBitArray>
//#include <q3ptrvector.h>  // 2018.10.17

/**
@author David Saxton
*/
class RAM : public CallbackClass, public Component
{
public:
    RAM(ICNDocument *icnDocument, bool newItem, const char *id = nullptr);
    ~RAM() override;

    static Item *construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

protected:
    void initPins();
    void dataChanged() override;
public: // internal interfaces
    void inStateChanged(bool newState);
protected:

    QBitArray m_data;
    LogicIn *m_pCS; // Chip select
    LogicIn *m_pOE; // Output enable
    LogicIn *m_pWE; // Write enable

    int m_wordSize;
    int m_addressSize;

    QVector<LogicIn *> m_address;
    QVector<LogicIn *> m_dataIn;
    QVector<LogicOut *> m_dataOut;
};

#endif
