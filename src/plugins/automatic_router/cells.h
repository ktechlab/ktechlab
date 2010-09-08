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

class QGraphicsItem;
namespace KTechLab{
    class IDocumentScene;
class ConnectorItem;
class IComponentItem;
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

#include <map>

// Key = cell, data = previous cell, compare = score
typedef std::multimap< unsigned short, QPointF > TempLabelMap;

/**
 * This class represents a grid (2 dimensional array) of \class Cell instances.
 * In this implementation a \class Cell represents 8x8 pixels in the scene to
 * reduce complexity.
 * @author David Saxton
*/
class Cells : public KTechLab::IRoutingInformation {
    Q_OBJECT
public:
    Cells(KTechLab::IDocumentScene* scene, QObject* parent = 0);
    ~Cells();

    /**
     * Scores for different type of items on the scene
     */
    enum {
        ScoreNone = 0,
        ScoreConnector = 4,
        ScoreItem = 1000
    };

    virtual QPointF alignToGrid(const QPointF& point);
    virtual void mapRoute(QPointF p1, QPointF p2);
    virtual void mapRoute(qreal sx, qreal sy, qreal ex, qreal ey);

    /**
     * Resets bestScore, prevX, prevY, addedToLabels, it, permanent for each cell
     */
    void reset();

    QRect cellsRect() const ;

    /**
     * Returns the cell containing the given position on the canvas.
     */
    Cell &cellContaining(const int x, const int y) const ;

    /**
     * @return if the given cell exists.
     */
    bool haveCell(const int i, const int j) const ;

    /**
     * @return if there is a cell containg the given canvas point.
     */
    bool haveCellContaing(int x, int y) const ;

    Cell &cell(int i, int j) const ;

    QColor colorForScenePoint( QPointF p ) const ;

    void updateVisualization(const QRectF& region = QRectF());

public slots:
    void updateSceneRect(const QRectF &rect = QRectF());
    void addComponents(QList< KTechLab::IComponentItem* > components);
    void removeComponents(QList< KTechLab::IComponentItem* > components);
    void addConnectors(QList< KTechLab::ConnectorItem* > connectors);
    void removeConnectors(QList< KTechLab::ConnectorItem* > connectors);
    void removeGraphicsItem(QGraphicsItem* item);

protected:
    virtual void paintRaster(QPainter* p, const QRectF& region) const;
    virtual void paintRoutingInfo(QPainter* p, const QRectF& target, const QRectF& source) const;

private:
    /**
     * Update the internal representation
     */
    void update();
    void init(const QRect &canvasRect);
    QRect m_cellsRect;
    Cell **m_cells;
    QImage m_visualizedData;
    QRect m_sceneRect;
    Cells(const Cells &);
//      Cells & operator= (const Cells &);

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
    inline void addCIPenalty(const QPainterPath& path, int score);
    inline void addCIPenalty(const KTechLab::IComponentItem* item, int score);

    TempLabelMap m_tempLabels;
    qreal m_lcx;
    qreal m_lcy;
};

#endif
