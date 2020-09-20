/***************************************************************************
 *   Copyright (C) 2005 by David Saxton                                    *
 *   david@bluehaze.org                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "resizeoverlay.h"
#include "itemdocument.h"
#include "mechanicsitem.h"

#include <QDebug>
#include <QPainter>
#include <cmath>

#define DPR (180.0 / M_PI)

// BEGIN class ResizeOverlay
ResizeOverlay::ResizeOverlay(Item *parent)
    : QObject(parent)
{
    b_showResizeHandles = false;
    b_visible = true;
    p_item = parent;
}

ResizeOverlay::~ResizeOverlay()
{
    const ResizeHandleMap::iterator end = m_resizeHandleMap.end();
    for (ResizeHandleMap::iterator it = m_resizeHandleMap.begin(); it != end; ++it) {
        if (it.value())
            it.value()->setCanvas(nullptr);
        delete (ResizeHandle *)it.value();
    }
    m_resizeHandleMap.clear();
}

void ResizeOverlay::showResizeHandles(bool show)
{
    b_showResizeHandles = show;
    const ResizeHandleMap::iterator end = m_resizeHandleMap.end();
    for (ResizeHandleMap::iterator it = m_resizeHandleMap.begin(); it != end; ++it) {
        it.value()->setVisible(b_showResizeHandles && b_visible);
    }
}

void ResizeOverlay::setVisible(bool visible)
{
    b_visible = visible;
    const ResizeHandleMap::iterator end = m_resizeHandleMap.end();
    for (ResizeHandleMap::iterator it = m_resizeHandleMap.begin(); it != end; ++it) {
        it.value()->setVisible(b_showResizeHandles && b_visible);
    }
}

ResizeHandle *ResizeOverlay::createResizeHandle(int id, ResizeHandle::DrawType drawType, int xsnap, int ysnap)
{
    ResizeHandleMap::iterator it = m_resizeHandleMap.find(id);
    if (it != m_resizeHandleMap.end())
        return it.value();

    ResizeHandle *newResizeHandle = new ResizeHandle(this, id, drawType, xsnap, ysnap);
    m_resizeHandleMap[id] = newResizeHandle;
    connect(newResizeHandle, SIGNAL(rhMovedBy(int, double, double)), this, SLOT(slotResizeHandleMoved(int, double, double)));
    return newResizeHandle;
}

void ResizeOverlay::removeResizeHandle(int id)
{
    ResizeHandleMap::iterator it = m_resizeHandleMap.find(id);
    if (it == m_resizeHandleMap.end())
        return;

    ResizeHandle *rh = it.value();
    disconnect(rh, SIGNAL(rhMovedBy(int, double, double)), this, SLOT(slotResizeHandleMoved(int, double, double)));
    delete rh;
    m_resizeHandleMap.erase(it);
}

ResizeHandle *ResizeOverlay::resizeHandle(int id)
{
    ResizeHandleMap::iterator it = m_resizeHandleMap.find(id);
    if (it != m_resizeHandleMap.end())
        return it.value();
    return nullptr;
}

void ResizeOverlay::slotMoveAllResizeHandles(double dx, double dy)
{
    const ResizeHandleMap::iterator end = m_resizeHandleMap.end();
    for (ResizeHandleMap::iterator it = m_resizeHandleMap.begin(); it != end; ++it) {
        it.value()->moveBy(dx, dy);
    }
}

void ResizeOverlay::syncX(ResizeHandle *rh1, ResizeHandle *rh2, ResizeHandle *rh3)
{
    syncX(rh1, rh2);
    syncX(rh1, rh3);
    syncX(rh2, rh3);
}
void ResizeOverlay::syncY(ResizeHandle *rh1, ResizeHandle *rh2, ResizeHandle *rh3)
{
    syncY(rh1, rh2);
    syncY(rh1, rh3);
    syncY(rh2, rh3);
}
void ResizeOverlay::syncX(ResizeHandle *rh1, ResizeHandle *rh2)
{
    if (!rh1 || !rh2)
        return;
    connect(rh1, SIGNAL(rhMovedByX(double)), rh2, SLOT(slotMoveByX(double)));
    connect(rh2, SIGNAL(rhMovedByX(double)), rh1, SLOT(slotMoveByX(double)));
}
void ResizeOverlay::syncY(ResizeHandle *rh1, ResizeHandle *rh2)
{
    if (!rh1 || !rh2)
        return;
    connect(rh1, SIGNAL(rhMovedByY(double)), rh2, SLOT(slotMoveByY(double)));
    connect(rh2, SIGNAL(rhMovedByY(double)), rh1, SLOT(slotMoveByY(double)));
}
// END class ResizeOverlay

// BEGIN class MechanicsItemOverlay
MechanicsItemOverlay::MechanicsItemOverlay(MechanicsItem *parent)
    : ResizeOverlay(parent)
{
    p_mechanicsItem = parent;
    connect(parent, SIGNAL(moved()), this, SLOT(slotUpdateResizeHandles()));
    connect(parent, SIGNAL(resized()), this, SLOT(slotUpdateResizeHandles()));

    m_tl = createResizeHandle(ResizeHandle::rhp_topLeft, ResizeHandle::dt_resize_backwardsDiagonal);
    m_tm = createResizeHandle(ResizeHandle::rhp_topMiddle, ResizeHandle::dt_resize_vertical);
    m_tr = createResizeHandle(ResizeHandle::rhp_topRight, ResizeHandle::dt_resize_forwardsDiagonal);
    m_mr = createResizeHandle(ResizeHandle::rhp_middleRight, ResizeHandle::dt_resize_horizontal);
    m_br = createResizeHandle(ResizeHandle::rhp_bottomRight, ResizeHandle::dt_resize_backwardsDiagonal);
    m_bm = createResizeHandle(ResizeHandle::rhp_bottomMiddle, ResizeHandle::dt_resize_vertical);
    m_bl = createResizeHandle(ResizeHandle::rhp_bottomLeft, ResizeHandle::dt_resize_forwardsDiagonal);
    m_ml = createResizeHandle(ResizeHandle::rhp_middleLeft, ResizeHandle::dt_resize_horizontal);
    m_mm = createResizeHandle(ResizeHandle::rhp_center, ResizeHandle::dt_point_crosshair);

    slotUpdateResizeHandles();
}

MechanicsItemOverlay::~MechanicsItemOverlay()
{
}

void MechanicsItemOverlay::slotUpdateResizeHandles()
{
    const PositionInfo absPos = p_mechanicsItem->absolutePosition();
    const QRect sizeRect = p_mechanicsItem->sizeRect();

    QPolygon pa(9);
    pa[0] = sizeRect.topLeft();
    pa[2] = sizeRect.topRight();
    pa[1] = (pa[0] + pa[2]) / 2;
    pa[4] = sizeRect.bottomRight();
    pa[3] = (pa[2] + pa[4]) / 2;
    pa[6] = sizeRect.bottomLeft();
    pa[5] = (pa[4] + pa[6]) / 2;
    pa[7] = (pa[6] + pa[0]) / 2;
    pa[8] = QPoint(0, 0);

    QMatrix m;
    m.rotate(absPos.angle() * DPR);

    pa = m.map(pa);

    m_tl->move(absPos.x() + pa[0].x(), absPos.y() + pa[0].y());
    m_tm->move(absPos.x() + pa[1].x(), absPos.y() + pa[1].y());
    m_tr->move(absPos.x() + pa[2].x(), absPos.y() + pa[2].y());
    m_mr->move(absPos.x() + pa[3].x(), absPos.y() + pa[3].y());
    m_br->move(absPos.x() + pa[4].x(), absPos.y() + pa[4].y());
    m_bm->move(absPos.x() + pa[5].x(), absPos.y() + pa[5].y());
    m_bl->move(absPos.x() + pa[6].x(), absPos.y() + pa[6].y());
    m_ml->move(absPos.x() + pa[7].x(), absPos.y() + pa[7].y());
    m_mm->move(absPos.x() + pa[8].x(), absPos.y() + pa[8].y());
}

void MechanicsItemOverlay::slotResizeHandleMoved(int id, double dx, double dy)
{
    Q_UNUSED(id);
    Q_UNUSED(dx);
    Q_UNUSED(dy);

    switch (id) {
    case ResizeHandle::rhp_topLeft:
        break;
    case ResizeHandle::rhp_topMiddle:
        break;
    case ResizeHandle::rhp_topRight:
        break;
    case ResizeHandle::rhp_middleRight:
        break;
    case ResizeHandle::rhp_bottomRight:
        break;
    case ResizeHandle::rhp_bottomMiddle:
        break;
    case ResizeHandle::rhp_bottomLeft:
        break;
    case ResizeHandle::rhp_middleLeft:
        break;
    case ResizeHandle::rhp_center:
        break;
    default:
        qCritical() << Q_FUNC_INFO << "Unknown resize handle id " << id << endl;
        break;
    }
}
// END class MechanicsItemOverlay

// BEGIN class RectangularOverlay
RectangularOverlay::RectangularOverlay(Item *parent, int xsnap, int ysnap)
    : ResizeOverlay(parent)
{
    connect(parent, SIGNAL(resized()), this, SLOT(slotUpdateResizeHandles()));
    connect(parent, SIGNAL(movedBy(double, double)), this, SLOT(slotMoveAllResizeHandles(double, double)));

    m_tl = createResizeHandle(ResizeHandle::rhp_topLeft, ResizeHandle::dt_resize_backwardsDiagonal, xsnap, ysnap);
    m_tm = createResizeHandle(ResizeHandle::rhp_topMiddle, ResizeHandle::dt_resize_vertical, xsnap, ysnap);
    m_tr = createResizeHandle(ResizeHandle::rhp_topRight, ResizeHandle::dt_resize_forwardsDiagonal, xsnap, ysnap);
    m_mr = createResizeHandle(ResizeHandle::rhp_middleRight, ResizeHandle::dt_resize_horizontal, xsnap, ysnap);
    m_br = createResizeHandle(ResizeHandle::rhp_bottomRight, ResizeHandle::dt_resize_backwardsDiagonal, xsnap, ysnap);
    m_bm = createResizeHandle(ResizeHandle::rhp_bottomMiddle, ResizeHandle::dt_resize_vertical, xsnap, ysnap);
    m_bl = createResizeHandle(ResizeHandle::rhp_bottomLeft, ResizeHandle::dt_resize_forwardsDiagonal, xsnap, ysnap);
    m_ml = createResizeHandle(ResizeHandle::rhp_middleLeft, ResizeHandle::dt_resize_horizontal, xsnap, ysnap);

    syncX(m_tl, m_ml, m_bl);
    syncX(m_tr, m_mr, m_br);
    syncY(m_tl, m_tm, m_tr);
    syncY(m_bl, m_bm, m_br);

    slotUpdateResizeHandles();
}

void RectangularOverlay::removeTopMiddle()
{
    if (!m_tm)
        return;
    removeResizeHandle(m_tm->id());
    m_tm = nullptr;
}

void RectangularOverlay::removeBotMiddle()
{
    if (!m_bm)
        return;
    removeResizeHandle(m_bm->id());
    m_bm = nullptr;
}

void RectangularOverlay::slotUpdateResizeHandles()
{
    const QRect sizeRect = p_item->sizeRect();

    int x1 = sizeRect.left() + int(p_item->x());
    int x2 = x1 + sizeRect.width();

    int y1 = sizeRect.top() + int(p_item->y());
    int y2 = y1 + sizeRect.height();

    m_tl->move(x1, y1);
    if (m_tm)
        m_tm->move((x1 + x2) / 2, y1);
    m_tr->move(x2, y1);
    m_mr->move(x2, (y1 + y2) / 2);
    m_br->move(x2, y2);
    if (m_bm)
        m_bm->move((x1 + x2) / 2, y2);
    m_bl->move(x1, y2);
    m_ml->move(x1, (y1 + y2) / 2);
}

bool RectangularOverlay::isValidXPos(ResizeHandle *rh)
{
    Q_UNUSED(rh);
    bool ok;
    getSizeRect(nullptr, &ok, nullptr);
    return ok;
}

bool RectangularOverlay::isValidYPos(ResizeHandle *rh)
{
    Q_UNUSED(rh);
    bool ok;
    getSizeRect(nullptr, nullptr, &ok);
    return ok;
}

void RectangularOverlay::slotResizeHandleMoved(int id, double dx, double dy)
{
    Q_UNUSED(id);
    Q_UNUSED(dx);
    Q_UNUSED(dy);

    bool ok;
    QRect sizeRect = getSizeRect(&ok);
    if (!ok)
        return;

    p_item->setSize(sizeRect);
    slotUpdateResizeHandles();
}

QRect RectangularOverlay::getSizeRect(bool *ok, bool *widthOk, bool *heightOk) const
{
    bool t1, t2, t3;
    if (!ok)
        ok = &t1;
    if (!widthOk)
        widthOk = &t2;
    if (!heightOk)
        heightOk = &t3;

    int width = int(m_br->x() - m_tl->x());
    int height = int(m_br->y() - m_tl->y());

    QRect sizeRect(int(m_tl->x() - p_item->x()), int(m_tl->y() - p_item->y()), width, height);

    *widthOk = sizeRect.width() >= p_item->minimumSize().width();
    *heightOk = sizeRect.height() >= p_item->minimumSize().height();
    *ok = *widthOk && *heightOk;

    return sizeRect;
}
// END class RectangularOverlay

// BEGIN class LineOverlay
LineOverlay::LineOverlay(Item *parent)
    : ResizeOverlay(parent)
{
    connect(parent, SIGNAL(resized()), this, SLOT(slotUpdateResizeHandles()));
    connect(parent, SIGNAL(movedBy(double, double)), this, SLOT(slotMoveAllResizeHandles(double, double)));

    m_pStart = createResizeHandle(ResizeHandle::rhp_start, ResizeHandle::dt_point_rect);
    m_pEnd = createResizeHandle(ResizeHandle::rhp_end, ResizeHandle::dt_point_rect);

    slotUpdateResizeHandles();
}

QPoint LineOverlay::startPoint() const
{
    return QPoint(int(m_pStart->x()), int(m_pStart->y()));
}
QPoint LineOverlay::endPoint() const
{
    return QPoint(int(m_pEnd->x()), int(m_pEnd->y()));
}

void LineOverlay::slotUpdateResizeHandles()
{
    int _x = int(p_item->x() + p_item->offsetX());
    int _y = int(p_item->y() + p_item->offsetY());

    m_pStart->move(_x, _y);
    m_pEnd->move(_x + p_item->width(), _y + p_item->height());
}

void LineOverlay::slotResizeHandleMoved(int id, double dx, double dy)
{
    Q_UNUSED(id);
    Q_UNUSED(dx);
    Q_UNUSED(dy);

    p_item->setSize(int(m_pStart->x() - p_item->x()), int(m_pStart->y() - p_item->y()), int(m_pEnd->x() - m_pStart->x()), int(m_pEnd->y() - m_pStart->y()));
}
// END class LineOverlay

// BEGIN class ResizeHandle
ResizeHandle::ResizeHandle(ResizeOverlay *resizeOverlay, int id, DrawType drawType, int xsnap, int ysnap)
    : // QObject(),
    KtlQCanvasRectangle(0, 0, 13, 13, resizeOverlay->parentItem()->canvas())
{
    p_resizeOverlay = resizeOverlay;
    m_drawType = drawType;
    m_id = id;
    b_hover = false;
    m_xsnap = xsnap;
    m_ysnap = ysnap;
    setZ(ItemDocument::Z::ResizeHandle);
}

ResizeHandle::~ResizeHandle()
{
    hide();
}

void ResizeHandle::setHover(bool hover)
{
    if (b_hover == hover)
        return;

    b_hover = hover;
    canvas()->setChanged(QRect(int(x()) - 8, int(y()) - 8, 15, 15));
}

QPolygon ResizeHandle::areaPoints() const
{
    // 	QPolygon pa = KtlQCanvasRectangle::areaPoints();
    // 	pa.translate( -7, -7 );
    // 	return pa;
    return QPolygon(QRect(int(x()) - 8, int(y()) - 8, 15, 15));
}

void ResizeHandle::moveRH(double _x, double _y)
{
    double dx = int((_x - 4) / m_xsnap) * m_xsnap + 4 - x();
    double dy = int((_y - 4) / m_ysnap) * m_ysnap + 4 - y();
    if ((dx == 0) && (dy == 0))
        return;

    // BEGIN Move and check
    moveBy(dx, dy);
    if (dx != 0)
        emit rhMovedByX(dx);
    if (dy != 0)
        emit rhMovedByY(dy);

    bool xOk = p_resizeOverlay->isValidXPos(this);
    bool yOk = p_resizeOverlay->isValidYPos(this);

    if (!xOk) {
        moveBy(-dx, 0);
        emit rhMovedByX(-dx);
        dx = 0;
    }
    if (!yOk) {
        moveBy(0, -dy);
        emit rhMovedByY(-dy);
        dy = 0;
    }

    if (!xOk && !yOk)
        return;
    // END Move and check

    emit rhMovedBy(id(), dx, dy);
}

void ResizeHandle::setDrawType(DrawType drawType)
{
    m_drawType = drawType;
    canvas()->setChanged(boundingRect());
}

void ResizeHandle::drawShape(QPainter &p)
{
    p.drawPixmap(rect().topLeft() - QPoint(7, 7), handlePixmap(m_drawType, b_hover));
}

const QPixmap &ResizeHandle::handlePixmap(DrawType drawType, bool hover)
{
    const char *resize_forwardsDiagonal_hover_xpm[] = {"13 13 3 1",
                                                       " 	c None",
                                                       ".	c #000000",
                                                       "+	c #8EA5D0",
                                                       "             ",
                                                       "     ....... ",
                                                       "      ..+++. ",
                                                       "      .++++. ",
                                                       "     .+++++. ",
                                                       " .  .+++++.. ",
                                                       " ...+++++... ",
                                                       " ..+++++.  . ",
                                                       " .+++++.     ",
                                                       " .++++.      ",
                                                       " .+++..      ",
                                                       " .......     ",
                                                       "             "};
    static QPixmap pixmap_forwardsDiagonal_hover(resize_forwardsDiagonal_hover_xpm);

    const char *resize_forwardsDiagonal_nohover_xpm[] = {"13 13 2 1",
                                                         " 	c None",
                                                         ".	c #000000",
                                                         "             ",
                                                         "     ....... ",
                                                         "      ...... ",
                                                         "      ...... ",
                                                         "     ....... ",
                                                         " .  ........ ",
                                                         " ........... ",
                                                         " ........  . ",
                                                         " .......     ",
                                                         " ......      ",
                                                         " ......      ",
                                                         " .......     ",
                                                         "             "};
    static QPixmap pixmap_forwardsDiagonal_nohover(resize_forwardsDiagonal_nohover_xpm);

    const char *resize_backwardsDiagonal_hover_xpm[] = {"13 13 3 1",
                                                        " 	c None",
                                                        ".	c #000000",
                                                        "+	c #8EA5D0",
                                                        "             ",
                                                        " .......     ",
                                                        " .+++..      ",
                                                        " .++++.      ",
                                                        " .+++++.     ",
                                                        " ..+++++.  . ",
                                                        " ...+++++... ",
                                                        " .  .+++++.. ",
                                                        "     .+++++. ",
                                                        "      .++++. ",
                                                        "      ..+++. ",
                                                        "     ....... ",
                                                        "             "};
    static QPixmap pixmap_backwardsDiagonal_hover(resize_backwardsDiagonal_hover_xpm);

    const char *resize_backwardsDiagonal_nohover_xpm[] = {"13 13 2 1",
                                                          " 	c None",
                                                          ".	c #000000",
                                                          "             ",
                                                          " .......     ",
                                                          " ......      ",
                                                          " ......      ",
                                                          " .......     ",
                                                          " ........  . ",
                                                          " ........... ",
                                                          " .  ........ ",
                                                          "     ....... ",
                                                          "      ...... ",
                                                          "      ...... ",
                                                          "     ....... ",
                                                          "             "};
    static QPixmap pixmap_backwardsDiagonal_nohover(resize_backwardsDiagonal_nohover_xpm);

    const char *resize_vertical_hover_xpm[] = {"13 13 3 1",
                                               " 	c None",
                                               ".	c #000000",
                                               "+	c #8EA5D0",
                                               "      .      ",
                                               "     ...     ",
                                               "    ..+..    ",
                                               "   ..+++..   ",
                                               "  ..+++++..  ",
                                               "    .+++.    ",
                                               "    .+++.    ",
                                               "    .+++.    ",
                                               "  ..+++++..  ",
                                               "   ..+++..   ",
                                               "    ..+..    ",
                                               "     ...     ",
                                               "      .      "};
    static QPixmap pixmap_vertical_hover(resize_vertical_hover_xpm);

    const char *resize_vertical_nohover_xpm[] = {"13 13 2 1",
                                                 " 	c None",
                                                 ".	c #000000",
                                                 "      .      ",
                                                 "     ...     ",
                                                 "    .....    ",
                                                 "   .......   ",
                                                 "  .........  ",
                                                 "    .....    ",
                                                 "    .....    ",
                                                 "    .....    ",
                                                 "  .........  ",
                                                 "   .......   ",
                                                 "    .....    ",
                                                 "     ...     ",
                                                 "      .      "};
    static QPixmap pixmap_vertical_nohover(resize_vertical_nohover_xpm);

    const char *resize_horizontal_hover_xpm[] = {"13 13 3 1",
                                                 " 	c None",
                                                 ".	c #000000",
                                                 "+	c #8EA5D0",
                                                 "             ",
                                                 "             ",
                                                 "    .   .    ",
                                                 "   ..   ..   ",
                                                 "  ..+...+..  ",
                                                 " ..+++++++.. ",
                                                 "..+++++++++..",
                                                 " ..+++++++.. ",
                                                 "  ..+...+..  ",
                                                 "   ..   ..   ",
                                                 "    .   .    ",
                                                 "             ",
                                                 "             "};
    static QPixmap pixmap_horizontal_hover(resize_horizontal_hover_xpm);

    const char *resize_horizontal_nohover_xpm[] = {"13 13 2 1",
                                                   " 	c None",
                                                   ".	c #000000",
                                                   "             ",
                                                   "             ",
                                                   "    .   .    ",
                                                   "   ..   ..   ",
                                                   "  .........  ",
                                                   " ........... ",
                                                   ".............",
                                                   " ........... ",
                                                   "  .........  ",
                                                   "   ..   ..   ",
                                                   "    .   .    ",
                                                   "             ",
                                                   "             "};
    static QPixmap pixmap_horizontal_nohover(resize_horizontal_nohover_xpm);

    const char *point_rect_hover_xpm[] = {"13 13 3 1",
                                          " 	c None",
                                          ".	c #000000",
                                          "+	c #8EA5D0",
                                          "             ",
                                          "             ",
                                          "             ",
                                          "             ",
                                          "             ",
                                          "     .....   ",
                                          "     .+++.   ",
                                          "     .+++.   ",
                                          "     .+++.   ",
                                          "     .....   ",
                                          "             ",
                                          "             ",
                                          "             "};
    static QPixmap pixmap_point_rect_hover(point_rect_hover_xpm);

    const char *point_rect_nohover_xpm[] = {"13 13 3 1",
                                            " 	c None",
                                            ".	c #000000",
                                            "+	c #FFFFFF",
                                            "             ",
                                            "             ",
                                            "             ",
                                            "             ",
                                            "             ",
                                            "     .....   ",
                                            "     .+++.   ",
                                            "     .+++.   ",
                                            "     .+++.   ",
                                            "     .....   ",
                                            "             ",
                                            "             ",
                                            "             "};
    static QPixmap pixmap_point_rect_nohover(point_rect_nohover_xpm);

    if (hover) {
        switch (drawType) {
        case ResizeHandle::dt_resize_forwardsDiagonal:
            return pixmap_forwardsDiagonal_hover;
        case ResizeHandle::dt_resize_backwardsDiagonal:
            return pixmap_backwardsDiagonal_hover;
        case ResizeHandle::dt_resize_vertical:
            return pixmap_vertical_hover;
        case ResizeHandle::dt_resize_horizontal:
            return pixmap_horizontal_hover;
        case ResizeHandle::dt_point_rect:
            return pixmap_point_rect_hover;

        case ResizeHandle::dt_point_crosshair:
        case ResizeHandle::dt_rotate_topLeft:
        case ResizeHandle::dt_rotate_topRight:
        case ResizeHandle::dt_rotate_bottomRight:
        case ResizeHandle::dt_rotate_bottomLeft:
            qWarning() << Q_FUNC_INFO << "ResizeHandle of type " << drawType << " does not have an image." << endl;
        }
    } else {
        switch (drawType) {
        case ResizeHandle::dt_resize_forwardsDiagonal:
            return pixmap_forwardsDiagonal_nohover;
        case ResizeHandle::dt_resize_backwardsDiagonal:
            return pixmap_backwardsDiagonal_nohover;
        case ResizeHandle::dt_resize_vertical:
            return pixmap_vertical_nohover;
        case ResizeHandle::dt_resize_horizontal:
            return pixmap_horizontal_nohover;
        case ResizeHandle::dt_point_rect:
            return pixmap_point_rect_nohover;

        case ResizeHandle::dt_point_crosshair:
        case ResizeHandle::dt_rotate_topLeft:
        case ResizeHandle::dt_rotate_topRight:
        case ResizeHandle::dt_rotate_bottomRight:
        case ResizeHandle::dt_rotate_bottomLeft:
            qWarning() << Q_FUNC_INFO << "ResizeHandle of type " << drawType << " does not have an image." << endl;
        }
    }

    static QPixmap blank;
    return blank;
}
// END class ResizeHandle
