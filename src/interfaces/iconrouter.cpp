/*
    Abstraction for the routing of a connection
    Copyright (C) 2003-2004 by David Saxton <david@bluehaze.org>
    Copyright (C) 2010  Julian Bäume <julian@svg4all.de>

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

#include "iconrouter.h"

#include <QPointF>
#include <QPainterPath>
#include <KDebug>

using namespace KTechLab;

IConRouter::IConRouter()
    : m_documentScene(0)
{}

IConRouter::~IConRouter()
{}


void IConRouter::mapRoute(qreal sx, qreal sy, qreal ex, qreal ey)
{
    mapRoute(QPointF(sx,sy),QPointF(ex,ey));
}

QPainterPath IConRouter::paintedRoute() const
{
    QPainterPath p;
    if (m_route.size() < 2)
        return p;

    QList<QPointF>::const_iterator end = m_route.constEnd();
    QList<QPointF>::const_iterator it=m_route.constBegin();
    p.moveTo(*it);
    for ( ++it; it != end; ++it ){
        p.lineTo(*it);
    }
    return p;
}

QList< QPointF > IConRouter::route() const
{
    return m_route;
}

void IConRouter::setRoute(const QList< QPointF >& route)
{
    if (m_route.size()>0) {
        //this can happen, but in debug-mode, we should inform about it
        kDebug() << "Overwriting non-empty route.";
    }

    m_route = route;
}

void IConRouter::setDocumentScene(const KTechLab::IDocumentScene* scene)
{
    if (!m_documentScene){
        //this can happen, but in debug-mode, we should inform about it
        kDebug() << "Overwriting documentScene";
    }

    m_documentScene = scene;
}

QPixmap KTechLab::IConRouter::visualizedData(const QRectF &region) const
{
    Q_UNUSED(region);
    return QPixmap();
}
