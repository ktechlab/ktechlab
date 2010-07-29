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
#include "idocumentscene.h"

using namespace KTechLab;

IConRouter::IConRouter()
    : m_visualize(VisualizeRaster)
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

void IConRouter::setDocumentScene(IDocumentScene* scene)
{
    m_documentScene = scene;
    if (scene->routingInfo().isNull()){
        generateRoutingInfo( scene );
    }
}

QPixmap KTechLab::IConRouter::visualizedData(const QRectF &region) const
{
    if (!m_visualize)
        return QPixmap();

    QRectF sceneRect = m_documentScene->sceneRect();

    QRectF dataRegion = region.intersected(sceneRect);
    QRectF targetRegion(dataRegion);

    dataRegion.moveLeft(dataRegion.left()-sceneRect.left());
    dataRegion.moveTop(dataRegion.top()-sceneRect.top());
    targetRegion.moveLeft(targetRegion.left()-region.left());
    targetRegion.moveTop(targetRegion.top()-region.top());

    QPixmap pic(region.size().toSize());
    pic.fill(Qt::transparent);
    QPainter p(&pic);
    if (m_visualize & IConRouter::VisualizeRaster)
        paintRaster(&p, region);
    if (dataRegion.isValid() && m_visualize & IConRouter::VisualizeRoutingInformation)
        paintRoutingInfo(&p, targetRegion, dataRegion);
    return pic;
}

void IConRouter::paintRoutingInfo(QPainter* p, const QRectF& target, const QRectF& source) const
{
    Q_UNUSED(p)
    Q_UNUSED(target)
    Q_UNUSED(source)
}

void IConRouter::paintRaster(QPainter* p, const QRectF& region) const
{
    Q_UNUSED(p)
    Q_UNUSED(region)
}
