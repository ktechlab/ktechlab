/***************************************************************************
 *   Copyright (C) 2003-2006 by David Saxton <david@bluehaze.org>          *
 *   Copyright (C) 2010 by Julian Bäume <julian@svg4all.de>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef CELLS_H
#define CELLS_H

#include <cassert>
#include <QRect>
#include <QPixmap>
#include "utils.h"
#include <QColor>
#include <interfaces/iroutinginformation.h>

namespace KTechLab{
    class IDocumentScene;
}

/**
 * @short Used for mapping out connections
 * In this implementation a \class Cell represents 8x8 pixels in the scene to
 * reduce complexity. An instance of this class stores the costs needed to
 * route through this area of the scene.
*/
const short startCellPos = -(1 << 14);

class Cell {

public:
    Cell();
    /**
     * Resets bestScore, prevX, prevY, addedToLabels, it, permanent for
     * the cell.
     */
    void reset();

    short getNumCon() const {
        return numCon;
    }
    void addConnectors(const short connectors) {
        numCon += connectors;
    }

    short getCIPenalty() const {
        return CIpenalty;
    }
    void addCIPenalty(const short x) {
        CIpenalty += x;
    }

    short incBestScore() {
        return ++bestScore;
    };
    bool scoreIsWorse(const short score) const {
        return score > bestScore;
    }
    bool sameScoreAs(const short score) const {
        return score == bestScore;
    }
    void resetBestScore() {
        bestScore = 0xFFFF;
    }
    void setBestScore(const short aScore) {
        bestScore = aScore;
    }

    short getPrevX() const {
        return prevX;
    }
    short getPrevY() const {
        return prevY;
    }
    bool comparePrevX(const short x) const {
        return prevX == x;
    }
    bool comparePrevY(const short y) const {
        return prevY == y;
    }
    void setPrevXY(const short x, const short y) {
        prevX = x;
        prevY = y;
    }

    void setAddedToLabels() {
        addedToLabels = true;
    }
    bool getAddedToLabels() const {
        return addedToLabels;
    }

    bool isPermanent() const {
        return permanent;
    }
    void makePermanent() {
        permanent = true;
    }

private:
    /**
     * Best (lowest) score so far, _the_ best if it is permanent.
     */
    unsigned short bestScore;

    /**
     * 'Penalty' of using the cell from CNItem.
     */
    unsigned short CIpenalty;

    /**
     * Whether the score can be improved on.
     */
    bool permanent;

    /**
     * Whether the cell has already been added to the list of cells to
     * check.
     */
    bool addedToLabels;

    /**
     * Which cell this came from, (startCellPos,startCellPos) if originating
     * cell.
     */
    short prevX, prevY;

    /**
     * Number of connectors through that point.
     */
    unsigned short numCon;
};

/**
 * This class represents a grid (2 dimensional array) of \class Cell instances.
 * In this implementation a \class Cell represents 8x8 pixels in the scene to
 * reduce complexity.
 * @author David Saxton
*/
class Cells : public KTechLab::IRoutingInformation {
    Q_OBJECT
public:
    Cells(const QRect &canvasRect);
    ~Cells();

    /**
     * Scores for different type of items on the scene
     */
    enum {
        ScoreNone = 0,
        ScoreConnector = 4,
        ScoreItem = 1000
    };

    /**
     * Resets bestScore, prevX, prevY, addedToLabels, it, permanent for each cell
     */
    void reset();

    QRect cellsRect() const {
        return m_cellsRect;
    }

    /**
     * Returns the cell containing the given position on the canvas.
     */
    Cell &cellContaining(const int x, const int y) const {
        return cell(fromCanvas(x), fromCanvas(y));
    }

    /**
     * @return if the given cell exists.
     */
    bool haveCell(const int i, const int j) const {
        if ((i < m_cellsRect.left()) || (i >= m_cellsRect.right()))
            return false;

        if ((j < m_cellsRect.top()) || (j >= m_cellsRect.bottom()))
            return false;

        return true;
    }

    /**
     * @return if there is a cell containg the given canvas point.
     */
    bool haveCellContaing(int x, int y) const {
        return haveCell(fromCanvas(x), fromCanvas(y));
    }

    Cell &cell(int i, int j) const {
        assert(i < m_cellsRect.right());
        assert(j < m_cellsRect.bottom());
        i -= m_cellsRect.left();
        j -= m_cellsRect.top();
        return m_cells[i][j];
    }

    QColor colorForScenePoint( QPointF p ) const {
        if (!haveCellContaing(p.x(),p.y()))
            return QColor(Qt::transparent);

        int penalty = cellContaining(p.x(),p.y()).getCIPenalty();
        QColor c(Qt::red);
        c.setAlpha(qMin(penalty,200));
        return c;
    }
    /**
     * Update the internal representation of the scene.
     * If a region is provided, only the items contained in that region
     * are considered in the update.
     *
     * @param scene - the documents scene containing all components
     * @param region - the region that is to be considered
     */
    void update(const KTechLab::IDocumentScene* scene, const QRectF &region = QRectF());

    const QPixmap& visualizedData() const;
    void updateVisualization();

protected:
    void init(const QRect &canvasRect);
    QRect m_cellsRect;
    Cell **m_cells;

private:
    QPixmap m_visualizedData;
    QRect m_sceneRect;
    Cells(const Cells &);
//      Cells & operator= (const Cells &);
};

#endif

