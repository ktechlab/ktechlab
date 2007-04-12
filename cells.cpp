/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cells.h"

#if 0
class CellSmall
{
	public:
		/**
	 * Resets bestScore, prevX, prevY, addedToLabels, it, permanent for each cell
		 */
		void reset();
	
// 		Point *point; // Pointer to the point in the TempLabelMap
		short prevX, prevY; // Which cell this came from, (-1,-1) if originating cell
		unsigned short CIpenalty; // 'Penalty' of using the cell from CNItem
		unsigned short Cpenalty; // 'Penalty' of using the cell from Connector
		unsigned short bestScore; // Best (lowest) score so far, _the_ best if it is permanent
		unsigned char numCon; // Number of connectors through that point
		bool permanent:1; // Whether the score can be improved on
		bool addedToLabels:1; // Whether the cell has already been added to the list of cells to check
};

class CellBig
{
	public:
		/**
	 * Resets bestScore, prevX, prevY, addedToLabels, it, permanent for each cell
		 */
		void reset();
	
		Point *point; // Pointer to the point in the TempLabelMap
		short prevX, prevY; // Which cell this came from, (-1,-1) if originating cell
		unsigned short CIpenalty; // 'Penalty' of using the cell from CNItem
		unsigned short Cpenalty; // 'Penalty' of using the cell from Connector
		unsigned short bestScore; // Best (lowest) score so far, _the_ best if it is permanent
		unsigned char numCon; // Number of connectors through that point
		bool permanent:1; // Whether the score can be improved on
		bool addedToLabels:1; // Whether the cell has already been added to the list of cells to check
};
#endif


Cells::Cells( const uint w, const uint h )
{
#if 0
	kdDebug() << "sizeof(CellSmall)="<<sizeof(CellSmall)<<endl;
	kdDebug() << "sizeof(CellBig)="<<sizeof(Cell)<<endl;
	kdDebug() << "sizeof(unsigned short)="<<sizeof(unsigned short)<<endl;
	kdDebug() << "sizeof(short)="<<sizeof(short)<<endl;
	kdDebug() << "sizeof(Point*)="<<sizeof(Point*)<<endl;
	kdDebug() << "sizeof(bool)="<<sizeof(bool)<<endl;
	kdDebug() << "sizeof(char)="<<sizeof(char)<<endl;
#endif
	init( w, h );
}


Cells::~Cells()
{
	for ( uint i=0; i<m_w; ++i ) {
		delete [] m_cells[i];
	}
	delete [] m_cells;
}

Cells::Cells( const Cells &c )
{
	init( c.width(), c.height() );
	for ( uint x=0; x<m_w; x++ )
	{
		for ( uint y=0; y<m_h; y++ )
		{
			m_cells[x][y] = c.cell( x, y );
		}
	}
}


void Cells::init( const uint w, const uint h )
{
	m_w = w;
	m_h = h;
	
	typedef Cell* cellptr;
	m_cells = new cellptr[m_w];
	for ( uint i=0; i<m_w; ++i )
	{
		m_cells[i] = new Cell[m_h];
	}
}


void Cells::reset()
{
	for ( uint x=0; x<m_w; x++ )
	{
		for ( uint y=0; y<m_h; y++ )
		{
			m_cells[x][y].reset();
		}
	}
}


Point::Point()
{
	x = y = prevX = prevY = -1;
}

Cell::Cell()
{
	addedToLabels = false;
	permanent = false;
	CIpenalty = 0;
	numCon = 0;
	Cpenalty = 0;
	bestScore = (int)1e9; // Nice large value
}

void Cell::reset()
{
	addedToLabels = false;
	permanent = false;
	bestScore = (int)1e9; // Nice large value
}



