/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef MATRIXDISPLAYDRIVER_H
#define MATRIXDISPLAYDRIVER_H

#include <vector>
#include "dipcomponent.h"
#include "logic.h"

/**
@author David Saxton
 */
class MatrixDisplayDriver : public DIPComponent {

public:
    MatrixDisplayDriver(ICNDocument *icnDocument, bool newItem, const char *id = 0);
    ~MatrixDisplayDriver();

    static Item* construct(ItemDocument *itemDocument, bool newItem, const char *id);
    static LibraryItem *libraryItem();

    virtual void stepNonLogic();
    virtual bool doesStepNonLogic() const { return true; }

protected:
    std::vector<LogicIn> m_pValueLogic;
    std::vector<LogicOut> m_pRowLogic;
    std::vector<LogicOut> m_pColLogic;

    unsigned m_Col;
};

#endif
