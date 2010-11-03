/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton <david@bluehaze.org>          *
 *   Copyright (C) 2010 by Julian Bäume <julian@svg4all.de>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cells.h"
#include "utils.h"
#include <cmath>
#include <interfaces/idocumentscene.h>
#include <interfaces/component/icomponentitem.h>
#include <interfaces/component/connectoritem.h>
#include <QGraphicsSvgItem>
#include <QPainter>
#include <KDebug>

//BEGIN class Cells

Cells::Cells(KTechLab::IDocumentScene* scene, QObject* parent)
    : KTechLab::IRoutingInformation(scene,parent)
{
    init(scene->sceneRect().toRect());
    update();
}

Cells::~Cells() {
    unsigned w = unsigned(m_cellsRect.width());

    for (uint i = 0; i < w; i++)
        delete [] m_cells[i];

    delete [] m_cells;
}

Cells::Cells(const Cells &c) {
    init(c.m_sceneRect);

    unsigned w = unsigned(m_cellsRect.width());
    unsigned h = unsigned(m_cellsRect.height());

    for (uint i = 0; i < w; i++) {
        for (uint j = 0; j < h; j++) {
            m_cells[i][j] = c.cell(i, j);
        }
    }
}

void Cells::init(const QRect &canvasRect) {
    m_sceneRect = canvasRect;
    m_visualizedData = QImage(m_sceneRect.size() + QSize(4,4),QImage::Format_ARGB32);
    m_visualizedData.fill(QColor(Qt::transparent).rgba());
    m_cellsRect = QRect(mapToCells(m_sceneRect.topLeft()), mapToCells(m_sceneRect.size()));
    m_cellsRect.adjust(-1,-1,2,2);

    unsigned w = unsigned(m_cellsRect.width());
    unsigned h = unsigned(m_cellsRect.height());

    typedef Cell *cellptr;
    m_cells = new cellptr[w];

    for (uint i = 0; i < w; ++i) {
        m_cells[i] = new Cell[h];
    }
}

void Cells::reset() {
    unsigned w = unsigned(m_cellsRect.width());
    unsigned h = unsigned(m_cellsRect.height());

    for (uint i = 0; i < w; i++) {
        for (uint j = 0; j < h; j++)
            m_cells[i][j].reset();
    }
}

QPointF Cells::alignToGrid(const QPointF& point)
{
    QPoint result(mapToCells(point.toPoint()));
    return mapFromCells(result);
}

void Cells::update()
{
    QRectF updateRegion = m_sceneRect;

    foreach (QGraphicsItem* item, m_documentScene->items(updateRegion)) {
        //TODO: remove this, once updateSceneRect got fixed
        //work around the fact, that items get added twice, when update is
        //called and we are moving items around, because they are "removed"
        //when moving starts and added again after being dropped
        if (m_documentScene->movingItems().contains(item)) {
            kDebug() << "not adding moving item: " << item;
            continue;
        }
        KTechLab::IComponentItem* component = 0;
        KTechLab::ConnectorItem* connector = 0;
        int score = Cells::ScoreNone;
        if ((component = qgraphicsitem_cast<KTechLab::IComponentItem*>(item))) {
            score = Cells::ScoreItem;
            addCIPenalty(component, score);
        } else if ((connector = qgraphicsitem_cast<KTechLab::ConnectorItem*>(item))) {
            score = Cells::ScoreConnector;
            addCIPenalty(connector->path(),score);
        }
    }
    updateVisualization();
}

void Cells::updateVisualization(const QRectF &region)
{
    QRect dataRegion(region.toRect());
    if (region.isNull())
        dataRegion = m_sceneRect;

    dataRegion &= m_sceneRect;

    QImage& i = m_visualizedData;
    for (int y = dataRegion.y(); y < dataRegion.y()+dataRegion.height(); ++y)
        for (int x = dataRegion.x(); x < dataRegion.x()+dataRegion.width(); ++x) {
            QPoint poi = QPoint(x,y);
            QColor c = colorForScenePoint(poi);
            i.setPixel(poi-m_sceneRect.topLeft(),c.rgba());
        }
}

void Cells::updateSceneRect(const QRectF& rect)
{
    if (m_documentScene->isRouting() || m_sceneRect.contains(rect.toRect()))
        return;

    //FIXME: this doesn't scale, takes very long for larger scenes
    unsigned w = unsigned(m_cellsRect.width());
    for (uint i = 0; i < w; i++)
        delete [] m_cells[i];
    delete [] m_cells;

    QRect canvasRect(rect.toRect());
    if (canvasRect.isEmpty())
        canvasRect = m_documentScene->sceneRect().toRect();
    init(canvasRect);
    update();
}

Cell& Cells::cell(int i, int j) const {
    Q_ASSERT(i < m_cellsRect.x()+m_cellsRect.width());
    Q_ASSERT(j < m_cellsRect.y()+m_cellsRect.height());
    i -= m_cellsRect.left();
    j -= m_cellsRect.top();
    return m_cells[i][j];
}
bool Cells::haveCell(const int i, const int j) const {
    if ((i < m_cellsRect.left()) || (i >= m_cellsRect.x()+m_cellsRect.width()))
        return false;

    if ((j < m_cellsRect.top()) || (j >= m_cellsRect.y()+m_cellsRect.height()))
        return false;

    return true;
}
bool Cells::haveCellContaing(int x, int y) const {
    QPoint p(mapToCells(QPoint(x,y)));
    return haveCell(p.x(), p.y());
}
Cell& Cells::cellContaining(const int x, const int y) const {
    QPoint p(mapToCells(QPoint(x,y)));
    return cell(p.x(), p.y());
}
QRect Cells::cellsRect() const {
    return m_cellsRect;
}
QColor Cells::colorForScenePoint(QPointF p) const {
    if (!haveCellContaing(p.x(),p.y()))
        return QColor(Qt::transparent);

    int penalty = cellContaining(p.x(),p.y()).getCIPenalty();
    QColor c;
    if(penalty < 0){
        c.setBlue(255);
        penalty *= -1;
    } else {
        c.setRed(255);
    }
    c.setAlpha(qMax(-200,qMin(penalty,200)));
    return c;
}

void Cells::paintRaster(QPainter* p, const QRectF& region) const
{
    //region is not aligned to the grid, so we need to adjust
    int offX = (int)region.x() % 8 + 4;
    int offY = (int)region.y() % 8 + 4;
    for (int y = 0; y < region.height()+12; y+=8)
        for (int x = 0; x < region.width()+12; x+=8)
            p->drawPoint(QPoint(x-offX,y-offY));
}

void Cells::paintRoutingInfo(QPainter* p, const QRectF& target, const QRectF& source) const
{
    p->drawImage(target, m_visualizedData, source);
}

void Cells::mapRoute(QPointF p1, QPointF p2)
{
    p1 = mapToCells(p1).toPoint();
    p2 = mapToCells(p2).toPoint();

    m_route.clear();

    if ( !haveCell(p1.x(), p1.y()) || !haveCell(p2.x(), p2.y()) ) {
        return;
    }

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
        reset();

        // Now to map out the shortest routes to the cells
        Cell *startCell = &cell(p2.x(), p2.y());
        startCell->makePermanent();
        startCell->resetBestScore();
        startCell->setPrevXY(startCellPos, startCellPos);

        m_tempLabels.clear();
        checkCell(p2.x(), p2.y());

        // Daniel: I changed it from a do while to a while otherwise
        // in rare cases the iterator can end up as end().

        while (m_tempLabels.size() > 0 && !cell(p1.x(), p1.y()).isPermanent()) {
            TempLabelMap::iterator it = m_tempLabels.begin();
            checkCell(it->second.x(), it->second.y());
            m_tempLabels.erase(it);
        }

        // Now, retrace the shortest route from the endcell to get out points :)
        int x = p1.x(), y = p1.y();

        bool ok = true;
        do {
            m_route.append(mapFromCells(QPointF(x,y)));
            int newx = cell(x, y).getPrevX();
            int newy = cell(x, y).getPrevY();

            if (newx == x && newy == y)
                ok = false;
            else {
                x = newx;
                y = newy;
            }
        } while (haveCell(x, y)
                 && (x != startCellPos)
                 && (y != startCellPos)
                 && ok);

        // And append the last point...
        m_route.append(mapFromCells(p2));
    }

    removeDuplicatePoints();
}

void Cells::mapRoute(qreal sx, qreal sy, qreal ex, qreal ey)
{
    mapRoute(QPointF(sx,sy),QPointF(ex,ey));
}

void Cells::checkACell(int x, int y, Cell *prev, int prevX, int prevY, int nextScore) {
    //      if ( !p_icnDocument->isValidCellReference(x,y) ) return;
    if (!haveCell(x, y))
        return;

    Cell *c = &cell(x, y);

    if (c->isPermanent())
        return;

    int newScore = nextScore + c->getCIPenalty(); // + c->Cpenalty;

    // Check for changing direction
    if ((x != prevX && prev->comparePrevX(prevX)) ||
        (y != prevY && prev->comparePrevY(prevY))) newScore += 5;

    if (c->scoreIsWorse(newScore))
        return;

    // We only want to change the previous cell if the score is different,
    // or the score is the same but this cell allows the connector
    // to travel in the same direction
    if (c->sameScoreAs(newScore) &&
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

void Cells::checkCell(int x, int y) {
    Cell *c = &cell(x, y);
    c->makePermanent();

    int nextScore = c->incBestScore();

    // Check the surrounding cells (up, left, right, down)
    checkACell(x - 1, y, c, x, y, nextScore);
    checkACell(x + 1, y, c, x, y, nextScore);
    checkACell(x, y + 1, c, x, y, nextScore);
    checkACell(x, y - 1, c, x, y, nextScore);
}

bool Cells::checkLineRoute(int scx, int scy, int ecx, int ecy, int maxCIScore)
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

    if (isHorizontal) {
        for (qreal x = start; x != end; x += dd) {
            if (std::abs(x - start) > 1 && std::abs(x - end) > 1
                    && (cell(x, y).getCIPenalty() > maxCIScore)) {
                return false;
            } else  m_route.append(mapFromCells(QPoint(x, y)));
        }
    } else {
        for (qreal y = start; y != end; y += dd) {
            if (std::abs(y - start) > 1 && std::abs(y - end) > 1
                    && (cell(x, y).getCIPenalty() > maxCIScore)) {
                return false;
            } else m_route.append(mapFromCells(QPointF(x, y)));
        }
    }

    removeDuplicatePoints();

    return true;
}

void Cells::removeDuplicatePoints() {
    const QPointF invalid(-(1 << 30), -(1 << 30));

    QList<QPointF>::const_iterator end = m_route.constEnd();
    for (QList<QPointF>::iterator it = m_route.begin(); it != end; ++it) {
        if (m_route.count(*it) > 1)
            *it = invalid;
    }

    m_route.removeAll(invalid);
}

inline void Cells::addCIPenalty(const QPainterPath& path, int score)
{
    for (int i=0; i < path.elementCount(); ++i) {
        Q_ASSERT((path.elementAt(i).isLineTo() || i==0) && (path.elementAt(i).isMoveTo() || i!=0));
        QPointF p(path.elementAt(i).x,path.elementAt(i).y);
        cellContaining(p.x(),p.y()).addCIPenalty(score);
    }
    updateVisualization(path.boundingRect());
}

inline void Cells::addCIPenalty(const KTechLab::IComponentItem* item, int score)
{
    // score < 0 -> we actually don't add, but subtract the score
    int factor = score < 0 ? -1 : 1;
    QPainterPath shape = item->mapToScene(item->shape());
    QRect rect = shape.boundingRect().toRect();
    if (!m_sceneRect.contains(rect))
        updateSceneRect(m_sceneRect | rect);
    //transform to internal raster
    rect.setSize(mapToCells(rect.size())-QSize(1,1));
    rect.moveTopLeft(mapToCells(rect.topLeft())+QPoint(1,1));

    //blur the surroundings of an item
    for (int x = rect.x()-1; x < rect.x()+rect.width()+1; ++x) {
        //update above rect
        int y = rect.y()-1;
        cell(x,y).addCIPenalty(5*factor*Cells::ScoreConnector);
        //update below rect
        y += rect.height()+1;
        cell(x,y).addCIPenalty(5*factor*Cells::ScoreConnector);
    }
    for (int y = rect.y(); y < rect.y()+rect.height(); ++y) {
        //update left of rect
        int x = rect.x()-1;
        cell(x,y).addCIPenalty(5*factor*Cells::ScoreConnector);
        //update right of rect
        x += rect.width()+1;
        cell(x,y).addCIPenalty(5*factor*Cells::ScoreConnector);
    }
    //update item-rect
    for (int x = rect.x(); x < rect.x()+rect.width(); ++x)
        for (int y = rect.y(); y < rect.y()+rect.height(); ++y) {
            cell(x,y).addCIPenalty(score);
        }
    updateVisualization(item->mapRectToScene(item->boundingRect()));
}

void Cells::addComponents(QList< KTechLab::IComponentItem* > components)
{
    int score = Cells::ScoreItem;
    foreach(KTechLab::IComponentItem* item, components) {
        addCIPenalty(item,score);
    }
}
void Cells::removeComponents(QList< KTechLab::IComponentItem* > components)
{
    int score = -Cells::ScoreItem;
    foreach(KTechLab::IComponentItem* item, components) {
        addCIPenalty(item,score);
    }
}
void Cells::addConnectors(QList< KTechLab::ConnectorItem* > connectors)
{
    int score = Cells::ScoreConnector;
    foreach(KTechLab::ConnectorItem* connector, connectors) {
        addCIPenalty(connector->path(),score);
    }
}

void Cells::removeConnectors(QList< KTechLab::ConnectorItem* > connectors)
{
    int score = -Cells::ScoreConnector;
    foreach(const KTechLab::ConnectorItem* c,connectors) {
        addCIPenalty(c->path(),score);
    }
}

void Cells::removeGraphicsItem(QGraphicsItem* item)
{
    KTechLab::IComponentItem* component = 0;
    KTechLab::ConnectorItem* connector = 0;
    int score = Cells::ScoreNone;

    if ((component = qgraphicsitem_cast<KTechLab::IComponentItem*>(item))) {
        score = -Cells::ScoreItem;
        addCIPenalty(component, score);
    }
    if ((connector = qgraphicsitem_cast<KTechLab::ConnectorItem*>(item))) {
        score = -Cells::ScoreConnector;
        addCIPenalty(connector->path(),score);
    }
}


//END class Cells

//BEGIN class Cell
Cell::Cell() {
    reset();
    CIpenalty = 0;
    numCon = 0;
}

void Cell::reset() {
    addedToLabels = false;
    permanent = false;
    resetBestScore();
}

//END class Cell

#include "cells.moc"
// kate: indent-mode cstyle; space-indent on; indent-width 0; 
