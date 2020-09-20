/***************************************************************************
 *   Copyright (C) 1999-2005 Trolltech AS                                  *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This file may be distributed and/or modified under the terms of the   *
 *   GNU General Public License version 2 as published by the Free         *
 *   Software Foundation                                                   *
 ***************************************************************************/

#include "canvasitems.h"
#include "canvas.h"
#include "canvas_private.h"

#include <QBrush>
#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPolygon>
#include <QRect>

static bool isCanvasDebugEnabled()
{
    return false;
}

KtlQCanvasItem::KtlQCanvasItem(KtlQCanvas *canvas)
    : val(false)
    , myx(0)
    , myy(0)
    , myz(0)
    , cnv(canvas)
    , ext(nullptr)
    , m_bNeedRedraw(true)
    , vis(false)
    , sel(false)
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
    if (cnv)
        cnv->addItem(this);
}

KtlQCanvasItem::~KtlQCanvasItem()
{
    if (cnv)
        cnv->removeItem(this);
    delete ext;
}

KtlQCanvasItemExtra &KtlQCanvasItem::extra()
{
    if (!ext)
        ext = new KtlQCanvasItemExtra;
    return *ext;
}

void KtlQCanvasItem::setZ(double a)
{
    if (myz == a)
        return;

    // remove and then add the item so that z-ordered list in canvas is updated

    if (cnv)
        cnv->removeItem(this);

    myz = a;
    changeChunks();

    if (cnv)
        cnv->addItem(this);
}

void KtlQCanvasItem::moveBy(const double dx, const double dy)
{
    if (dx || dy) {
        removeFromChunks();
        myx += dx;
        myy += dy;
        addToChunks();
    }
}

void KtlQCanvasItem::move(const double x, const double y)
{
    moveBy(x - myx, y - myy);
}

void KtlQCanvasItem::setCanvas(KtlQCanvas *c)
{
    bool v = isVisible();
    setVisible(false);
    if (cnv) {
        cnv->removeItem(this);
    }

    cnv = c;

    if (cnv) {
        cnv->addItem(this);
    }

    setVisible(v);
}

void KtlQCanvasItem::show()
{
    setVisible(true);
}

void KtlQCanvasItem::hide()
{
    setVisible(false);
}

void KtlQCanvasItem::setVisible(bool yes)
{
    if (vis != yes) {
        if (yes) {
            vis = (uint)yes;
            addToChunks();
        } else {
            removeFromChunks();
            vis = (uint)yes;
        }
    }
}

void KtlQCanvasItem::setSelected(const bool yes)
{
    if ((bool)sel != yes) {
        sel = (uint)yes;
        changeChunks();
    }
}

static bool collision_double_dispatch(const KtlQCanvasPolygonalItem *p1, const KtlQCanvasRectangle *r1, const KtlQCanvasEllipse *e1, const KtlQCanvasPolygonalItem *p2, const KtlQCanvasRectangle *r2, const KtlQCanvasEllipse *e2)
{
    const KtlQCanvasItem *i1 = nullptr;
    if (p1) {
        i1 = p1;
    } else {
        if (r1) {
            i1 = r1;
        } else {
            i1 = e1;
        }
    }
    const KtlQCanvasItem *i2 = nullptr;
    if (i2) {
        i2 = p2;
    } else {
        if (r2) {
            i2 = r2;
        } else {
            i2 = e2;
        }
    }
    //  const KtlQCanvasItem* i1 = p1 ?
    //          (const KtlQCanvasItem*)p1 : r1 ?
    //          (const KtlQCanvasItem*)r1 : (const KtlQCanvasItem*)e1;
    //  const KtlQCanvasItem* i2 = p2 ?
    //          (const KtlQCanvasItem*)p2 : r2 ?
    //          (const KtlQCanvasItem*)r2 : (const KtlQCanvasItem*)e2;

    if (r1 && r2) {
        // b
        QRect rc1 = i1->boundingRect();
        QRect rc2 = i2->boundingRect();
        return rc1.intersects(rc2);
    } else if (e1 && e2 && e1->angleLength() >= 360 * 16 && e2->angleLength() >= 360 * 16 && e1->width() == e1->height() && e2->width() == e2->height()) {
        // c
        double xd = (e1->x()) - (e2->x());
        double yd = (e1->y()) - (e2->y());
        double rd = (e1->width() + e2->width()) / 2;
        return xd * xd + yd * yd <= rd * rd;
    } else if (p1 && p2) {
        // d
        QPolygon pa1 = p1->areaPoints();
        QPolygon pa2 = p2 ? p2->areaPoints() : QPolygon(i2->boundingRect());
        bool col = !(QRegion(pa1) & QRegion(pa2, /* true */ Qt::WindingFill)).isEmpty();

        return col;
    } else {
        return collision_double_dispatch(p2, r2, e2, p1, r1, e1);
    }
}

bool KtlQCanvasPolygonalItem::collidesWith(const KtlQCanvasItem *i) const
{
    return i->collidesWith(this, nullptr, nullptr);
}

bool KtlQCanvasPolygonalItem::collidesWith(const KtlQCanvasPolygonalItem *p, const KtlQCanvasRectangle *r, const KtlQCanvasEllipse *e) const
{
    return collision_double_dispatch(p, r, e, this, nullptr, nullptr);
}

bool KtlQCanvasRectangle::collidesWith(const KtlQCanvasItem *i) const
{
    return i->collidesWith(this, this, nullptr);
}

bool KtlQCanvasRectangle::collidesWith(const KtlQCanvasPolygonalItem *p, const KtlQCanvasRectangle *r, const KtlQCanvasEllipse *e) const
{
    return collision_double_dispatch(p, r, e, this, this, nullptr);
}

bool KtlQCanvasEllipse::collidesWith(const KtlQCanvasItem *i) const
{
    return i->collidesWith(this, nullptr, this);
}

bool KtlQCanvasEllipse::collidesWith(const KtlQCanvasPolygonalItem *p, const KtlQCanvasRectangle *r, const KtlQCanvasEllipse *e) const
{
    return collision_double_dispatch(p, r, e, this, nullptr, this);
}

KtlQCanvasItemList KtlQCanvasItem::collisions(const bool exact) const
{
    return canvas()->collisions(chunks(), this, exact);
}

void KtlQCanvasItem::addToChunks()
{
    if (isVisible() && canvas()) {
        QPolygon pa = chunks();
        for (int i = 0; i < (int)pa.count(); i++)
            canvas()->addItemToChunk(this, pa[i].x(), pa[i].y());
        val = true;
    }
}

void KtlQCanvasItem::removeFromChunks()
{
    if (isVisible() && canvas()) {
        QPolygon pa = chunks();
        for (int i = 0; i < (int)pa.count(); i++)
            canvas()->removeItemFromChunk(this, pa[i].x(), pa[i].y());
    }
}

void KtlQCanvasItem::changeChunks()
{
    if (isVisible() && canvas()) {
        if (!val)
            addToChunks();
        QPolygon pa = chunks();
        for (int i = 0; i < (int)pa.count(); i++)
            canvas()->setChangedChunk(pa[i].x(), pa[i].y());
    }
}

QPolygon KtlQCanvasItem::chunks() const
{
    QPolygon r;
    int n = 0;
    QRect br = boundingRect();
    if (isVisible() && canvas()) {
        br &= canvas()->rect();
        if (br.isValid()) {
            r.resize((canvas()->toChunkScaling(br.width()) + 2) * (canvas()->toChunkScaling(br.height()) + 2));
            for (int j = canvas()->toChunkScaling(br.top()); j <= canvas()->toChunkScaling(br.bottom()); j++) {
                for (int i = canvas()->toChunkScaling(br.left()); i <= canvas()->toChunkScaling(br.right()); i++) {
                    r[n++] = QPoint(i, j);
                }
            }
        }
    }
    r.resize(n);
    return r;
}

/*
    Since most polygonal items don't have a pen, the default is
    NoPen and a black brush.
*/
static const QPen &defaultPolygonPen()
{
    static QPen *dp = nullptr;
    if (!dp)
        dp = new QPen;
    return *dp;
}

static const QBrush &defaultPolygonBrush()
{
    static QBrush *db = nullptr;
    if (!db)
        db = new QBrush;
    return *db;
}

KtlQCanvasPolygonalItem::KtlQCanvasPolygonalItem(KtlQCanvas *canvas)
    : KtlQCanvasItem(canvas)
    , br(defaultPolygonBrush())
    , pn(defaultPolygonPen())
    , wind(false)
{
    if (isCanvasDebugEnabled()) {
        qDebug() << "created KtlQCanvasPolygonalItem at " << this;
    }
}

KtlQCanvasPolygonalItem::~KtlQCanvasPolygonalItem()
{
    if (isCanvasDebugEnabled()) {
        qDebug() << "destroying KtlQCanvasPolygonalItem at " << this;
    }
}

bool KtlQCanvasPolygonalItem::winding() const
{
    return wind;
}

void KtlQCanvasPolygonalItem::setWinding(bool enable)
{
    wind = enable;
}

void KtlQCanvasPolygonalItem::invalidate()
{
    val = false;
    removeFromChunks();
}

QPolygon KtlQCanvasPolygonalItem::chunks() const
{
    QPolygon pa = areaPoints();

    if (!pa.size()) {
        pa.detach(); // Explicit sharing is stupid.
        return pa;
    }

    KtlQPolygonalProcessor processor(canvas(), pa);

    scanPolygon(pa, wind, processor);

    return processor.result;
}

QPolygon KtlQCanvasRectangle::chunks() const
{
    // No need to do a polygon scan!
    return KtlQCanvasItem::chunks();
}

QRect KtlQCanvasPolygonalItem::boundingRect() const
{
    return areaPoints().boundingRect();
}

void KtlQCanvasPolygonalItem::draw(QPainter &p)
{
    p.setPen(pn);
    p.setBrush(br);
    drawShape(p);
}

void KtlQCanvasPolygonalItem::setPen(const QPen &p)
{
    if (pn == p)
        return;

    pn.setColor(p.color());

    // if only the color was different, then don't need to re-add to chunks
    if (pn == p) {
        changeChunks();
    } else {
        // Have to re-add to chunks as e.g. pen width might have changed
        removeFromChunks();
        pn = p;
        addToChunks();
    }
}

void KtlQCanvasPolygonalItem::setBrush(const QBrush &b)
{
    if (br != b) {
        br = b;
        changeChunks();
    }
}

KtlQCanvasPolygon::KtlQCanvasPolygon(KtlQCanvas *canvas)
    : KtlQCanvasPolygonalItem(canvas)
    , guardBef()
    , poly(new QPolygon)
    , guardAft()
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}

KtlQCanvasPolygon::~KtlQCanvasPolygon()
{
    hide();
    delete poly;
}

void KtlQCanvasPolygon::drawShape(QPainter &p)
{
    // ### why can't we draw outlines? We could use drawPolyline for it. Lars
    // ### see other message. Warwick

    p.setPen(Qt::NoPen); // since QRegion(QPolygon) excludes outline :-(  )-:
    p.drawPolygon(*poly);
}

void KtlQCanvasPolygon::setPoints(QPolygon pa)
{
    removeFromChunks();
    *poly = pa;
    poly->detach(); // Explicit sharing is stupid.
    poly->translate((int)x(), (int)y());
    addToChunks();
}

void KtlQCanvasPolygon::moveBy(double dx, double dy)
{
    // Note: does NOT call KtlQCanvasPolygonalItem::moveBy(), since that
    // only does half this work.
    //
    int idx = int(x() + dx) - int(x());
    int idy = int(y() + dy) - int(y());
    if (idx || idy) {
        removeFromChunks();
        poly->translate(idx, idy);
    }
    myx += dx;
    myy += dy;
    if (idx || idy) {
        addToChunks();
    }
}

QPolygon KtlQCanvasPolygon::points() const
{
    QPolygon pa = areaPoints();
    pa.translate(int(-x()), int(-y()));
    return pa;
}

QPolygon KtlQCanvasPolygon::areaPoints() const
{
    return QPolygon(*poly); // ->copy(); // 2018.06.02 - copy is only in QPolygon
}

// ### mark: Why don't we offer a constructor that lets the user set the
// points -- that way for some uses just the constructor call would be
// required?

KtlQCanvasLine::KtlQCanvasLine(KtlQCanvas *canvas)
    : KtlQCanvasPolygonalItem(canvas)
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
    x1 = y1 = x2 = y2 = 0;
}

KtlQCanvasLine::~KtlQCanvasLine()
{
    hide();
}

void KtlQCanvasLine::setPen(const QPen &p)
{
    KtlQCanvasPolygonalItem::setPen(p);
}

void KtlQCanvasLine::setPoints(int xa, int ya, int xb, int yb)
{
    if (x1 != xa || x2 != xb || y1 != ya || y2 != yb) {
        removeFromChunks();
        x1 = xa;
        y1 = ya;
        x2 = xb;
        y2 = yb;
        addToChunks();
    }
}

void KtlQCanvasLine::drawShape(QPainter &p)
{
    p.drawLine((int)(x() + x1), (int)(y() + y1), (int)(x() + x2), (int)(y() + y2));
}

QPolygon KtlQCanvasLine::areaPoints() const
{
    QPolygon p(4);
    int xi = int(x());
    int yi = int(y());
    int pw = pen().width();
    int dx = abs(x1 - x2);
    int dy = abs(y1 - y2);
    pw = pw * 4 / 3 + 2; // approx pw*sqrt(2)
    int px = x1 < x2 ? -pw : pw;
    int py = y1 < y2 ? -pw : pw;
    if (dx && dy && (dx > dy ? (dx * 2 / dy <= 2) : (dy * 2 / dx <= 2))) {
        // steep
        if (px == py) {
            p[0] = QPoint(x1 + xi, y1 + yi + py);
            p[1] = QPoint(x2 + xi - px, y2 + yi);
            p[2] = QPoint(x2 + xi, y2 + yi - py);
            p[3] = QPoint(x1 + xi + px, y1 + yi);
        } else {
            p[0] = QPoint(x1 + xi + px, y1 + yi);
            p[1] = QPoint(x2 + xi, y2 + yi - py);
            p[2] = QPoint(x2 + xi - px, y2 + yi);
            p[3] = QPoint(x1 + xi, y1 + yi + py);
        }
    } else if (dx > dy) {
        // horizontal
        p[0] = QPoint(x1 + xi + px, y1 + yi + py);
        p[1] = QPoint(x2 + xi - px, y2 + yi + py);
        p[2] = QPoint(x2 + xi - px, y2 + yi - py);
        p[3] = QPoint(x1 + xi + px, y1 + yi - py);
    } else {
        // vertical
        p[0] = QPoint(x1 + xi + px, y1 + yi + py);
        p[1] = QPoint(x2 + xi + px, y2 + yi - py);
        p[2] = QPoint(x2 + xi - px, y2 + yi - py);
        p[3] = QPoint(x1 + xi - px, y1 + yi + py);
    }
    return p;
}

void KtlQCanvasLine::moveBy(double dx, double dy)
{
    KtlQCanvasPolygonalItem::moveBy(dx, dy);
}

KtlQCanvasRectangle::KtlQCanvasRectangle(KtlQCanvas *canvas)
    : KtlQCanvasPolygonalItem(canvas)
    , w(32)
    , h(32)
{
    setObjectName("KtlQCanvasRectangle");
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}

KtlQCanvasRectangle::KtlQCanvasRectangle(const QRect &r, KtlQCanvas *canvas)
    : KtlQCanvasPolygonalItem(canvas)
    , w(r.width())
    , h(r.height())
{
    setObjectName("KtlQCanvasRectangle");
    move(r.x(), r.y());
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}

KtlQCanvasRectangle::KtlQCanvasRectangle(int x, int y, int width, int height, KtlQCanvas *canvas)
    : KtlQCanvasPolygonalItem(canvas)
    , w(width)
    , h(height)
{
    setObjectName("KtlQCanvasRectangle");
    move(x, y);
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}

KtlQCanvasRectangle::~KtlQCanvasRectangle()
{
    hide();
}

int KtlQCanvasRectangle::width() const
{
    return w;
}

int KtlQCanvasRectangle::height() const
{
    return h;
}

void KtlQCanvasRectangle::setSize(const int width, const int height)
{
    if (w != width || h != height) {
        removeFromChunks();
        w = width;
        h = height;
        addToChunks();
    }
}

QPolygon KtlQCanvasRectangle::areaPoints() const
{
    QPolygon pa(4);
    int pw = (pen().width() + 1) / 2;
    if (pw < 1)
        pw = 1;
    if (pen() == Qt::NoPen)
        pw = 0;
    pa[0] = QPoint((int)x() - pw, (int)y() - pw);
    pa[1] = pa[0] + QPoint(w + pw * 2, 0);
    pa[2] = pa[1] + QPoint(0, h + pw * 2);
    pa[3] = pa[0] + QPoint(0, h + pw * 2);
    return pa;
}

void KtlQCanvasRectangle::drawShape(QPainter &p)
{
    p.drawRect((int)x(), (int)y(), w, h);
}

KtlQCanvasEllipse::KtlQCanvasEllipse(KtlQCanvas *canvas)
    : KtlQCanvasPolygonalItem(canvas)
    , w(32)
    , h(32)
    , a1(0)
    , a2(360 * 16)
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}

/*!
    Constructs a \a width by \a height pixel ellipse, centered at
    (0, 0) on \a canvas.
 */
KtlQCanvasEllipse::KtlQCanvasEllipse(int width, int height, KtlQCanvas *canvas)
    : KtlQCanvasPolygonalItem(canvas)
    , w(width)
    , h(height)
    , a1(0)
    , a2(360 * 16)
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}

KtlQCanvasEllipse::KtlQCanvasEllipse(int width, int height, int startangle, int angle, KtlQCanvas *canvas)
    : KtlQCanvasPolygonalItem(canvas)
    , w(width)
    , h(height)
    , a1(startangle)
    , a2(angle)
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " this=" << this;
    }
}

KtlQCanvasEllipse::~KtlQCanvasEllipse()
{
    hide();
}

int KtlQCanvasEllipse::width() const
{
    return w;
}

int KtlQCanvasEllipse::height() const
{
    return h;
}

void KtlQCanvasEllipse::setSize(int width, int height)
{
    if (w != width || h != height) {
        removeFromChunks();
        w = width;
        h = height;
        addToChunks();
    }
}

void KtlQCanvasEllipse::setAngles(int start, int length)
{
    if (a1 != start || a2 != length) {
        removeFromChunks();
        a1 = start;
        a2 = length;
        addToChunks();
    }
}

QPolygon KtlQCanvasEllipse::areaPoints() const
{
    // Q3PointArray r;     // 2018.08.14 - see below
    // // makeArc at 0,0, then translate so that fixed point math doesn't overflow
    // r.makeArc(int(x()-w/2.0+0.5)-1, int(y()-h/2.0+0.5)-1, w+3, h+3, a1, a2);

    QPainterPath path;
    path.arcTo(int(x() - w / 2.0 + 0.5) - 1, int(y() - h / 2.0 + 0.5) - 1, w + 3, h + 3, a1, a2 - a1);
    QPolygon r = path.toFillPolygon().toPolygon();

    r.resize(r.size() + 1);
    r.setPoint(r.size() - 1, int(x()), int(y()));
    return QPolygon(r);
}

void KtlQCanvasEllipse::drawShape(QPainter &p)
{
    p.setPen(Qt::NoPen); // since QRegion(QPolygon) excludes outline :-(  )-:
    if (!a1 && a2 == 360 * 16) {
        p.drawEllipse(int(x() - w / 2.0 + 0.5), int(y() - h / 2.0 + 0.5), w, h);
    } else {
        p.drawPie(int(x() - w / 2.0 + 0.5), int(y() - h / 2.0 + 0.5), w, h, a1, a2);
    }
}

void KtlQCanvasPolygonalItem::scanPolygon(const QPolygon &pa, int winding, KtlQPolygonalProcessor &process) const
{
    KtlQCanvasPolygonScanner scanner(process);
    scanner.scan(pa, winding);
}
