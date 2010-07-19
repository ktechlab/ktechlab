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

#include "ktlautomaticrouterplugin.h"
#include "cells.h"

#include <KGenericFactory>
#include <KAboutData>
#include <KDebug>
#include <QPointF>
#include <cmath>
#include <interfaces/idocumentscene.h>

K_PLUGIN_FACTORY(KTLAutomaticRouterFactory, registerPlugin<AutomaticRouter>(); )
K_EXPORT_PLUGIN(KTLAutomaticRouterFactory(KAboutData("ktlautomatic_router","ktlautomatic_router", ki18n("KTechLab Automatic Router"), "0.1", ki18n("Automatic Routing in Circuit files"), KAboutData::License_LGPL)))

AutomaticRouter::AutomaticRouter(QObject* parent, const QVariantList& args)
    : IPlugin( KTLAutomaticRouterFactory::componentData(), parent)
{
    KDEV_USE_EXTENSION_INTERFACE( KTechLab::IConRouter )
}

void AutomaticRouter::createCells()
{
    if (!m_documentScene)
        return;

    connect(m_documentScene,SIGNAL(sceneRectChanged(const QRectF&)),
            this,SLOT(updateScene(const QRectF&)));


    updateVisualization();
}

void AutomaticRouter::updateVisualization()
{
    QRectF rect = m_documentScene->sceneRect();
    Cells* cells = qobject_cast< Cells* >( m_documentScene->routingInfo().data() );
    if (!cells){
        kWarning() << "Routing information doesn't match";
        return;
    }
    QPainter p;
    p.begin(&m_visualizedData);
    for (int y = 0; y < rect.height(); ++y)
        for (int x = 0; x < rect.width(); ++x){
            p.setPen(cells->colorForScenePoint(QPoint(x,y)));
            p.drawPoint(x,y);
        }
    p.end();
}

void AutomaticRouter::generateRoutingInfo(KTechLab::IDocumentScene* scene)
{
    QRectF rect;
    rect = scene->sceneRect();
    Cells* cells = new Cells(rect.toRect());
    cells->update(scene, rect);
    scene->setRoutingInfo(QSharedPointer<Cells>(cells));
    m_cellsNeedUpdate = false;

    m_visualizedData = QPixmap(rect.size().toSize());
    m_visualizedData.fill(QColor(Qt::transparent));
}

void AutomaticRouter::mapRoute(QPointF p1, QPointF p2)
{
    p1 = p1.toPoint() / 8;
    p2 = p2.toPoint() / 8;
    Cells* cells = qobject_cast< Cells* >( m_documentScene->routingInfo().data() );
    if (!cells){
        kWarning() << "Routing information doesn't match";
        return;
    }

    cells->update(m_documentScene, QRectF(p1,p2));

    if ( !cells->haveCell(p1.x(), p1.y()) || !cells->haveCell(p2.x(), p2.y()) ) {
        return;
    }

    m_route.clear();

    m_lcx = p2.x();
    m_lcy = p2.y();

    // First, lets try some common connector routes (which will not necesssarily
    // be shortest, but they will be neat, and cut down on overall CPU usage)
    // If that fails, we will resort to a shortest-route algorithm to find an
    // appropriate route.

    // Connector configuration: Line
    if (checkLineRoute(p1.x(), p1.y(), p2.x(), p2.y(), 4 * Cells::ScoreConnector)) {
        return;
    } else m_route.clear();

    if (checkLineRoute(p1.x(), p1.y(), p2.x(), p2.y(), 2 * Cells::ScoreConnector)) {
        if (checkLineRoute(p1.x(), p1.y(), p2.x(), p2.y(), Cells::ScoreConnector - 1)) {
            return;
        } else m_route.clear();
    } else m_route.clear();

    // more basic routes??? (TODO)

    // It seems we must resort to brute-force route-checking
    {
        cells->reset();

        // Now to map out the shortest routes to the cells
        Cell *startCell = &cells->cell(p2.x(), p2.y());
        startCell->makePermanent();
        startCell->resetBestScore();
        startCell->setPrevXY(startCellPos, startCellPos);

        m_tempLabels.clear();
        checkCell(p2.x(), p2.y());

        // Daniel: I changed it from a do while to a while otherwise
        // in rare cases the iterator can end up as end().

        while (m_tempLabels.size() > 0 && !cells->cell(p1.x(), p1.y()).isPermanent()) {
            TempLabelMap::iterator it = m_tempLabels.begin();
            checkCell(it->second.x(), it->second.y());
            m_tempLabels.erase(it);
        }

        // Now, retrace the shortest route from the endcell to get out points :)
        int x = p1.x(), y = p1.y();

        bool ok = true;
        do {
            m_route.append(QPointF(x,y)*8);
            int newx = cells->cell(x, y).getPrevX();
            int newy = cells->cell(x, y).getPrevY();

            if (newx == x && newy == y)
                ok = false;
            else {
                x = newx;
                y = newy;
            }
        } while (cells->haveCell(x, y)
        && (x != startCellPos)
        && (y != startCellPos)
        && ok);

        // And append the last point...
        m_route.append(p2*8);
    }

    removeDuplicatePoints();
}

void AutomaticRouter::mapRoute(qreal sx, qreal sy, qreal ex, qreal ey)
{
    mapRoute(QPointF(sx,sy),QPointF(ex,ey));
}

void AutomaticRouter::checkACell(int x, int y, Cell *prev, int prevX, int prevY, int nextScore) {
    //      if ( !p_icnDocument->isValidCellReference(x,y) ) return;
    Cells* cells = qobject_cast< Cells* >( m_documentScene->routingInfo().data() );
    if (!cells) {
        kWarning() << "Routing information doesn't match";
        return;
    }
    if(!cells->haveCell(x, y))
        return;

    Cell *c = &cells->cell(x, y);

    if(c->isPermanent())
        return;

    int newScore = nextScore + c->getCIPenalty(); // + c->Cpenalty;

    // Check for changing direction
    if((x != prevX && prev->comparePrevX(prevX)) ||
        (y != prevY && prev->comparePrevY(prevY))) newScore += 5;

    if(c->scoreIsWorse(newScore))
        return;

    // We only want to change the previous cell if the score is different,
    // or the score is the same but this cell allows the connector
    // to travel in the same direction
    if(c->sameScoreAs(newScore) &&
        x != prevX &&
        y != prevY) return;

    c->setBestScore(newScore);
    c->setPrevXY(prevX, prevY);

    if (!c->getAddedToLabels()) {
        c->setAddedToLabels();
        QPointF point(x,y);
        TempLabelMap::iterator it = m_tempLabels.insert(std::make_pair(newScore, point));
    }
}

void AutomaticRouter::checkCell(int x, int y) {
    Cells* cells = qobject_cast< Cells* >( m_documentScene->routingInfo().data() );
    if (!cells){
        kWarning() << "Routing information doesn't match";
        return;
    }
    Cell *c = &cells->cell(x, y);
    c->makePermanent();

    int nextScore = c->incBestScore();

    // Check the surrounding cells (up, left, right, down)
    checkACell(x - 1, y, c, x, y, nextScore);
    checkACell(x + 1, y, c, x, y, nextScore);
    checkACell(x, y + 1, c, x, y, nextScore);
    checkACell(x, y - 1, c, x, y, nextScore);
}

bool AutomaticRouter::checkLineRoute(int scx, int scy, int ecx, int ecy, int maxCIScore)
{
    //no straight route possible -> return false
    if ((scx != ecx) || (scy != ecy))
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

    Cells* cells = qobject_cast< Cells* >( m_documentScene->routingInfo().data() );
    if (!cells) {
        kError() << "Routing information doesn't match";
    }

    if (isHorizontal) {
        for (qreal x = start; x != end; x += dd) {
            if (std::abs(x - start) > 1 && std::abs(x - end) > 1
                && (cells->cell(x, y).getCIPenalty() > maxCIScore)) {
                return false;
            } else  m_route.append(QPoint(x, y)*8);
        }
    } else {
        for (qreal y = start; y != end; y += dd) {
            if (std::abs(y - start) > 1 && std::abs(y - end) > 1
                && (cells->cell(x, y).getCIPenalty() > maxCIScore)) {
                return false;
            } else m_route.append(QPointF(x, y)*8);
        }
    }

    removeDuplicatePoints();

    return true;
}

void AutomaticRouter::removeDuplicatePoints() {
    const QPointF invalid(-(1 << 30), -(1 << 30));

    QList<QPointF>::const_iterator end = m_route.constEnd();
    for (QList<QPointF>::iterator it = m_route.begin(); it != end; ++it) {
        if(m_route.count(*it) > 1)
            *it = invalid;
    }

    m_route.removeAll(invalid);
}

QPixmap AutomaticRouter::visualizedData(const QRectF& region) const
{
    Cells* cells = qobject_cast< Cells* >( m_documentScene->routingInfo().data() );
    if (!cells)
        return QPixmap();

    QRectF sceneRect = m_documentScene->sceneRect();
    if (!region.intersects(sceneRect))
        return QPixmap();

    QRectF dataRegion = region.intersected(sceneRect);
    QPixmap pic(region.size().toSize());
    pic.fill(Qt::transparent);
    QPainter p(&pic);
    p.drawPixmap(dataRegion, m_visualizedData, dataRegion);
    return pic;
}

void AutomaticRouter::updateScene(const QRectF& rect)
{
    m_cellsNeedUpdate = true;
}

#include "ktlautomaticrouterplugin.moc"
