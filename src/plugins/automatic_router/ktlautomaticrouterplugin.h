/*
    Copyright (C) 2003-2004  David Saxton <david@bluehaze.org>
    Copyright (C) 2010 Julian Bäume <julian@svg4all.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#ifndef KTLAUTOMATICROUTERPLUGIN_H
#define KTLAUTOMATICROUTERPLUGIN_H

#include <kdevplatform/interfaces/iplugin.h>
#include <interfaces/iconrouter.h>
#include <kdevplatform/interfaces/iextension.h>
#include <QVariantList>
#include <map>

class Cells;
class Cell;

// Key = cell, data = previous cell, compare = score
typedef std::multimap< unsigned short, QPointF > TempLabelMap;

class AutomaticRouter : public KDevelop::IPlugin, public KTechLab::IConRouter
{
    Q_OBJECT
    Q_INTERFACES( KTechLab::IConRouter )
public:
    AutomaticRouter(QObject* parent = 0, const QVariantList& args = QVariantList());

    virtual void mapRoute(QPointF p1, QPointF p2);
    virtual void mapRoute(qreal sx, qreal sy, qreal ex, qreal ey);

private:
    /**
    * Check a line of the ICNDocument cells for a valid route
    */
    bool checkLineRoute(int scx, int scy, int ecx, int ecy, int maxCIScore);

    /**
    * Remove duplicated points from the route
    */
    void removeDuplicatePoints();

    void checkACell(int x, int y, Cell *prev, int prevX, int prevY, int nextScore);
    void checkCell(int x, int y);   // Gets the shortest route from the final cell
    void createCells();

    Cells *m_cells;
    TempLabelMap m_tempLabels;
    qreal m_lcx;
    qreal m_lcy;
};

#endif // KTLAUTOMATICROUTERPLUGIN_H
