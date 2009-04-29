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

#include <cassert>
#include <map>
#include <qrect.h>
#include "utils.h"

// what a stupid little class...
class Point {

public:
	Point();
	void setXY(short new_x, short new_y);
	short getX();
	short getY();

private:
	short x;
	short y;
};

// Key = cell, data = previous cell, compare = score
typedef std::multimap< unsigned short, Point > TempLabelMap;

/**
@short Used for mapping out connections
*/
const short startCellPos = -(1 << 14);

class Cell {

public:
	Cell();
	/**
	 * Resets bestScore, prevX, prevY, addedToLabels, it, permanent for
	 * each cell.
	 */
	void reset();

	short getNumCon() const { return numCon; }
	short getPrevX() const { return prevX; }
	short getPrevY() const { return prevY; }

	bool comparePrevX(const short x) const { return prevX == x; }
	bool comparePrevY(const short y) const { return prevY == y; }

	void setPrevXY(const short x, const short y) {
		prevX = x; prevY = y; }

	void addConnectors(const short connectors) { numCon += connectors; }

	bool getAddedToLabels() const { return addedToLabels; }
	void setAddedToLabels() { addedToLabels = true;  }

	bool isPermanent() const { return permanent; }
	void makePermanent() { permanent = true; }

	short getCIPenalty() const { return CIpenalty; }
	void addCIPenalty(short x) { CIpenalty += x; }

	/**
	 * Best (lowest) score so far, _the_ best if it is permanent.
	 */
	unsigned short bestScore;

private:

	/**
	 * 'Penalty' of using the cell from CNItem.
	 */
	unsigned short CIpenalty;

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
	 * Which cell this came from, (startCellPos,startCellPos) if originating
	 * cell.
	 */
	short prevX, prevY;

	/**
	 * Number of connectors through that point.
	 */
	unsigned short numCon;
};

/**
@author David Saxton
*/
class Cells {

public:
	Cells(const QRect &canvasRect);
	~Cells();
	/**
	 * Resets bestScore, prevX, prevY, addedToLabels, it, permanent for each cell
	 */
	void reset();

	QRect cellsRect() const {
		return m_cellsRect;
	}

	/**
	 * Returns the cell containing the given position on the canvas.
	 */
	Cell &cellContaining(int x, int y) const {
		return cell(roundDown(x, 8), roundDown(y, 8));
	}

	/**
	 * @return if the given cell exists.
	 */
	bool haveCell(int i, int j) const {
		if ((i < m_cellsRect.left()) || (i >= m_cellsRect.right()))
			return false;

		if ((j < m_cellsRect.top()) || (j >= m_cellsRect.bottom()))
			return false;

		return true;
	}

	/**
	 * @return if there is a cell containg the given canvas point.
	 */
	bool haveCellContaing(int x, int y) const {
		return haveCell(roundDown(x, 8), roundDown(y, 8));
	}

	Cell &cell(int i, int j) const {
		assert(i < m_cellsRect.right());
		assert(j < m_cellsRect.bottom());
		i -= m_cellsRect.left();
		j -= m_cellsRect.top();
		return m_cells[i][j];
	}

protected:
	void init(const QRect & canvasRect);
	QRect m_cellsRect;
	Cell **m_cells;

private:
	Cells(const Cells &);
	Cells & operator= (const Cells &);
};

#endif

