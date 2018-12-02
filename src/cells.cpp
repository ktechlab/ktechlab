/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cells.h"
#include "utils.h"


//BEGIN class Cells
Cells::Cells( const QRect & canvasRect )
{
	init( canvasRect );
}


Cells::~Cells()
{
	unsigned w = unsigned(m_cellsRect.width());
	for ( uint i=0; i<w; i++ )
		delete [] m_cells[i];
	delete [] m_cells;
}


Cells::Cells( const Cells & c )
{
	init( QRect( c.cellsRect().topLeft() * 8, c.cellsRect().size() * 8 ) );
	
	unsigned w = unsigned(m_cellsRect.width());
	unsigned h = unsigned(m_cellsRect.height());
	
	for ( uint i=0; i<w; i++ )
	{
		for ( uint j=0; j<h; j++ )
		{
			m_cells[i][j] = c.cell( i, j );
		}
	}
}


void Cells::init( const QRect & canvasRect )
{
	m_cellsRect = QRect( roundDown( canvasRect.topLeft(), 8 ), canvasRect.size()/8 );
	m_cellsRect = m_cellsRect.normalized();
	
	unsigned w = unsigned(m_cellsRect.width());
	unsigned h = unsigned(m_cellsRect.height());
	
	typedef Cell* cellptr;
	m_cells = new cellptr[w];
	for ( uint i=0; i<w; ++i )
	{
		m_cells[i] = new Cell[h];
	}
}


void Cells::reset()
{
	unsigned w = unsigned(m_cellsRect.width());
	unsigned h = unsigned(m_cellsRect.height());
	
	for ( uint i=0; i<w; i++ )
	{
		for ( uint j=0; j<h; j++ )
			m_cells[i][j].reset();
	}
}
//END class Cells



//BEGIN class Point
Point::Point()
{
	x = y = prevX = prevY = startCellPos;
}
//END class Point



//BEGIN class Cell
Cell::Cell()
{
	addedToLabels = false;
	permanent = false;
	CIpenalty = 0;
	numCon = 0;
	Cpenalty = 0;
	bestScore = 0xffff; // Nice large value
}


void Cell::reset()
{
	addedToLabels = false;
	permanent = false;
	bestScore = 0xffff; // Nice large value
}
//END class Cell


