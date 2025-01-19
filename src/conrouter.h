/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CONROUTER_H
#define CONROUTER_H

#include "cells.h"

#include <QList>
#include <QPoint>

class ICNDocument;
class Cell;

typedef QList<QPoint> QPointList;
typedef QList<QPointList> QPointListList;

/**
Abstraction for the routing of a connector.

NB: As a general rule of thumb, the point references stored as members of this
class are in Cell-space (i.e. 8^2 x smaller than Canvas-space), and the
interfacing functions take or give point references in Canvas-space (unless
otherwise indicated).

@author David Saxton
*/
class ConRouter
{
public:
    ConRouter(ICNDocument *cv);
    ~ConRouter();

    /**
     * What this class is all about - finding a route, from (sx,sy) to (ex,ey).
     */
    void mapRoute(int sx, int sy, int ex, int ey);
    /**
     * Translates the precalculated routepoints by the given amount
     */
    void translateRoute(int dx, int dy);
    /**
     * Sets the route to the given canvas points
     * @param reverse if true, the points in pointList will be reversed
     */
    void setPoints(const QPointList &pointList, bool reverse = false);
    /**
     * Sets the route to the given route points
     */
    void setRoutePoints(const QPointList &pointList);
    /**
     * @returns true if the start or end points differ from that of the current route
     */
    bool needsRouting(int sx, int sy, int ex, int ey) const;
    /**
     * Returns the list of canvas points
     */
    QPointList pointList(bool reverse) const;
    /**
     * Returns a pointer to the internal cellPointList
     */
    QPointList *cellPointList()
    {
        return &m_cellPointList;
    }
    /**
     * This will return two lists of Canvas points from the splitting of the
     * route at the Canvas point "pos". The internal stored points are not
     * affected.
     */
    QPointListList splitPoints(const QPoint &pos) const;
    /**
     * This will return a list of Canvas pointLists from the route, divided
     * into n parts (at n-1 equally spaced places).
     */
    QPointListList dividePoints(uint n) const;

protected:
    /**
     * Check a line of the ICNDocument cells for a valid route
     */
    bool checkLineRoute(int scx, int scy, int ecx, int ecy, int maxConScore, int maxCIScore);
    void checkACell(int x, int y, Cell *prev, int prevX, int prevY, int nextScore);
    void checkCell(int x, int y); // Gets the shortest route from the final cell
    /**
     * Remove duplicated points from the route
     */
    void removeDuplicatePoints();

    int xcells, ycells;
    int m_lcx, m_lcy; // Last x / y from mapRoute, if we need a point on the route
    Cells *cellsPtr;
    TempLabelMap tempLabels;
    ICNDocument *p_icnDocument;
    QPointList m_cellPointList;
};

#endif
