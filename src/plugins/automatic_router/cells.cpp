/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton <david@bluehaze.org>          *
 *   Copyright (C) 2010 by Julian Bäume <julian@svg4all.de>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cells.h"
#include "utils.h"

//BEGIN class Cells
Cells::Cells(const QRect &canvasRect) {
    QRect rect(canvasRect);
    rect.setHeight(rect.height()+32);
    rect.setWidth(rect.width()+32);
    init(rect);
}

Cells::~Cells() {
    unsigned w = unsigned(m_cellsRect.width());

    for (uint i = 0; i < w; i++)
        delete [] m_cells[i];

    delete [] m_cells;
}

Cells::Cells(const Cells &c) {
    init(QRect(c.cellsRect().topLeft() * 8, c.cellsRect().size() * 8));

    unsigned w = unsigned(m_cellsRect.width());
    unsigned h = unsigned(m_cellsRect.height());

    for (uint i = 0; i < w; i++) {
        for (uint j = 0; j < h; j++) {
            m_cells[i][j] = c.cell(i, j);
        }
    }
}

void Cells::init(const QRect &canvasRect) {
    m_cellsRect = QRect(fromCanvas(canvasRect.topLeft()), canvasRect.size() / 8);
    m_cellsRect = m_cellsRect.normalized();

    unsigned w = unsigned(m_cellsRect.width());
    unsigned h = unsigned(m_cellsRect.height());

    typedef Cell *cellptr;
    m_cells = new cellptr[w];

    for (uint i = 0; i < w; ++i) {
        m_cells[i] = new Cell[h];
    }
}

void Cells::reset() {
    unsigned w = unsigned(m_cellsRect.width());
    unsigned h = unsigned(m_cellsRect.height());

    for (uint i = 0; i < w; i++) {
        for (uint j = 0; j < h; j++)
            m_cells[i][j].reset();
    }
}
//END class Cells

//BEGIN class Cell
Cell::Cell() {
    reset();
    CIpenalty = 0;
    numCon = 0;
}

void Cell::reset() {
    addedToLabels = false;
    permanent = false;
    resetBestScore();
}
//END class Cell
