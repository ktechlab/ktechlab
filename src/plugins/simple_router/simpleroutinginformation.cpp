/*
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


#include "simpleroutinginformation.h"

using namespace KTechLab;

KTechLab::SimpleRoutingInformation::SimpleRoutingInformation(IDocumentScene* scene, QObject* parent)
    : IRoutingInformation(scene, parent)
{
}

QPointF KTechLab::SimpleRoutingInformation::alignToGrid(const QPointF& point)
{
    QPoint r = (point - QPointF(4,4)).toPoint();
    r /= 8;
    return (r*8) + QPointF(4,4);
}

void KTechLab::SimpleRoutingInformation::mapRoute(QPointF p1, QPointF p2)
{
    m_route.clear();
    QPointF start = p1;
    QPointF end = p2;

    p2.setX(start.x());
    p1.setX(end.x());
    QPointF mid = ((p1 - start).manhattanLength() < (p2 - start).manhattanLength()) ? p1 : p2;

    m_route.append(start);
    m_route.append(mid);
    m_route.append(end);
}

void KTechLab::SimpleRoutingInformation::paintRaster(QPainter* p, const QRectF& region) const
{
    //region is not aligned to the grid, so we need to adjust
    int offX = (int)region.x() % 8 + 4;
    int offY = (int)region.y() % 8 + 4;
    for (int y = 0; y < region.height()+12; y+=8)
        for (int x = 0; x < region.width()+12; x+=8)
            p->drawPoint(QPoint(x-offX,y-offY));
}
