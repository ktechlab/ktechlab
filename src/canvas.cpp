/***************************************************************************
 *   Copyright (C) 1999-2005 Trolltech AS                                  *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This file may be distributed and/or modified under the terms of the   *
 *   GNU General Public License version 2 as published by the Free         *
 *   Software Foundation                                                   *
 ***************************************************************************/

#include "canvas.h"
#include "canvas_private.h"
#include "ktlq3polygonscanner.h"
#include "utils.h"

#include <ktlqt3support/ktlq3scrollview.h>

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
//#include "q3ptrdict.h"
#include <QPainter>
#include <QTimer>
// #include "q3tl.h"
// #include <q3pointarray.h>   // needed for q3polygonscanner

#include <algorithm>

#include <stdlib.h>

using namespace std;

static bool isCanvasDebugEnabled()
{
    return false;
}

// BEGIN class KtlQCanvasClusterizer

static void include(QRect &r, const QRect &rect)
{
    if (rect.left() < r.left()) {
        r.setLeft(rect.left());
    }
    if (rect.right() > r.right()) {
        r.setRight(rect.right());
    }
    if (rect.top() < r.top()) {
        r.setTop(rect.top());
    }
    if (rect.bottom() > r.bottom()) {
        r.setBottom(rect.bottom());
    }
}

/*
A KtlQCanvasClusterizer groups rectangles (QRects) into non-overlapping rectangles
by a merging heuristic.
*/
KtlQCanvasClusterizer::KtlQCanvasClusterizer(int maxclusters)
    : cluster(new QRect[maxclusters])
    , count(0)
    , maxcl(maxclusters)
{
}

KtlQCanvasClusterizer::~KtlQCanvasClusterizer()
{
    delete[] cluster;
}

void KtlQCanvasClusterizer::clear()
{
    count = 0;
}

void KtlQCanvasClusterizer::add(int x, int y)
{
    add(QRect(x, y, 1, 1));
}

void KtlQCanvasClusterizer::add(int x, int y, int w, int h)
{
    add(QRect(x, y, w, h));
}

void KtlQCanvasClusterizer::add(const QRect &rect)
{
    QRect biggerrect(rect.x() - 1, rect.y() - 1, rect.width() + 2, rect.height() + 2);

    // assert(rect.width()>0 && rect.height()>0);

    int cursor;

    for (cursor = 0; cursor < count; cursor++) {
        if (cluster[cursor].contains(rect)) {
            // Wholly contained already.
            return;
        }
    }

    int lowestcost = 9999999;
    int cheapest = -1;
    cursor = 0;
    while (cursor < count) {
        if (cluster[cursor].intersects(biggerrect)) {
            QRect larger = cluster[cursor];
            include(larger, rect);
            int cost = larger.width() * larger.height() - cluster[cursor].width() * cluster[cursor].height();

            if (cost < lowestcost) {
                bool bad = false;
                for (int c = 0; c < count && !bad; c++) {
                    bad = cluster[c].intersects(larger) && c != cursor;
                }

                if (!bad) {
                    cheapest = cursor;
                    lowestcost = cost;
                }
            }
        }
        cursor++;
    }

    if (cheapest >= 0) {
        include(cluster[cheapest], rect);
        return;
    }

    if (count < maxcl) {
        cluster[count++] = rect;
        return;
    }

    // Do cheapest of:
    //     add to closest cluster
    //     do cheapest cluster merge, add to new cluster

    lowestcost = 9999999;
    cheapest = -1;
    cursor = 0;
    while (cursor < count) {
        QRect larger = cluster[cursor];
        include(larger, rect);
        int cost = larger.width() * larger.height() - cluster[cursor].width() * cluster[cursor].height();
        if (cost < lowestcost) {
            bool bad = false;
            for (int c = 0; c < count && !bad; c++) {
                bad = cluster[c].intersects(larger) && c != cursor;
            }

            if (!bad) {
                cheapest = cursor;
                lowestcost = cost;
            }
        }
        cursor++;
    }

    // ###
    // could make an heuristic guess as to whether we need to bother
    // looking for a cheap merge.

    int cheapestmerge1 = -1;
    int cheapestmerge2 = -1;

    int merge1 = 0;
    while (merge1 < count) {
        int merge2 = 0;
        while (merge2 < count) {
            if (merge1 != merge2) {
                QRect larger = cluster[merge1];
                include(larger, cluster[merge2]);
                int cost = larger.width() * larger.height() - cluster[merge1].width() * cluster[merge1].height() - cluster[merge2].width() * cluster[merge2].height();
                if (cost < lowestcost) {
                    bool bad = false;

                    for (int c = 0; c < count && !bad; c++) {
                        bad = cluster[c].intersects(larger) && c != cursor;
                    }

                    if (!bad) {
                        cheapestmerge1 = merge1;
                        cheapestmerge2 = merge2;
                        lowestcost = cost;
                    }
                }
            }
            merge2++;
        }
        merge1++;
    }

    if (cheapestmerge1 >= 0) {
        include(cluster[cheapestmerge1], cluster[cheapestmerge2]);
        cluster[cheapestmerge2] = cluster[count--];
    } else {
        // if (!cheapest) debugRectangles(rect);
        include(cluster[cheapest], rect);
    }

    // NB: clusters do not intersect (or intersection will
    //     overwrite). This is a result of the above algorithm,
    //     given the assumption that (x,y) are ordered topleft
    //     to bottomright.

    // ###
    //
    // add explicit x/y ordering to that comment, move it to the top
    // and rephrase it as pre-/post-conditions.
}

const QRect &KtlQCanvasClusterizer::operator[](int i)
{
    return cluster[i];
}

// END class KtlQCanvasClusterizer

static int gcd(int a, int b)
{
    int r;
    while ((r = a % b)) {
        a = b;
        b = r;
    }
    return b;
}

static int scm(int a, int b)
{
    int g = gcd(a, b);
    return a / g * b;
}

int KtlQCanvas::toChunkScaling(int x) const
{
    return roundDown(x, chunksize);
}

void KtlQCanvas::initChunkSize(const QRect &s)
{
    m_chunkSize = QRect(toChunkScaling(s.left()), toChunkScaling(s.top()), ((s.width() - 1) / chunksize) + 3, ((s.height() - 1) / chunksize) + 3);
}

void KtlQCanvas::init(int w, int h, int chunksze, int mxclusters)
{
    init(QRect(0, 0, w, h), chunksze, mxclusters);
}

void KtlQCanvas::init(const QRect &r, int chunksze, int mxclusters)
{
    m_size = r;
    chunksize = chunksze;
    maxclusters = mxclusters;
    initChunkSize(r);
    chunks = new KtlQCanvasChunk[m_chunkSize.width() * m_chunkSize.height()];
    update_timer = nullptr;
    bgcolor = Qt::white;
    grid = nullptr;
    htiles = 0;
    vtiles = 0;
    debug_redraw_areas = false;
}

KtlQCanvas::KtlQCanvas(QObject *parent, const char *name)
    : QObject(parent /*, name*/)
{
    setObjectName(name);
    init(0, 0);
}

KtlQCanvas::KtlQCanvas(const int w, const int h)
{
    init(w, h);
}

KtlQCanvas::KtlQCanvas(QPixmap p, int h, int v, int tilewidth, int tileheight)
{
    init(h * tilewidth, v * tileheight, scm(tilewidth, tileheight));
    setTiles(p, h, v, tilewidth, tileheight);
}

void qt_unview(KtlQCanvas *c)
{
    for (QList<KtlQCanvasView *>::iterator itView = c->m_viewList.begin(); itView != c->m_viewList.end(); ++itView) {
        KtlQCanvasView *view = *itView;
        view->viewing = nullptr;
    }
}

KtlQCanvas::~KtlQCanvas()
{
    qt_unview(this);
    KtlQCanvasItemList all = allItems();
    for (KtlQCanvasItemList::Iterator it = all.begin(); it != all.end(); ++it)
        delete *it;
    delete[] chunks;
    delete[] grid;
}

/*!
    \internal
    Returns the chunk at a chunk position \a i, \a j.
 */
KtlQCanvasChunk &KtlQCanvas::chunk(int i, int j) const
{
    i -= m_chunkSize.left();
    j -= m_chunkSize.top();
    // return chunks[i+m_chunkSize.width()*j];
    const int chunkOffset = i + m_chunkSize.width() * j;
    if ((chunkOffset < 0) || (chunkOffset >= (m_chunkSize.width() * m_chunkSize.height()))) {
        qWarning() << Q_FUNC_INFO << " invalid chunk coordinates: " << i << " " << j;
        return chunks[0]; // at least it should not crash
    }
    return chunks[chunkOffset];
}

/*!
    \internal
    Returns the chunk at a pixel position \a x, \a y.
 */
KtlQCanvasChunk &KtlQCanvas::chunkContaining(int x, int y) const
{
    return chunk(toChunkScaling(x), toChunkScaling(y));
}

KtlQCanvasItemList KtlQCanvas::allItems()
{
    KtlQCanvasItemList list;
    SortedCanvasItems::iterator end = m_canvasItems.end();
    for (SortedCanvasItems::iterator it = m_canvasItems.begin(); it != end; ++it)
        list << it->second;
    return list;
}

void KtlQCanvas::resize(const QRect &newSize)
{
    if (newSize == m_size)
        return;

    KtlQCanvasItem *item;
    QList<KtlQCanvasItem *> hidden;
    SortedCanvasItems::iterator end = m_canvasItems.end();
    for (SortedCanvasItems::iterator it = m_canvasItems.begin(); it != end; ++it) {
        KtlQCanvasItem *i = it->second;
        if (i->isVisible()) {
            i->hide();
            hidden.append(i);
        }
    }

    initChunkSize(newSize);
    KtlQCanvasChunk *newchunks = new KtlQCanvasChunk[m_chunkSize.width() * m_chunkSize.height()];
    m_size = newSize;
    delete[] chunks;
    chunks = newchunks;

    for (QList<KtlQCanvasItem *>::iterator itItem = hidden.begin(); itItem != hidden.end(); ++itItem) {
        KtlQCanvasItem *item = *itItem;
        item->show();
    }
    // 	for (item=hidden.first(); item != 0; item=hidden.next()) {  // 2018.08.14 - use QList
    // 		item->show();
    // 	}

    setAllChanged();

    emit resized();
}

void KtlQCanvas::retune(int chunksze, int mxclusters)
{
    maxclusters = mxclusters;

    if (chunksize != chunksze) {
        QList<KtlQCanvasItem *> hidden;
        SortedCanvasItems::iterator end = m_canvasItems.end();
        for (SortedCanvasItems::iterator it = m_canvasItems.begin(); it != end; ++it) {
            KtlQCanvasItem *i = it->second;
            if (i->isVisible()) {
                i->hide();
                hidden.append(i);
            }
        }

        chunksize = chunksze;

        initChunkSize(m_size);
        KtlQCanvasChunk *newchunks = new KtlQCanvasChunk[m_chunkSize.width() * m_chunkSize.height()];
        delete[] chunks;
        chunks = newchunks;

        for (QList<KtlQCanvasItem *>::iterator itItem = hidden.begin(); itItem != hidden.end(); ++itItem) {
            KtlQCanvasItem *item = *itItem;
            item->show();
        }
    }
}

void KtlQCanvas::addItem(KtlQCanvasItem *item)
{
    m_canvasItems.insert(make_pair(item->z(), item));
}

void KtlQCanvas::removeItem(const KtlQCanvasItem *item)
{
    SortedCanvasItems::iterator end = m_canvasItems.end();
    for (SortedCanvasItems::iterator it = m_canvasItems.begin(); it != end; ++it) {
        if (it->second == item) {
            m_canvasItems.erase(it);
            return;
        }
    }
}

void KtlQCanvas::addView(KtlQCanvasView *view)
{
    m_viewList.append(view);
    if (htiles > 1 || vtiles > 1 || pm.isNull()) {
        // view->viewport()->setBackgroundColor(backgroundColor()); // 2018.11.21
        QPalette palette;
        palette.setColor(view->viewport()->backgroundRole(), backgroundColor());
        view->viewport()->setPalette(palette);
    }
}

void KtlQCanvas::removeView(KtlQCanvasView *view)
{
    m_viewList.removeAll(view);
}

void KtlQCanvas::setUpdatePeriod(int ms)
{
    if (ms < 0) {
        if (update_timer)
            update_timer->stop();
    } else {
        if (update_timer)
            delete update_timer;
        update_timer = new QTimer(this);
        connect(update_timer, SIGNAL(timeout()), this, SLOT(update()));
        update_timer->start(ms);
    }
}

// Don't call this unless you know what you're doing.
// p is in the content's co-ordinate example.
void KtlQCanvas::drawViewArea(KtlQCanvasView *view, QPainter *p, const QRect &vr, bool dbuf /* always false */)
{
    QPoint tl = view->contentsToViewport(QPoint(0, 0));

    QMatrix wm = view->worldMatrix();
    QMatrix iwm = wm.inverted();
    // ivr = covers all chunks in vr
    QRect ivr = iwm.mapRect(vr);
    QMatrix twm;
    twm.translate(tl.x(), tl.y());

    // 	QRect all(0,0,width(),height());
    QRect all(m_size);

    if (!p->isActive()) {
        qWarning() << Q_FUNC_INFO << " painter is not active";
    }

    if (!all.contains(ivr)) {
        // Need to clip with edge of canvas.

        // For translation-only transformation, it is safe to include the right
        // and bottom edges, but otherwise, these must be excluded since they
        // are not precisely defined (different bresenham paths).
        QPolygon a;
        if (wm.m12() == 0.0 && wm.m21() == 0.0 && wm.m11() == 1.0 && wm.m22() == 1.0)
            a = QPolygon(QRect(all.x(), all.y(), all.width() + 1, all.height() + 1));
        else
            a = QPolygon(all);

        a = (wm * twm).map(a);

        // if ( view->viewport()->backgroundMode() == Qt::NoBackground ) // 2018.12.02
        QWidget *vp = view->viewport();
        if (vp->palette().color(vp->backgroundRole()) == QColor(Qt::transparent)) {
            QRect cvr = vr;
            cvr.translate(tl.x(), tl.y());
            p->setClipRegion(QRegion(cvr) - QRegion(a));
            p->fillRect(vr, view->viewport()->palette().brush(QPalette::Active, QPalette::Window));
        }
        p->setClipRegion(a);
    }

#if 0 // 2018.03.11 - dbuf is always false
	if ( dbuf ) {
		offscr = QPixmap(vr.width(), vr.height());
		offscr.x11SetScreen(p->device()->x11Screen());
		//QPainter dbp(&offscr);
        QPainter dbp;
        const bool isSuccess = dbp.begin(&offscr);
        if (!isSuccess) {
            qWarning() << Q_FUNC_INFO << " painter not active";
        }

		twm.translate(-vr.x(),-vr.y());
		twm.translate(-tl.x(),-tl.y());
		dbp.setWorldMatrix( wm*twm, true );

        // 2015.11.27 - do not clip, in order to fix drawing of garbage on the screen.
		//dbp.setClipRect(0,0,vr.width(), vr.height());
// 		dbp.setClipRect(v);
		drawCanvasArea(ivr,&dbp,false);
		dbp.end();
		p->drawPixmap(vr.x(), vr.y(), offscr, 0, 0, vr.width(), vr.height());
	} else
#endif
    {
        QRect r = vr;
        r.translate(tl.x(), tl.y()); // move to untransformed co-ords
        if (!all.contains(ivr)) {
            QRegion inside = p->clipRegion() & r;
            // QRegion outside = p->clipRegion() - r;
            // p->setClipRegion(outside);
            // p->fillRect(outside.boundingRect(),red);
            // 2015.11.27 - do not clip, in order to fix drawing of garbage on the screen.
            // p->setClipRegion(inside);
        } else {
            // 2015.11.27 - do not clip, in order to fix drawing of garbage on the screen.
            // p->setClipRect(r);
        }
        p->setWorldMatrix(wm * twm);

        p->setBrushOrigin(tl.x(), tl.y());
        drawCanvasArea(ivr, p, false);
    }
}

void KtlQCanvas::advance()
{
    qWarning() << "KtlQCanvas::advance: TODO"; // TODO
}

/*!
    Repaints changed areas in all views of the canvas.
 */
void KtlQCanvas::update()
{
    KtlQCanvasClusterizer clusterizer(m_viewList.count());
    QList<QRect> doneareas;

    // Q3PtrListIterator<KtlQCanvasView> it(m_viewList);   // 2018.08.14 - see below
    // KtlQCanvasView* view;
    // while( (view=it.current()) != 0 ) {
    //	++it;
    //
    for (QList<KtlQCanvasView *>::iterator itView = m_viewList.begin(); itView != m_viewList.end(); ++itView) {
        KtlQCanvasView *view = *itView;

        QMatrix wm = view->worldMatrix();

        QRect area(view->contentsX(), view->contentsY(), view->visibleWidth(), view->visibleHeight());
        if (area.width() > 0 && area.height() > 0) {
            if (!wm.isIdentity()) {
                // r = Visible area of the canvas where there are changes
                QRect r = changeBounds(view->inverseWorldMatrix().mapRect(area));
                if (!r.isEmpty()) {
                    // as of my testing, drawing below always fails, so just post for an update event to the widget
                    view->viewport()->update();

#if 0
                    //view->viewport()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true); // note: remove this when possible
					//QPainter p(view->viewport());
                    QPainter p;
                    const bool startSucces = p.begin( view->viewport() );
                    if (!startSucces) {
                        qWarning() << Q_FUNC_INFO << " painter is not active ";
                    }
		  		  // Translate to the coordinate system of drawViewArea().
					QPoint tl = view->contentsToViewport(QPoint(0,0));
					p.translate(tl.x(),tl.y());
// 					drawViewArea( view, &p, wm.map(r), true );
#endif
                    doneareas.append(r);
                }
            } else
                clusterizer.add(area);
        }
    }

    for (int i = 0; i < clusterizer.clusters(); i++)
        drawChanges(clusterizer[i]);

    // for ( QRect* r=doneareas.first(); r != 0; r=doneareas.next() )        // 2018.08.14 - use iterators
    //	setUnchanged(*r);
    for (QList<QRect>::iterator itDone = doneareas.begin(); itDone != doneareas.end(); ++itDone) {
        setUnchanged(*itDone);
    }
}

/*!
    Marks the whole canvas as changed.
    All views of the canvas will be entirely redrawn when
    update() is called next.
 */
void KtlQCanvas::setAllChanged()
{
    setChanged(m_size);
}

/*!
    Marks \a area as changed. This \a area will be redrawn in all
    views that are showing it when update() is called next.
 */
void KtlQCanvas::setChanged(const QRect &area)
{
    QRect thearea = area.intersect(m_size);

    int mx = toChunkScaling(thearea.x() + thearea.width() + chunksize);
    int my = toChunkScaling(thearea.y() + thearea.height() + chunksize);
    if (mx > m_chunkSize.right())
        mx = m_chunkSize.right();
    if (my > m_chunkSize.bottom())
        my = m_chunkSize.bottom();

    int x = toChunkScaling(thearea.x());
    while (x < mx) {
        int y = toChunkScaling(thearea.y());
        while (y < my) {
            chunk(x, y).change();
            y++;
        }
        x++;
    }
}

/*!
    Marks \a area as \e unchanged. The area will \e not be redrawn in
    the views for the next update(), unless it is marked or changed
    again before the next call to update().
 */
void KtlQCanvas::setUnchanged(const QRect &area)
{
    QRect thearea = area.intersect(m_size);

    int mx = toChunkScaling(thearea.x() + thearea.width() + chunksize);
    int my = toChunkScaling(thearea.y() + thearea.height() + chunksize);
    if (mx > m_chunkSize.right())
        mx = m_chunkSize.right();
    if (my > m_chunkSize.bottom())
        my = m_chunkSize.bottom();

    int x = toChunkScaling(thearea.x());
    while (x < mx) {
        int y = toChunkScaling(thearea.y());
        while (y < my) {
            chunk(x, y).takeChange();
            y++;
        }
        x++;
    }
}

QRect KtlQCanvas::changeBounds(const QRect &inarea)
{
    QRect area = inarea.intersect(m_size);

    int mx = toChunkScaling(area.x() + area.width() + chunksize);
    int my = toChunkScaling(area.y() + area.height() + chunksize);
    if (mx > m_chunkSize.right())
        mx = m_chunkSize.right();
    if (my > m_chunkSize.bottom())
        my = m_chunkSize.bottom();

    QRect result;

    int x = toChunkScaling(area.x());
    while (x < mx) {
        int y = toChunkScaling(area.y());
        while (y < my) {
            KtlQCanvasChunk &ch = chunk(x, y);
            if (ch.hasChanged())
                result |= QRect(x, y, 1, 1);
            y++;
        }
        x++;
    }

    if (!result.isEmpty()) {
        // result.rLeft() *= chunksize; // 2018.11.18
        // result.rTop() *= chunksize;
        // result.rRight() *= chunksize;
        // result.rBottom() *= chunksize;
        // result.rRight() += chunksize;
        // result.rBottom() += chunksize;

        result.setLeft(result.left() * chunksize);
        result.setTop(result.top() * chunksize);
        result.setRight(result.right() * chunksize);
        result.setBottom(result.bottom() * chunksize);
        result.setRight(result.right() + chunksize);
        result.setBottom(result.bottom() + chunksize);
    }

    return result;
}

/*!
    Redraws the area \a inarea of the KtlQCanvas.
 */
void KtlQCanvas::drawChanges(const QRect &inarea)
{
    QRect area = inarea.intersect(m_size);

    KtlQCanvasClusterizer clusters(maxclusters);

    int mx = toChunkScaling(area.x() + area.width() + chunksize);
    int my = toChunkScaling(area.y() + area.height() + chunksize);
    if (mx > m_chunkSize.right())
        mx = m_chunkSize.right();
    if (my > m_chunkSize.bottom())
        my = m_chunkSize.bottom();

    int x = toChunkScaling(area.x());
    while (x < mx) {
        int y = toChunkScaling(area.y());
        while (y < my) {
            KtlQCanvasChunk &ch = chunk(x, y);
            if (ch.hasChanged())
                clusters.add(x, y);
            y++;
        }
        x++;
    }

    for (int i = 0; i < clusters.clusters(); i++) {
        QRect elarea = clusters[i];
        elarea.setRect(elarea.left() * chunksize, elarea.top() * chunksize, elarea.width() * chunksize, elarea.height() * chunksize);
        drawCanvasArea(elarea, nullptr, /*true*/ false);
    }
}

/*!
    Paints all canvas items that are in the area \a clip to \a
    painter, using double-buffering if \a dbuf is true.

    e.g. to print the canvas to a printer:
    \code
    QPrinter pr;
    if ( pr.setup() ) {
    QPainter p(&pr);        // this code is in a comment block
    canvas.drawArea( canvas.rect(), &p );
}
    \endcode
 */
void KtlQCanvas::drawArea(const QRect &clip, QPainter *painter)
{
    if (painter)
        drawCanvasArea(clip, painter, false);
}

void KtlQCanvas::drawCanvasArea(const QRect &inarea, QPainter *p, bool double_buffer /* 2018.03.11 - always false */)
{
    QRect area = inarea.intersect(m_size);

    if (!m_viewList.first() && !p)
        return; // Nothing to do.

    int lx = toChunkScaling(area.x());
    int ly = toChunkScaling(area.y());
    int mx = toChunkScaling(area.right());
    int my = toChunkScaling(area.bottom());
    if (mx >= m_chunkSize.right())
        mx = m_chunkSize.right() - 1;
    if (my >= m_chunkSize.bottom())
        my = m_chunkSize.bottom() - 1;

    // Stores the region within area that need to be drawn. It is relative
    // to area.topLeft()  (so as to keep within bounds of 16-bit XRegions)
    QRegion rgn;

    for (int x = lx; x <= mx; x++) {
        for (int y = ly; y <= my; y++) {
            // Only reset change if all views updating, and
            // wholy within area. (conservative:  ignore entire boundary)
            //
            // Disable this to help debugging.
            //
            if (!p) {
                if (chunk(x, y).takeChange()) {
                    // ### should at least make bands
                    rgn |= QRegion(x * chunksize - area.x(), y * chunksize - area.y(), chunksize, chunksize);
                    // 					allvisible += *chunk(x,y).listPtr();
                    setNeedRedraw(chunk(x, y).listPtr());
                    // 					chunk(x,y).listPtr()->first()->m_bNeedRedraw = true;
                }
            } else {
                // 				allvisible += *chunk(x,y).listPtr();
                setNeedRedraw(chunk(x, y).listPtr());
            }
        }
    }
    // 	allvisible.sort();

#if 0 // 2018.03.11 - double buffer is always false
	if ( double_buffer ) {
		offscr = QPixmap(area.width(), area.height());
		if (p) offscr.x11SetScreen(p->device()->x11Screen());
	}
	if ( double_buffer && !offscr.isNull() ) {
		QPainter painter;
        const bool isSucces = painter.begin(&offscr);
        if (!isSucces) {
            qWarning() << Q_FUNC_INFO << " " << __LINE__ << " painter not active ";
        }
		painter.translate(-area.x(),-area.y());
		painter.setBrushOrigin(-area.x(),-area.y());

		if ( p ) {
			painter.setClipRect(QRect(0,0,area.width(),area.height()));
		} else {
			painter.setClipRegion(rgn);
		}
        if (!painter.isActive()) {
            qWarning() << Q_FUNC_INFO << " " << __LINE__ << " painter is not active";
        }

		drawBackground(painter,area);
// 		allvisible.drawUnique(painter);
		drawChangedItems( painter );
		drawForeground(painter,area);
		painter.end();

		if ( p ) {
			p->drawPixmap( area.x(), area.y(), offscr, 0, 0, area.width(), area.height() );
			return;
		}

	} else
#endif
    if (p) {
        drawBackground(*p, area);
        // 		allvisible.drawUnique(*p);
        drawChangedItems(*p);
        drawForeground(*p, area);
        return;
    }

    QPoint trtr; // keeps track of total translation of rgn

    trtr -= area.topLeft();

    for (QList<KtlQCanvasView *>::iterator itView = m_viewList.begin(); itView != m_viewList.end(); ++itView) {
        KtlQCanvasView *view = *itView;

        if (!view->worldMatrix().isIdentity())
            continue; // Cannot paint those here (see callers).

        // as of my testing, drawing below always fails, so just post for an update event to the widget
        view->viewport()->update();

#if 0
        //view->viewport()->setAttribute(Qt::WA_PaintOutsidePaintEvent, true); // note: remove this when possible
		//QPainter painter(view->viewport());
		QPainter painter;
        const bool isSuccess = painter.begin(view->viewport());
        static int paintSuccessCount = 0;
        static int paintFailCount = 0;
        if (!isSuccess) {
            //qWarning() << Q_FUNC_INFO << " on view " << view << " viewport " << view->viewport();
            qWarning() << Q_FUNC_INFO << " " << __LINE__ << " painter not active, applying workaround";
            // TODO fix this workaround for repainting: the painter would try to draw to the widget outside of a paint event,
            //  which is not expected to work. Thus this code just sends an update() to the widget, ensuring correct painting
            ++paintFailCount;
            qWarning() << Q_FUNC_INFO << " paint success: " << paintSuccessCount << ", fail: " << paintFailCount;
            view->viewport()->update();
            continue;
        } else {
            ++paintSuccessCount;
        }
		QPoint tr = view->contentsToViewport(area.topLeft());
		QPoint nrtr = view->contentsToViewport(QPoint(0,0)); // new translation
		QPoint rtr = nrtr - trtr; // extra translation of rgn
		trtr += rtr; // add to total
		
		if (double_buffer) {
			rgn.translate(rtr.x(),rtr.y());
			painter.setClipRegion(rgn);
			painter.drawPixmap(tr,offscr, QRect(QPoint(0,0),area.size()));
		} else {
			painter.translate(nrtr.x(),nrtr.y());
			rgn.translate(rtr.x(),rtr.y());
			painter.setClipRegion(rgn);
			drawBackground(painter,area);
// 			allvisible.drawUnique(painter);
			drawChangedItems( painter );
			drawForeground(painter,area);
			painter.translate(-nrtr.x(),-nrtr.y());
		}
#endif
    }
}

void KtlQCanvas::setNeedRedraw(const KtlQCanvasItemList *list)
{
    KtlQCanvasItemList::const_iterator end = list->end();
    for (KtlQCanvasItemList::const_iterator it = list->begin(); it != end; ++it)
        (*it)->setNeedRedraw(true);
}

void KtlQCanvas::drawChangedItems(QPainter &painter)
{
    SortedCanvasItems::iterator end = m_canvasItems.end();
    for (SortedCanvasItems::iterator it = m_canvasItems.begin(); it != end; ++it) {
        KtlQCanvasItem *i = it->second;
        if (i->needRedraw()) {
            i->draw(painter);
            i->setNeedRedraw(false);
        }
    }
}

/*!
    \internal
    This method to informs the KtlQCanvas that a given chunk is
    `dirty' and needs to be redrawn in the next Update.

    (\a x,\a y) is a chunk location.

    The sprite classes call this. Any new derived class of KtlQCanvasItem
    must do so too. SetChangedChunkContaining can be used instead.
 */
void KtlQCanvas::setChangedChunk(int x, int y)
{
    if (validChunk(x, y)) {
        KtlQCanvasChunk &ch = chunk(x, y);
        ch.change();
    }
}

/*!
    \internal
    This method to informs the KtlQCanvas that the chunk containing a given
    pixel is `dirty' and needs to be redrawn in the next Update.

    (\a x,\a y) is a pixel location.

    The item classes call this. Any new derived class of KtlQCanvasItem must
    do so too. SetChangedChunk can be used instead.
 */
void KtlQCanvas::setChangedChunkContaining(int x, int y)
{
    if (onCanvas(x, y)) {
        KtlQCanvasChunk &chunk = chunkContaining(x, y);
        chunk.change();
    }
}

/*!
    \internal
    This method adds the KtlQCanvasItem \a g to the list of those which need to be
    drawn if the given chunk at location ( \a x, \a y ) is redrawn. Like
    SetChangedChunk and SetChangedChunkContaining, this method marks the
    chunk as `dirty'.
 */
void KtlQCanvas::addItemToChunk(KtlQCanvasItem *g, int x, int y)
{
    if (validChunk(x, y)) {
        chunk(x, y).add(g);
    }
}

/*!
    \internal
    This method removes the KtlQCanvasItem \a g from the list of those which need to
    be drawn if the given chunk at location ( \a x, \a y ) is redrawn. Like
    SetChangedChunk and SetChangedChunkContaining, this method marks the chunk
    as `dirty'.
 */
void KtlQCanvas::removeItemFromChunk(KtlQCanvasItem *g, int x, int y)
{
    if (validChunk(x, y)) {
        chunk(x, y).remove(g);
    }
}

/*!
    \internal
    This method adds the KtlQCanvasItem \a g to the list of those which need to be
    drawn if the chunk containing the given pixel ( \a x, \a y ) is redrawn. Like
    SetChangedChunk and SetChangedChunkContaining, this method marks the
    chunk as `dirty'.
 */
void KtlQCanvas::addItemToChunkContaining(KtlQCanvasItem *g, int x, int y)
{
    if (onCanvas(x, y)) {
        chunkContaining(x, y).add(g);
    }
}

/*!
    \internal
    This method removes the KtlQCanvasItem \a g from the list of those which need to
    be drawn if the chunk containing the given pixel ( \a x, \a y ) is redrawn.
    Like SetChangedChunk and SetChangedChunkContaining, this method
    marks the chunk as `dirty'.
 */
void KtlQCanvas::removeItemFromChunkContaining(KtlQCanvasItem *g, int x, int y)
{
    if (onCanvas(x, y)) {
        chunkContaining(x, y).remove(g);
    }
}

/*!
    Returns the color set by setBackgroundColor(). By default, this is
    white.

    This function is not a reimplementation of
    QWidget::backgroundColor() (KtlQCanvas is not a subclass of QWidget),
    but all QCanvasViews that are viewing the canvas will set their
    backgrounds to this color.

    \sa setBackgroundColor(), backgroundPixmap()
 */
QColor KtlQCanvas::backgroundColor() const
{
    return bgcolor;
}

/*!
    Sets the solid background to be the color \a c.

    \sa backgroundColor(), setBackgroundPixmap(), setTiles()
 */
void KtlQCanvas::setBackgroundColor(const QColor &c)
{
    if (bgcolor != c) {
        bgcolor = c;
        for (QList<KtlQCanvasView *>::iterator itView = m_viewList.begin(); itView != m_viewList.end(); ++itView) {
            KtlQCanvasView *view = *itView;

            /* XXX this doesn't look right. Shouldn't this
                be more like setBackgroundPixmap? : Ian */
            // view->viewport()->setEraseColor( bgcolor ); // 2018.11.21
            QWidget *viewportWidg = view->viewport();
            QPalette palette;
            palette.setColor(viewportWidg->backgroundRole(), bgcolor);
            viewportWidg->setPalette(palette);
        }
        setAllChanged();
    }
}

/*!
    Returns the pixmap set by setBackgroundPixmap(). By default,
    this is a null pixmap.

    \sa setBackgroundPixmap(), backgroundColor()
 */
QPixmap KtlQCanvas::backgroundPixmap() const
{
    return pm;
}

/*!
    Sets the solid background to be the pixmap \a p repeated as
    necessary to cover the entire canvas.

    \sa backgroundPixmap(), setBackgroundColor(), setTiles()
 */
void KtlQCanvas::setBackgroundPixmap(const QPixmap &p)
{
    setTiles(p, 1, 1, p.width(), p.height());

    for (QList<KtlQCanvasView *>::iterator itView = m_viewList.begin(); itView != m_viewList.end(); ++itView) {
        (*itView)->updateContents();
    }
    // KtlQCanvasView* view = m_viewList.first();    // 2018.08.14 - see above
    // while ( view != 0 ) {
    //	view->updateContents();
    //	view = m_viewList.next();
    //}
}

/*!
    This virtual function is called for all updates of the canvas. It
    renders any background graphics using the painter \a painter, in
    the area \a clip. If the canvas has a background pixmap or a tiled
    background, that graphic is used, otherwise the canvas is cleared
    using the background color.

    If the graphics for an area change, you must explicitly call
    setChanged(const QRect&) for the result to be visible when
    update() is next called.

    \sa setBackgroundColor(), setBackgroundPixmap(), setTiles()
 */
void KtlQCanvas::drawBackground(QPainter &painter, const QRect &clip)
{
    painter.fillRect(clip, Qt::white);

    if (pm.isNull())
        painter.fillRect(clip, bgcolor);

    else if (!grid) {
        for (int x = clip.x() / pm.width(); x < (clip.x() + clip.width() + pm.width() - 1) / pm.width(); x++) {
            for (int y = clip.y() / pm.height(); y < (clip.y() + clip.height() + pm.height() - 1) / pm.height(); y++) {
                painter.drawPixmap(x * pm.width(), y * pm.height(), pm);
            }
        }
    } else {
        const int x1 = roundDown(clip.left(), tilew);
        int x2 = roundDown(clip.right(), tilew);
        const int y1 = roundDown(clip.top(), tileh);
        int y2 = roundDown(clip.bottom(), tileh);

        const int roww = pm.width() / tilew;

        for (int j = y1; j <= y2; j++) {
            int tv = tilesVertically();
            int jj = ((j % tv) + tv) % tv;
            for (int i = x1; i <= x2; i++) {
                int th = tilesHorizontally();
                int ii = ((i % th) + th) % th;
                int t = tile(ii, jj);
                int tx = t % roww;
                int ty = t / roww;
                painter.drawPixmap(i * tilew, j * tileh, pm, tx * tilew, ty * tileh, tilew, tileh);
            }
        }
    }
}

void KtlQCanvas::drawForeground(QPainter &painter, const QRect &clip)
{
    if (debug_redraw_areas) {
        painter.setPen(Qt::red);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(clip);
    }
}

void KtlQCanvas::setTiles(QPixmap p, int h, int v, int tilewidth, int tileheight)
{
    if (!p.isNull() && (!tilewidth || !tileheight || p.width() % tilewidth != 0 || p.height() % tileheight != 0))
        return;

    htiles = h;
    vtiles = v;
    delete[] grid;
    pm = p;
    if (h && v && !p.isNull()) {
        grid = new ushort[h * v];
        memset(grid, 0, h * v * sizeof(ushort));
        tilew = tilewidth;
        tileh = tileheight;
    } else {
        grid = nullptr;
    }
    if (h + v > 10) {
        int s = scm(tilewidth, tileheight);
        retune(s < 128 ? s : std::max(tilewidth, tileheight));
    }
    setAllChanged();
}

void KtlQCanvas::setTile(int x, int y, int tilenum)
{
    ushort &t = grid[x + y * htiles];
    if (t != tilenum) {
        t = tilenum;
        if (tilew == tileh && tilew == chunksize)
            setChangedChunk(x, y); // common case
        else
            setChanged(QRect(x * tilew, y * tileh, tilew, tileh));
    }
}

KtlQCanvasItemList KtlQCanvas::collisions(const QPoint &p) /* const */
{
    return collisions(QRect(p, QSize(1, 1)));
}

KtlQCanvasItemList KtlQCanvas::collisions(const QRect &r) /* const */
{
    KtlQCanvasRectangle *i = new KtlQCanvasRectangle(r, /*(KtlQCanvas*) */ this); // TODO verify here, why is crashing ?!
    i->setPen(QPen(Qt::NoPen));
    i->show(); // doesn't actually show, since we destroy it
    KtlQCanvasItemList l = i->collisions(true);
    delete i;
    l.sort();
    return l;
}

KtlQCanvasItemList KtlQCanvas::collisions(const QPolygon &chunklist, const KtlQCanvasItem *item, bool exact) const
{
    if (isCanvasDebugEnabled()) {
        qDebug() << Q_FUNC_INFO << " test item: " << item;
        for (SortedCanvasItems::const_iterator itIt = m_canvasItems.begin(); itIt != m_canvasItems.end(); ++itIt) {
            const KtlQCanvasItem *i = itIt->second;
            qDebug() << "   in canvas item: " << i;
        }
        qDebug() << "end canvas item list";
    }

    // Q3PtrDict<void> seen;
    QHash<KtlQCanvasItem *, bool> seen;
    KtlQCanvasItemList result;
    for (int i = 0; i < (int)chunklist.count(); i++) {
        int x = chunklist[i].x();
        int y = chunklist[i].y();
        if (validChunk(x, y)) {
            const KtlQCanvasItemList *l = chunk(x, y).listPtr();
            for (KtlQCanvasItemList::ConstIterator it = l->begin(); it != l->end(); ++it) {
                KtlQCanvasItem *g = *it;
                if (g != item) {
                    // if ( !seen.find(g) ) {
                    if (seen.find(g) == seen.end()) {
                        // seen.replace(g,(void*)1);
                        seen.take(g);
                        seen.insert(g, true);
                        // if ( !exact || item->collidesWith(g) )
                        //	result.append(g);
                        if (!exact) {
                            result.append(g);
                        }
                        if (isCanvasDebugEnabled()) {
                            qDebug() << "test collides " << item << " with " << g;
                        }
                        if (item->collidesWith(g)) {
                            result.append(g);
                        }
                    }
                }
            }
        }
    }
    return result;
}

KtlQCanvasView::KtlQCanvasView(QWidget *parent, const char *name, Qt::WindowFlags f)
    : KtlQ3ScrollView(parent, name, f /* |Qt::WResizeNoErase |Qt::WStaticContents */)
{
    setAttribute(Qt::WA_StaticContents);
    d = new KtlQCanvasViewData;
    viewing = nullptr;
    setCanvas(nullptr);
    connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(cMoving(int, int)));
}

KtlQCanvasView::KtlQCanvasView(KtlQCanvas *canvas, QWidget *parent, const char *name, Qt::WindowFlags f)
    : KtlQ3ScrollView(parent, name, f /* |Qt::WResizeNoErase |Qt::WA_StaticContents */)
{
    setAttribute(Qt::WA_StaticContents);
    d = new KtlQCanvasViewData;
    viewing = nullptr;
    setCanvas(canvas);

    connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(cMoving(int, int)));
}

KtlQCanvasView::~KtlQCanvasView()
{
    delete d;
    d = nullptr;
    setCanvas(nullptr);
}

void KtlQCanvasView::setCanvas(KtlQCanvas *canvas)
{
    if (viewing) {
        disconnect(viewing);
        viewing->removeView(this);
    }
    viewing = canvas;
    if (viewing) {
        connect(viewing, SIGNAL(resized()), this, SLOT(updateContentsSize()));
        viewing->addView(this);
    }
    if (d) // called by d'tor
        updateContentsSize();
}

const QMatrix &KtlQCanvasView::worldMatrix() const
{
    return d->xform;
}

const QMatrix &KtlQCanvasView::inverseWorldMatrix() const
{
    return d->ixform;
}

bool KtlQCanvasView::setWorldMatrix(const QMatrix &wm)
{
    bool ok = wm.isInvertible();
    if (ok) {
        d->xform = wm;
        d->ixform = wm.inverted();
        updateContentsSize();
        viewport()->update();
    }
    return ok;
}

void KtlQCanvasView::updateContentsSize()
{
    if (viewing) {
        QRect br;
        // 			br = d->xform.map(QRect(0,0,viewing->width(),viewing->height()));
        br = d->xform.mapRect(viewing->rect());

        if (br.width() < contentsWidth()) {
            QRect r(contentsToViewport(QPoint(br.width(), 0)), QSize(contentsWidth() - br.width(), contentsHeight()));
            // viewport()->erase(r); // 2015.11.25 - not recommended to directly repaint
            viewport()->update(r);
        }
        if (br.height() < contentsHeight()) {
            QRect r(contentsToViewport(QPoint(0, br.height())), QSize(contentsWidth(), contentsHeight() - br.height()));
            // viewport()->erase(r);  // 2015.11.25 - not recommended to directly repaint
            viewport()->update(r);
        }

        resizeContents(br.width(), br.height());
    } else {
        // viewport()->erase();  // 2015.11.25 - not recommended to directly repaint
        viewport()->update();
        resizeContents(1, 1);
    }
}

void KtlQCanvasView::cMoving(int x, int y)
{
    // A little kludge to smooth up repaints when scrolling
    int dx = x - contentsX();
    int dy = y - contentsY();
    d->repaint_from_moving = abs(dx) < width() / 8 && abs(dy) < height() / 8;
}

/*!
    Repaints part of the KtlQCanvas that the canvas view is showing
    starting at \a cx by \a cy, with a width of \a cw and a height of \a
    ch using the painter \a p.

    \warning When double buffering is enabled, drawContents() will
    not respect the current settings of the painter when setting up
    the painter for the double buffer (e.g., viewport() and
    window()). Also, be aware that KtlQCanvas::update() bypasses
    drawContents(), which means any reimplementation of
    drawContents() is not called.

    \sa setDoubleBuffering()
 */
void KtlQCanvasView::drawContents(QPainter *p, int cx, int cy, int cw, int ch)
{
    QRect r(cx, cy, cw, ch);
    r = r.normalized();

    if (viewing) {
        // viewing->drawViewArea(this,p,r,true);
        viewing->drawViewArea(this, p, r, /*!d->repaint_from_moving*/ false); /* 2018.03.11 - fix build for osx */
        d->repaint_from_moving = false;
    } else {
        p->eraseRect(r);
    }
}

/*!
    \reimp
    \internal

    (Implemented to get rid of a compiler warning.)
 */
void KtlQCanvasView::drawContents(QPainter *p)
{
    qDebug() << Q_FUNC_INFO << " called, although not expected";
    drawContents(p, 0, 0, width(), height());
}

/*!
    Suggests a size sufficient to view the entire canvas.
 */
QSize KtlQCanvasView::sizeHint() const
{
    if (!canvas())
        return KtlQ3ScrollView::sizeHint(); // TODO QT3
                                            // should maybe take transformations into account
    return (canvas()->size() + 2 * QSize(frameWidth(), frameWidth())).boundedTo(3 * QApplication::desktop()->size() / 4);
}
