/***************************************************************************
 *   Copyright (C) 1999-2005 Trolltech AS                                  *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This file may be distributed and/or modified under the terms of the   *
 *   GNU General Public License version 2 as published by the Free         *
 *   Software Foundation                                                   *
 ***************************************************************************/

#ifndef QCANVAS_H
#define QCANVAS_H

#include <map>

#include "ktlqt3support/ktlq3scrollview.h"
#include <QPixmap>
// #include "q3ptrlist.h"
#include <QBrush>
#include <QList>
#include <QPen>
// #include "q3pointarray.h" // 2018.08.14

#include "canvasitemlist.h"

class KtlQCanvasView;
class KtlQCanvasChunk;

class KtlQCanvas : public QObject
{
    Q_OBJECT
public:
    KtlQCanvas(QObject *parent = nullptr);
    KtlQCanvas(const int w, const int h);
    KtlQCanvas(QPixmap p, int h, int v, int tilewidth, int tileheight);

    ~KtlQCanvas() override;

    virtual void setTiles(QPixmap tiles, int h, int v, int tilewidth, int tileheight);
    virtual void setBackgroundPixmap(const QPixmap &p);
    QPixmap backgroundPixmap() const;

    virtual void setBackgroundColor(const QColor &c);
    QColor backgroundColor() const;

    virtual void setTile(int x, int y, int tilenum);
    int tile(int x, int y) const
    {
        return grid[x + y * htiles];
    }

    int tilesHorizontally() const
    {
        return htiles;
    }
    int tilesVertically() const
    {
        return vtiles;
    }

    int tileWidth() const
    {
        return tilew;
    }
    int tileHeight() const
    {
        return tileh;
    }

    virtual void resize(const QRect &newSize);
    int width() const
    {
        return size().width();
    }
    int height() const
    {
        return size().height();
    }
    QSize size() const
    {
        return m_size.size();
    }
    QRect rect() const
    {
        return m_size;
    }
    bool onCanvas(const int x, const int y) const
    {
        return onCanvas(QPoint(x, y));
    }
    bool onCanvas(const QPoint &p) const
    {
        return m_size.contains(p);
    }
    bool validChunk(const int x, const int y) const
    {
        return validChunk(QPoint(x, y));
    }
    bool validChunk(const QPoint &p) const
    {
        return m_chunkSize.contains(p);
    }

    int chunkSize() const
    {
        return chunksize;
    }
    virtual void retune(int chunksize, int maxclusters = 100);
    virtual void setChangedChunk(int i, int j);
    virtual void setChangedChunkContaining(int x, int y);
    virtual void setAllChanged();
    virtual void setChanged(const QRect &area);
    virtual void setUnchanged(const QRect &area);

    // These call setChangedChunk.
    void addItemToChunk(KtlQCanvasItem *, int i, int j);
    void removeItemFromChunk(KtlQCanvasItem *, int i, int j);
    void addItemToChunkContaining(KtlQCanvasItem *, int x, int y);
    void removeItemFromChunkContaining(KtlQCanvasItem *, int x, int y);

    KtlQCanvasItemList allItems();
    KtlQCanvasItemList collisions(const QPoint &) /* const */;
    KtlQCanvasItemList collisions(const QRect &) /* const */;
    KtlQCanvasItemList collisions(const QPolygon &pa, const KtlQCanvasItem *item, bool exact) const;

    void drawArea(const QRect &, QPainter *p);

    // These are for KtlQCanvasView to call
    virtual void addView(KtlQCanvasView *);
    virtual void removeView(KtlQCanvasView *);
    void drawCanvasArea(const QRect &, QPainter *p, bool double_buffer);
    void drawViewArea(KtlQCanvasView *view, QPainter *p, const QRect &r, bool dbuf);

    // These are for KtlQCanvasItem to call
    virtual void addItem(KtlQCanvasItem *);
    virtual void removeItem(const KtlQCanvasItem *);

    virtual void setUpdatePeriod(int ms);
    int toChunkScaling(int x) const;

signals:
    void resized();

public slots:
    virtual void advance();
    virtual void update();

protected:
    virtual void drawBackground(QPainter &, const QRect &area);
    virtual void drawForeground(QPainter &, const QRect &area);

private:
    void init(int w, int h, int chunksze = 16, int maxclust = 100);
    void init(const QRect &r, int chunksze = 16, int maxclust = 100);
    void initChunkSize(const QRect &s);

    KtlQCanvasChunk &chunk(int i, int j) const;
    KtlQCanvasChunk &chunkContaining(int x, int y) const;

    QRect changeBounds(const QRect &inarea);
    void drawChanges(const QRect &inarea);
    void drawChangedItems(QPainter &painter);
    void setNeedRedraw(const KtlQCanvasItemList *list);

    QPixmap offscr;
    int chunksize;
    int maxclusters;
    QRect m_size;
    QRect m_chunkSize;
    KtlQCanvasChunk *chunks;

    SortedCanvasItems m_canvasItems;
    QList<KtlQCanvasView *> m_viewList;

    void initTiles(QPixmap p, int h, int v, int tilewidth, int tileheight);
    ushort *grid;
    ushort htiles;
    ushort vtiles;
    ushort tilew;
    ushort tileh;
    bool oneone;
    QPixmap pm;
    QTimer *update_timer;
    QColor bgcolor;
    bool debug_redraw_areas;

    friend void qt_unview(KtlQCanvas *c);

    KtlQCanvas(const KtlQCanvas &);
    KtlQCanvas &operator=(const KtlQCanvas &);
};

class KtlQCanvasViewData;

class KtlQCanvasView : public KtlQ3ScrollView
{
    Q_OBJECT
public:
    KtlQCanvasView(QWidget *parent = nullptr, Qt::WindowFlags f = {}); // 2018.08.15 - unused?
    KtlQCanvasView(KtlQCanvas *viewing, QWidget *parent = nullptr, Qt::WindowFlags f = {});
    ~KtlQCanvasView() override;

    KtlQCanvas *canvas() const
    {
        return viewing;
    }
    void setCanvas(KtlQCanvas *v);

    const QTransform &worldMatrix() const;
    const QTransform &inverseWorldMatrix() const;
    bool setWorldTransform(const QTransform &);

protected:
    /** overrides KtlQ3ScrollView::drawContents() */ // override paintEvent?
    void drawContents(QPainter *, int cx, int cy, int cw, int ch) override;
    QSize sizeHint() const override;

private:
    void drawContents(QPainter *) override;
    KtlQCanvas *viewing;
    KtlQCanvasViewData *d;
    friend void qt_unview(KtlQCanvas *c);
    KtlQCanvasView(const KtlQCanvasView &);
    KtlQCanvasView &operator=(const KtlQCanvasView &);

private slots:
    void cMoving(int, int);
    void updateContentsSize();
};

#endif // QCANVAS_H
