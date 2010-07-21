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
    rect.setHeight(rect.height()+32);
    rect.setWidth(rect.width()+32);
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
    m_visualizedData = QPixmap(m_sceneRect.size());
    m_visualizedData.fill(QColor(Qt::transparent));
    m_cellsRect = QRect(fromCanvas(canvasRect.topLeft()), canvasRect.size() / 8);
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
        rect.setSize(rect.size() / 8);
        rect.moveTopLeft(rect.topLeft() / 8);
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
            for (int y = rect.y(); y < rect.y()+rect.height(); ++y) {
                int x = rect.x()-1;
                if (cell(x,y).getCIPenalty() < 5*Cells::ScoreConnector)
                    cell(x,y).addCIPenalty(5*Cells::ScoreConnector);
                x += rect.width()+1;
                if (cell(x,y).getCIPenalty() < 5*Cells::ScoreConnector)
                    cell(x,y).addCIPenalty(5*Cells::ScoreConnector);
            }
        }
        if ((connector = dynamic_cast<KTechLab::ConnectorItem*>(item))) {
            score = Cells::ScoreConnector;
        }
        for (int x = rect.x(); x < rect.x()+rect.width(); ++x)
            for (int y = rect.y(); y < rect.y()+rect.height(); ++y)
                if (cell(x,y).getCIPenalty() < score)
                    cell(x,y).addCIPenalty(score);
    }
    updateVisualization();
}

void Cells::updateVisualization()
{
    Cells* cells = this;
    QPainter p;
    p.begin(&m_visualizedData);
    for (int y = 0; y < m_sceneRect.height(); ++y)
        for (int x = 0; x < m_sceneRect.width(); ++x) {
            p.setPen(cells->colorForScenePoint(QPoint(x,y)+m_sceneRect.topLeft()));
            p.drawPoint(x,y);
        }
    p.end();
}
const QPixmap& Cells::visualizedData() const
{
    return m_visualizedData;
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