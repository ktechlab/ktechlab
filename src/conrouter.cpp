/***************************************************************************
 *   Copyright (C) 2003-2004 by David Saxton                               *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "conrouter.h"
#include "icndocument.h"
#include "utils.h"
#include <QDebug>

#include <cassert>
#include <cmath>
#include <cstdlib>

ConRouter::ConRouter(ICNDocument *cv)
{
    p_icnDocument = cv;
    m_lcx = m_lcy = 0;
}

ConRouter::~ConRouter()
{
}

QPointList ConRouter::pointList(bool reverse) const
{
    QPointList pointList;

    if (reverse) {
        bool notDone = m_cellPointList.size() > 0;
        for (QPointList::const_iterator it = (--m_cellPointList.constEnd()); notDone; --it) {
            pointList.append(toCanvas(&*it));
            if (it == m_cellPointList.begin())
                notDone = false;
        }
    } else {
        const QPointList::const_iterator end = m_cellPointList.end();
        for (QPointList::const_iterator it = m_cellPointList.begin(); it != end; ++it) {
            pointList.append(toCanvas(&*it));
        }
    }

    return pointList;
}

QPointListList ConRouter::splitPoints(const QPoint &pos) const
{
    const QPoint split = fromCanvas(&pos);

    QList<QPointList> list;

    // Check that the point is in the connector points, and not at the start or end
    bool found = false;
    QPointList::const_iterator end = m_cellPointList.end();

    double dl[] = {0.0, 1.1, 1.5}; // sqrt(2) < 1.5 < sqrt(5)
    for (unsigned i = 0; (i < 3) && !found; ++i) {
        for (QPointList::const_iterator it = m_cellPointList.begin(); it != end && !found; ++it) {
            QPointList::const_iterator fromLast = --m_cellPointList.constEnd();
            if (qpoint_distance(*it, split) <= dl[i] && it != m_cellPointList.begin() && it != fromLast) // m_cellPointList.fromLast() )
                found = true;
        }
    }

    QPointList first;
    QPointList second;

    if (!found) {
        qWarning() << "ConRouter::splitConnectorPoints: Could not find point (" << pos.x() << ", " << pos.y() << ") in connector points" << endl;
        qWarning() << "ConRouter::splitConnectorPoints: Returning generic list" << endl;

        first.append(toCanvas(m_cellPointList.first()));
        first.append(pos);
        second.append(pos);
        second.append(toCanvas(m_cellPointList.last()));

        list.append(first);
        list.append(second);

        return list;
    }

    // Now add the points to the two lists
    bool gotToSplit = false;
    for (QPointList::const_iterator it = m_cellPointList.begin(); it != end; ++it) {
        QPoint canvasPoint = toCanvas(&*it);
        if (*it == split) {
            gotToSplit = true;
            first.append(canvasPoint);
            second.prepend(canvasPoint);
        } else if (!gotToSplit) {
            first.append(canvasPoint);
        } else /*if (gotToSplit)*/
        {
            second.append(canvasPoint);
        }
    }

    list.append(first);
    list.append(second);

    return list;
}

QPointListList ConRouter::dividePoints(uint n) const
{
    // Divide the points up into n pieces...

    QPointList points = m_cellPointList;
    assert(n != 0);
    if (points.size() == 0) {
        points += QPoint(toCanvas(m_lcx), toCanvas(m_lcy));
    }

    const float avgLength = float(points.size() - 1) / float(n);

    QPointListList pll;
    for (uint i = 0; i < n; ++i) {
        QPointList pl;
        // Get the points between (pos) and (pos+avgLength)
        const int endPos = roundDouble(avgLength * (i + 1));
        const int startPos = roundDouble(avgLength * i);
        // const QPointList::iterator end = ++points.at(endPos);
        // for ( QPointList::iterator it = points.at(startPos); it != end; ++it )
        for (int pos = startPos; pos < endPos; ++pos) {
            // pl += toCanvas(*it);
            pl += toCanvas(points.at(pos));
        }
        pll += pl;
    }
    return pll;
}

void ConRouter::checkACell(int x, int y, Cell *prev, int prevX, int prevY, int nextScore)
{
    // 	if ( !p_icnDocument->isValidCellReference(x,y) ) return;
    if (!cellsPtr->haveCell(x, y))
        return;

    Cell *c = &cellsPtr->cell(x, y);
    if (c->permanent)
        return;

    int newScore = nextScore + c->CIpenalty + c->Cpenalty;

    // Check for changing direction
    if (x != prevX && prev->prevX == prevX)
        newScore += 5;
    else if (y != prevY && prev->prevY == prevY)
        newScore += 5;

    if (c->bestScore < newScore)
        return;

    // We only want to change the previous cell if the score is different,
    // or the score is the same but this cell allows the connector
    // to travel in the same direction

    if (c->bestScore == newScore && x != prevX && y != prevY)
        return;

    c->bestScore = newScore;
    c->prevX = prevX;
    c->prevY = prevY;

    if (!c->addedToLabels) {
        c->addedToLabels = true;
        Point point;
        point.x = x;
        point.y = y;
        point.prevX = prevX;
        point.prevY = prevY;
        TempLabelMap::iterator it = tempLabels.insert(std::make_pair(newScore, point));
        c->point = &it->second;
    } else {
        c->point->prevX = prevX;
        c->point->prevY = prevY;
    }
}

void ConRouter::checkCell(int x, int y)
{
    Cell *c = &cellsPtr->cell(x, y);

    c->permanent = true;
    int nextScore = c->bestScore + 1;

    // Check the surrounding cells (up, left, right, down)
    checkACell(x, y - 1, c, x, y, nextScore);
    checkACell(x - 1, y, c, x, y, nextScore);
    checkACell(x + 1, y, c, x, y, nextScore);
    checkACell(x, y + 1, c, x, y, nextScore);
}

bool ConRouter::needsRouting(int sx, int sy, int ex, int ey) const
{
    if (m_cellPointList.size() < 2) {
        // Better be on the safe side...
        return true;
    }

    const int scx = fromCanvas(sx);
    const int scy = fromCanvas(sy);
    const int ecx = fromCanvas(ex);
    const int ecy = fromCanvas(ey);

    const int psx = m_cellPointList.first().x();
    const int psy = m_cellPointList.first().y();
    const int pex = m_cellPointList.last().x();
    const int pey = m_cellPointList.last().y();

    return (psx != scx || psy != scy || pex != ecx || pey != ecy) && (pex != scx || pey != scy || psx != ecx || psy != ecy);
}

void ConRouter::setRoutePoints(const QPointList &pointList)
{
    m_cellPointList = pointList;
    removeDuplicatePoints();
}

void ConRouter::setPoints(const QPointList &pointList, bool reverse)
{
    if (pointList.size() == 0)
        return;

    QPointList cellPointList;

    QPoint prevCellPoint = fromCanvas(*pointList.begin());
    cellPointList.append(prevCellPoint);
    const QPointList::const_iterator end = pointList.end();
    for (QPointList::const_iterator it = pointList.begin(); it != end; ++it) {
        QPoint cellPoint = fromCanvas(*it);

        while (prevCellPoint != cellPoint) {
            cellPointList.append(prevCellPoint);

            if (prevCellPoint.x() < cellPoint.x())
                prevCellPoint.setX(prevCellPoint.x() + 1);
            else if (prevCellPoint.x() > cellPoint.x())
                prevCellPoint.setX(prevCellPoint.x() - 1);
            if (prevCellPoint.y() < cellPoint.y())
                prevCellPoint.setY(prevCellPoint.y() + 1);
            else if (prevCellPoint.y() > cellPoint.y())
                prevCellPoint.setY(prevCellPoint.y() - 1);
        };

        prevCellPoint = cellPoint;
    }
    cellPointList.append(prevCellPoint);

    if (reverse) {
        m_cellPointList.clear();
        const QPointList::iterator begin = cellPointList.begin();
        for (QPointList::iterator it = --cellPointList.end(); it != begin; --it) {
            m_cellPointList += *it;
        }
        m_cellPointList += *begin;
    } else {
        m_cellPointList = cellPointList;
    }

    removeDuplicatePoints();
}

void ConRouter::translateRoute(int dx, int dy)
{
    if (dx == 0 && dy == 0)
        return;

    m_lcx += dx;
    m_lcy += dy;

    // 	const QPoint ds = QPoint( fromCanvas(dx), fromCanvas(dy) );
    const QPoint ds = QPoint(dx / 8, dy / 8);

    QPointList::iterator end = m_cellPointList.end();
    for (QPointList::iterator it = m_cellPointList.begin(); it != end; ++it) {
        (*it) += ds;
    }

    removeDuplicatePoints();
}

void ConRouter::mapRoute(int sx, int sy, int ex, int ey)
{
    const int scx = fromCanvas(sx);
    const int scy = fromCanvas(sy);
    const int ecx = fromCanvas(ex);
    const int ecy = fromCanvas(ey);

    cellsPtr = p_icnDocument->cells();

    if (!cellsPtr->haveCell(scx, scy) || !cellsPtr->haveCell(ecx, ecy)) {
        qDebug() << Q_FUNC_INFO << "cellPtr doesn't have cells, giving up";
        return;
    }

    m_cellPointList.clear();
    m_lcx = ecx;
    m_lcy = ecy;

    // First, lets try some common connector routes (which will not necesssarily
    // be shortest, but they will be neat, and cut down on overall CPU usage)
    // If that fails, we will resort to a shortest-route algorithm to find an
    // appropriate route.

    // Connector configuration: Line
    {
        bool ok = checkLineRoute(scx, scy, ecx, ecy, 4 * ICNDocument::hs_connector, 0);
        if (ok) {
            return;
        } else {
            m_cellPointList.clear();
        }
    }

    // Corner 1
    {
        bool ok = checkLineRoute(scx, scy, ecx, ecy, 2 * ICNDocument::hs_connector, 0);
        if (!ok) {
            m_cellPointList.clear();
        } else {
            ok = checkLineRoute(scx, scy, ecx, ecy, ICNDocument::hs_connector - 1, 0);
            if (ok) {
                return;
            } else {
                m_cellPointList.clear();
            }
        }
    }

    // Corner 2
    {
        bool ok = checkLineRoute(scx, scy, ecx, ecy, 2 * ICNDocument::hs_connector, 0);
        if (!ok) {
            m_cellPointList.clear();
        } else {
            ok = checkLineRoute(scx, scy, ecx, ecy, ICNDocument::hs_connector - 1, 0);
            if (ok) {
                return;
            } else {
                m_cellPointList.clear();
            }
        }
    }

    // It seems we must resort to brute-force route-checking
    {
        cellsPtr->reset();

        xcells = p_icnDocument->canvas()->width() / 8;
        ycells = p_icnDocument->canvas()->height() / 8;

        // Now to map out the shortest routes to the cells
        Cell *const startCell = &cellsPtr->cell(ecx, ecy);
        startCell->permanent = true;
        startCell->bestScore = 0;
        startCell->prevX = startCellPos;
        startCell->prevY = startCellPos;

        tempLabels.clear();
        checkCell(ecx, ecy);

        // Daniel: I changed it from a do while to a while otherwise
        // in rare cases the iterator can end up as end().
        while (tempLabels.size() > 0 && !cellsPtr->cell(scx, scy).permanent) {
            TempLabelMap::iterator it = tempLabels.begin();
            checkCell(it->second.x, it->second.y);
            tempLabels.erase(it);
        }

        // Now, retrace the shortest route from the endcell to get out points :)
        int x = scx, y = scy;
        bool ok = true;

        do {
            m_cellPointList.append(QPoint(x, y));
            int newx = cellsPtr->cell(x, y).prevX;
            int newy = cellsPtr->cell(x, y).prevY;
            if (newx == x && newy == y) {
                ok = false;
            }
            x = newx;
            y = newy;
        } while (cellsPtr->haveCell(x, y) && (x != startCellPos) && (y != startCellPos) && ok);

        // And append the last point...
        m_cellPointList.append(QPoint(ecx, ecy));
    }

    removeDuplicatePoints();
}

bool ConRouter::checkLineRoute(int scx, int scy, int ecx, int ecy, int maxConScore, int maxCIScore)
{
    if ((scx != ecx) && (scy != ecy))
        return false;

    const bool isHorizontal = scy == ecy;

    int start = 0, end = 0, x = 0, y = 0, dd = 0;
    if (isHorizontal) {
        dd = (scx < ecx) ? 1 : -1;
        start = scx;
        end = ecx + dd;
        y = scy;
    } else {
        dd = (scy < ecy) ? 1 : -1;
        start = scy;
        end = ecy + dd;
        x = scx;
    }

    Cells *cells = p_icnDocument->cells();

    if (isHorizontal) {
        for (int x = start; x != end; x += dd) {
            if (std::abs(x - start) > 1 && std::abs(x - end) > 1 && (cells->cell(x, y).CIpenalty > maxCIScore || cells->cell(x, y).Cpenalty > maxConScore)) {
                return false;
            } else
                m_cellPointList.append(QPoint(x, y));
        }
    } else {
        for (int y = start; y != end; y += dd) {
            if (std::abs(y - start) > 1 && std::abs(y - end) > 1 && (cells->cell(x, y).CIpenalty > maxCIScore || cells->cell(x, y).Cpenalty > maxConScore)) {
                return false;
            } else {
                m_cellPointList.append(QPoint(x, y));
            }
        }
    }

    m_cellPointList.prepend(QPoint(scx, scy));
    m_cellPointList.append(QPoint(ecx, ecy));
    removeDuplicatePoints();
    return true;
}

void ConRouter::removeDuplicatePoints()
{
    QPoint invalid(-(1 << 30), -(1 << 30));
    QPoint prev = invalid;

    const QPointList::iterator end = m_cellPointList.end();
    for (QPointList::iterator it = m_cellPointList.begin(); it != end; ++it) {
        if (*it == prev) {
            *it = invalid;
        } else {
            prev = *it;
        }
    }
    m_cellPointList.removeAll(invalid);
}
