//
// C++ Interface: canvas_private
//
// Description:
//
//
// Author: Alan Grimes <agrimes@speakeasy.net>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef CANVAS_PRIVATE_H
#define CANVAS_PRIVATE_H

#include "canvasitems.h"
#include "ktlq3polygonscanner.h"
#include <QBitmap>
#include <QImage>

class KtlQPolygonalProcessor
{
public:
    KtlQPolygonalProcessor(KtlQCanvas *c, const QPolygon &pa)
        : canvas(c)
    {
        QRect pixelbounds = pa.boundingRect();
        bounds.setLeft(canvas->toChunkScaling(pixelbounds.left()));
        bounds.setRight(canvas->toChunkScaling(pixelbounds.right()));
        bounds.setTop(canvas->toChunkScaling(pixelbounds.top()));
        bounds.setBottom(canvas->toChunkScaling(pixelbounds.bottom()));
        // bitmap = QImage(bounds.width(),bounds.height(),1,2,QImage::LittleEndian); // 2018.09.07 - convert to non-deprecated
        bitmap = QImage(bounds.width(), bounds.height(), QImage::Format_MonoLSB);
        pnt = 0;
        bitmap.fill(0);
    }

    inline void add(int x, int y)
    {
        if (pnt >= result.size()) {
            result.resize(pnt * 2 + 10);
        }
        result[pnt++] = QPoint(x + bounds.x(), y + bounds.y());
    }

    inline void addBits(int x1, int x2, uchar newbits, int xo, int yo)
    {
        for (int i = x1; i <= x2; i++)
            if (newbits & (1 << i))
                add(xo + i, yo);
    }

    void doSpans(int n, QPoint *pt, int *w)
    {
        for (int j = 0; j < n; j++) {
            int y = canvas->toChunkScaling(pt[j].y()) - bounds.y();
            uchar *l = bitmap.scanLine(y);
            int x = pt[j].x();
            int x1 = canvas->toChunkScaling(x) - bounds.x();
            int x2 = canvas->toChunkScaling(x + w[j]) - bounds.x();
            int x1q = x1 / 8;
            int x1r = x1 % 8;
            int x2q = x2 / 8;
            int x2r = x2 % 8;
            if (x1q == x2q) {
                uchar newbits = (~l[x1q]) & (((2 << (x2r - x1r)) - 1) << x1r);
                if (newbits) {
                    addBits(x1r, x2r, newbits, x1q * 8, y);
                    l[x1q] |= newbits;
                }
            } else {
                uchar newbits1 = (~l[x1q]) & (0xff << x1r);
                if (newbits1) {
                    addBits(x1r, 7, newbits1, x1q * 8, y);
                    l[x1q] |= newbits1;
                }
                for (int i = x1q + 1; i < x2q; i++) {
                    if (l[i] != 0xff) {
                        addBits(0, 7, ~l[i], i * 8, y);
                        l[i] = 0xff;
                    }
                }
                uchar newbits2 = (~l[x2q]) & (0xff >> (7 - x2r));
                if (newbits2) {
                    addBits(0, x2r, newbits2, x2q * 8, y);
                    l[x2q] |= newbits2;
                }
            }
        }
        result.resize(pnt);
    }

    QPolygon result;

private:
    int pnt;

    KtlQCanvas *canvas;
    QRect bounds;
    QImage bitmap;
};

class KtlQCanvasViewData
{
public:
    KtlQCanvasViewData()
        : repaint_from_moving(false)
    {
    }
    QMatrix xform;
    QMatrix ixform;
    bool repaint_from_moving;
};

class KtlQCanvasClusterizer
{
public:
    KtlQCanvasClusterizer(int maxclusters);
    ~KtlQCanvasClusterizer();

    void add(int x, int y); // 1x1 rectangle (point)
    void add(int x, int y, int w, int h);
    void add(const QRect &rect);

    void clear();
    int clusters()
    {
        return count;
    }
    const QRect &operator[](int i);

private:
    QRect *cluster;
    int count;
    const int maxcl;
};

class KtlQCanvasItemPtr
{
public:
    KtlQCanvasItemPtr()
        : ptr(nullptr)
    {
    }
    KtlQCanvasItemPtr(KtlQCanvasItem *p)
        : ptr(p)
    {
    }

    bool operator<=(const KtlQCanvasItemPtr &that) const
    {
        // Order same-z objects by identity.
        if (that.ptr->z() == ptr->z())
            return that.ptr <= ptr;
        return that.ptr->z() <= ptr->z();
    }

    bool operator<(const KtlQCanvasItemPtr &that) const
    {
        // Order same-z objects by identity.
        if (that.ptr->z() == ptr->z())
            return that.ptr < ptr;
        return that.ptr->z() < ptr->z();
    }

    bool operator>(const KtlQCanvasItemPtr &that) const
    {
        // Order same-z objects by identity.
        if (that.ptr->z() == ptr->z())
            return that.ptr > ptr;
        return that.ptr->z() > ptr->z();
    }

    bool operator==(const KtlQCanvasItemPtr &that) const
    {
        return that.ptr == ptr;
    }

    operator KtlQCanvasItem *() const
    {
        return ptr;
    }

private:
    KtlQCanvasItem *ptr;
};

class KtlQCanvasChunk
{
public:
    KtlQCanvasChunk()
        : changed(true)
    {
    }
    // Other code assumes lists are not deleted. Assignment is also
    // done on ChunkRecs. So don't add that sort of thing here.

    void sort()
    {
        list.sort();
    }

    const KtlQCanvasItemList *listPtr() const
    {
        return &list;
    }

    void add(KtlQCanvasItem *item)
    {
        list.prepend(item);
        changed = true;
    }

    void remove(KtlQCanvasItem *item)
    {
        list.removeAll(item);
        changed = true;
    }

    void change()
    {
        changed = true;
    }

    bool hasChanged() const
    {
        return changed;
    }

    bool takeChange()
    {
        bool y = changed;
        changed = false;
        return y;
    }

private:
    KtlQCanvasItemList list;
    bool changed;
};

class KtlQCanvasPolygonScanner : public KtlQ3PolygonScanner
{
    KtlQPolygonalProcessor &processor;

public:
    KtlQCanvasPolygonScanner(KtlQPolygonalProcessor &p)
        : processor(p)
    {
    }
    void processSpans(int n, QPoint *point, int *width) override
    {
        processor.doSpans(n, point, width);
    }
};

// lesser-used data in canvas item, plus room for extension.
// Be careful adding to this - check all usages.
class KtlQCanvasItemExtra
{
    KtlQCanvasItemExtra() /* : vx(0.0), vy(0.0) */
    {
    }
    // double vx,vy; // 2017.10.01 - commented unused members
    friend class KtlQCanvasItem;
};

#endif
