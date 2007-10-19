/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CELLS_H
#define CELLS_H

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
typedef std::multimap< unsigned short, Point > TempLabelMap;

// Used for mapping out connections
const int cellSize = 8;
class Cell
{
public:
	Cell();
	/**
	 * Resets bestScore, prevX, prevY, addedToLabels, it, permanent for each cell
	 */
	void reset();
	
	unsigned short CIpenalty; // 'Penalty' of using the cell from CNItem
	unsigned short Cpenalty; // 'Penalty' of using the cell from Connector
	unsigned short bestScore; // Best (lowest) score so far, _the_ best if it is permanent
	short prevX, prevY; // Which cell this came from, (-1,-1) if originating cell
	bool permanent:1; // Whether the score can be improved on
	bool addedToLabels:1; // Whether the cell has already been added to the list of cells to check
	Point *point; // Pointer to the point in the TempLabelMap
	unsigned short numCon; // Number of connectors through that point
};


/**
@author David Saxton
*/
class Cells
{
public:
	Cells( const uint w, const uint h);
	~Cells();
	/**
	 * Resets bestScore, prevX, prevY, addedToLabels, it, permanent for each cell
	 */
	void reset();

	inline Cell* operator[] ( const uint x) const
	{
		if( x<m_w) return m_cells[x];
		return 0;
	}
	
	const uint width() const { return m_w; }
	const uint height() const { return m_h; }
	
	const Cell &cell( const uint x, const uint y) const { return m_cells[x][y]; }
	
private:
	Cells( const Cells &);
	Cells & operator= ( const Cells &);
	void init( const uint w, const uint h);
	
	uint m_w;
	uint m_h;
	
	Cell **m_cells;
};

#endif

