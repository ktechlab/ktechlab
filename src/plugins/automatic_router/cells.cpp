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
#include <interfaces/idocumentscene.h>
#include <interfaces/component/icomponentitem.h>
#include <interfaces/component/connectoritem.h>
#include <QGraphicsSvgItem>
#include <QPainter>
#include <KDebug>

//BEGIN class Cells
Cells::Cells(const QRect &canvasRect) {
    QRect rect(canvasRect);
    rect.setHeight(rect.height()+64);
    rect.setWidth(rect.width()+64);
    init(rect);
}

Cells::~Cells() {
    unsigned w = unsigned(m_cellsRect.width());

    for (uint i = 0; i < w; i++)
        delete [] m_cells[i];

    delete [] m_cells;
}

Cells::Cells(const Cells &c) {
    init(QRect(c.cellsRect().topLeft() * 8, c.cellsRect().size() * 8));

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
    m_visualizedData = QImage(m_sceneRect.size(),QImage::Format_ARGB32);
    m_visualizedData.fill(QColor(Qt::transparent).rgba());
    m_cellsRect = QRect(fromCanvas(canvasRect.topLeft()), canvasRect.size() / 8 + QSize(1,1));
    m_cellsRect = m_cellsRect.normalized();

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

void Cells::update(const KTechLab::IDocumentScene* scene, const QRectF &region)
{
    QRectF updateRegion = region.normalized();
    if (updateRegion.isNull())
        updateRegion = scene->sceneRect();

    foreach (QGraphicsItem* item, scene->items(updateRegion)) {
        KTechLab::IComponentItem* component = 0;
        KTechLab::ConnectorItem* connector = 0;
        int score = Cells::ScoreNone;
        QPainterPath shape = item->mapToScene(item->shape());
        QRect rect = shape.boundingRect().toRect();
        rect.setSize(rect.size() / 8 + QSize(1,1));
        rect.moveTopLeft(rect.topLeft() / 8 - QPoint(1,1));
        if ((component = dynamic_cast<KTechLab::IComponentItem*>(item))) {
            score = Cells::ScoreItem;
            //blur the surroundings of an item
            for (int x = rect.x()-1; x < rect.x()+rect.width()+1; ++x) {
                //update above rect
                int y = rect.y()-1;
                if (cell(x,y).getCIPenalty() < 5*Cells::ScoreConnector)
                    cell(x,y).addCIPenalty(5*Cells::ScoreConnector);
                //update below rect
                y += rect.height()+1;
                if (cell(x,y).getCIPenalty() < 5*Cells::ScoreConnector)
                    cell(x,y).addCIPenalty(5*Cells::ScoreConnector);
            }
            for (int y = rect.y(); y < rect.y()+rect.height()+1; ++y) {
                int x = rect.x()-1;
                if (cell(x,y).getCIPenalty() < 5*Cells::ScoreConnector)
                    cell(x,y).addCIPenalty(5*Cells::ScoreConnector);
                x += rect.width()+1;
                if (cell(x,y).getCIPenalty() < 5*Cells::ScoreConnector)
                    cell(x,y).addCIPenalty(5*Cells::ScoreConnector);
            }
            for (int x = rect.x(); x < rect.x()+rect.width(); ++x)
                for (int y = rect.y(); y < rect.y()+rect.height(); ++y) {
                    cell(x,y).addCIPenalty(score);
                }
        }
        // doesn't work, yet. needs fixing
        if ((connector = dynamic_cast<KTechLab::ConnectorItem*>(item))) {
            score = Cells::ScoreConnector;
            for (int x = rect.x(); x < rect.x()+rect.width(); ++x)
                for (int y = rect.y(); y < rect.y()+rect.height(); ++y)
                    if (shape.contains(QPoint(x*8,y*8)) && cell(x,y).getCIPenalty() < score)
                        cell(x,y).addCIPenalty(score);
        }
    }
    updateVisualization();
    m_needUpdate = false;
}
bool Cells::updateNeeded()
{
    return m_needUpdate;
}

void Cells::updateVisualization(const QRectF &region)
{
    QRect dataRegion(region.toRect());
    if (region.isNull())
        dataRegion = m_sceneRect;

    QImage& i = m_visualizedData;
    for (int y = dataRegion.y(); y < dataRegion.y()+dataRegion.height(); ++y)
        for (int x = dataRegion.x(); x < dataRegion.x()+dataRegion.width(); ++x) {
            QPoint poi(QPoint(x,y));
            QColor c = colorForScenePoint(poi);
            i.setPixel(poi-dataRegion.topLeft(),c.rgba());
        }
}
const QImage& Cells::visualizedData() const
{
    return m_visualizedData;
}

void Cells::updateSceneRect(const QRectF& rect)
{
    unsigned w = unsigned(m_cellsRect.width());
    for (uint i = 0; i < w; i++)
        delete [] m_cells[i];
    delete [] m_cells;

    QRect canvasRect(rect.toRect());
    canvasRect.setSize(canvasRect.size()+QSize(64,64));
    init(canvasRect);

    m_needUpdate = true;
}

Cell& Cells::cell(int i, int j) const {
    assert(i < m_cellsRect.right());
    assert(j < m_cellsRect.bottom());
    i -= m_cellsRect.left();
    j -= m_cellsRect.top();
    return m_cells[i][j];
}
bool Cells::haveCell(const int i, const int j) const {
    if ((i < m_cellsRect.left()) || (i >= m_cellsRect.right()))
        return false;

    if ((j < m_cellsRect.top()) || (j >= m_cellsRect.bottom()))
        return false;

    return true;
}
bool Cells::haveCellContaing(int x, int y) const {
    return haveCell(fromCanvas(x), fromCanvas(y));
}
Cell& Cells::cellContaining(const int x, const int y) const {
    return cell(fromCanvas(x), fromCanvas(y));
}
QRect Cells::cellsRect() const {
    return m_cellsRect;
}
QColor Cells::colorForScenePoint(QPointF p) const {
    if (!haveCellContaing(p.x(),p.y()))
        return QColor(Qt::transparent);

    int penalty = cellContaining(p.x(),p.y()).getCIPenalty();
    QColor c(Qt::red);
    c.setAlpha(qMin(penalty,200));
    return c;
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