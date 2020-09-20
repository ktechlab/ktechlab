/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CELLS_H
#define CELLS_H

#include "utils.h"
#include <QRect>
#include <cassert>
#include <map>

class Point
{
public:
    Point();

    short x;
    short y;
    short prevX;
    short prevY;
};

// Key = cell, data = previous cell, compare = score
typedef std::multimap<unsigned short, Point> TempLabelMap;

/**
@short Used for mapping out connections
*/
const short startCellPos = -(1 << 14);
class Cell
{
public:
    Cell();
    /**
     * Resets bestScore, prevX, prevY, addedToLabels, it, permanent for
     * each cell.
     */
    void reset();

    /**
     * 'Penalty' of using the cell from CNItem.
     */
    unsigned short CIpenalty;
    /**
     * 'Penalty' of using the cell from Connector.
     */
    unsigned short Cpenalty;
    /**
     * Best (lowest) score so far, _the_ best if it is permanent.
     */
    unsigned short bestScore;
    /**
     * Which cell this came from, (startCellPos,startCellPos) if originating
     * cell.
     */
    short prevX, prevY;
    /**
     * Whether the score can be improved on.
     */
    bool permanent;
    /**
     * Whether the cell has already been added to the list of cells to
     * check.
     */
    bool addedToLabels;
    /**
     * Pointer to the point in the TempLabelMap.
     */
    Point *point;
    /**
     * Number of connectors through that point.
     */
    unsigned short numCon;
};

/**
@author David Saxton
*/
class Cells
{
public:
    Cells(const QRect &canvasRect);
    ~Cells();
    /**
     * Resets bestScore, prevX, prevY, addedToLabels, it, permanent for each cell
     */
    void reset();

    QRect cellsRect() const
    {
        return m_cellsRect;
    }

    /**
     * Returns the cell containing the given position on the canvas.
     */
    Cell &cellContaining(int x, int y) const
    {
        return cell(roundDown(x, 8), roundDown(y, 8));
    }
    /**
     * @return if the given cell exists.
     */
    bool haveCell(int i, int j) const
    {
        if ((i < m_cellsRect.left()) || (i >= m_cellsRect.right()))
            return false;

        if ((j < m_cellsRect.top()) || (j >= m_cellsRect.bottom()))
            return false;

        return true;
    }
    /**
     * @return if there is a cell containg the given canvas point.
     */
    bool haveCellContaing(int x, int y) const
    {
        return haveCell(roundDown(x, 8), roundDown(y, 8));
    }
    Cell &cell(int i, int j) const
    {
        assert(i < m_cellsRect.right());
        assert(j < m_cellsRect.bottom());
        i -= m_cellsRect.left();
        j -= m_cellsRect.top();
        return m_cells[i][j];
    }

protected:
    void init(const QRect &canvasRect);

    QRect m_cellsRect;

    Cell **m_cells;

private:
    Cells(const Cells &);
    Cells &operator=(const Cells &);
};

#endif
