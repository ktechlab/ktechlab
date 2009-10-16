/***************************************************************************
 *   Copyright (C) 2004-2005 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H

#include "dipcomponent.h"
#include "logic.h"

#include <vector>

/**
@author David Saxton
*/

class Multiplexer : public CallbackClass, public DIPComponent {

public:
    Multiplexer(ICNDocument *icnDocument, bool newItem, const char *id = 0L);
    ~Multiplexer();

    static Item* construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

protected:
    void dataChanged();
    /**
     * Add / remove pins according to the number of inputs the user has requested
     */
    void initPins(unsigned addressSize);

    void inStateChanged(bool newState);

    std::vector<LogicIn> m_aLogic;
    std::vector<LogicIn> m_xLogic;
    LogicOut m_output;
};

#endif
