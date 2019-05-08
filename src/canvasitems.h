/***************************************************************************
 *   Copyright (C) 1999-2005 Trolltech AS                                  *
 *   Copyright (C) 2006 David Saxton <david@bluehaze.org>                  *
 *                                                                         *
 *   This file may be distributed and/or modified under the terms of the   *
 *   GNU General Public License version 2 as published by the Free         *
 *   Software Foundation                                                   *
 ***************************************************************************/

#ifndef KTL_CANVASITEMS_H_
#define KTL_CANVASITEMS_H_

#include "canvasitemlist.h"

#include <qbrush.h>
#include <qobject.h>
#include <qpen.h>
#include <qpolygon.h>
#include <qrect.h>

class QPainter;

class KtlQCanvasPolygonalItem;
class KtlQCanvasRectangle;
class KtlQCanvasPolygon;
class KtlQCanvasEllipse;
class KtlQCanvasLine;
class KtlQCanvasChunk;
class KtlQCanvas;
class KtlQCanvasItem;
class KtlQCanvasView;

class KtlQCanvasItemExtra;

class KtlQCanvasItem : public QObject
{
    Q_OBJECT
    public:
        KtlQCanvasItem(KtlQCanvas* canvas);
        virtual ~KtlQCanvasItem();

        double x() const { return myx; }
        double y() const { return myy; }
        double z() const { return myz; } // (depth)

        virtual void moveBy(double const dx, double const dy);
        void move(double const x, double const y);
        void setX(double a) { move(a,y()); }
        void setY(double a) { move(x(),a); }
        void setZ(double a);

        virtual bool collidesWith( const KtlQCanvasItem* ) const=0;

        KtlQCanvasItemList collisions(const bool exact /* NO DEFAULT */ ) const;

        virtual void setCanvas(KtlQCanvas*);

        virtual void draw(QPainter&)=0;

        void show();
        void hide();

        virtual void setVisible(bool yes);
        bool isVisible() const { return vis; }
        virtual void setSelected(const bool yes);
        bool isSelected() const { return sel; }

        virtual QRect boundingRect() const=0;

        KtlQCanvas* canvas() const { return cnv; }

        virtual bool collidesWith( const KtlQCanvasPolygonalItem*,
                       const KtlQCanvasRectangle*,
                       const KtlQCanvasEllipse* ) const = 0;

        bool needRedraw() const { return m_bNeedRedraw; }
        void setNeedRedraw( const bool needRedraw ) { m_bNeedRedraw = needRedraw; }

    protected:
        void update() { changeChunks(); }

        virtual QPolygon chunks() const;
        virtual void addToChunks();
        virtual void removeFromChunks();
        virtual void changeChunks();

        bool val;
        double myx,myy,myz;

    private:
        KtlQCanvas* cnv;
        static KtlQCanvas* current_canvas;
        KtlQCanvasItemExtra *ext;
        KtlQCanvasItemExtra& extra();
        bool m_bNeedRedraw;
        bool vis;
        bool sel;

};


class KtlQPolygonalProcessor;

class KtlQCanvasPolygonalItem : public KtlQCanvasItem
{
    public:
        KtlQCanvasPolygonalItem(KtlQCanvas* canvas);
        virtual ~KtlQCanvasPolygonalItem();

        bool collidesWith( const KtlQCanvasItem* ) const override;

        virtual void setPen( const QPen & p );
        virtual void setBrush( const QBrush & b );

        QPen pen() const
        { return pn; }
        QBrush brush() const
        { return br; }

        virtual QPolygon areaPoints() const=0;
        QRect boundingRect() const override;

    protected:
        void draw(QPainter &) override;
        virtual void drawShape(QPainter &) = 0;

        bool winding() const;
        void setWinding(bool);

        void invalidate();
        bool isValid() const
        { return val; }

    private:
        void scanPolygon( const QPolygon& pa, int winding,
                    KtlQPolygonalProcessor& process ) const;

        QPolygon chunks() const override;

        bool collidesWith( const KtlQCanvasPolygonalItem*,
                   const KtlQCanvasRectangle*,
                   const KtlQCanvasEllipse* ) const override;

        QBrush br;
        QPen pn;
        bool wind;
};


class KtlQCanvasRectangle : public KtlQCanvasPolygonalItem
{
    public:
        KtlQCanvasRectangle(KtlQCanvas* canvas);
        KtlQCanvasRectangle(const QRect&, KtlQCanvas* canvas);
        KtlQCanvasRectangle(int x, int y, int width, int height, KtlQCanvas* canvas);

        ~KtlQCanvasRectangle();

        int width() const;
        int height() const;
        void setSize(const int w, const int h);
        QSize size() const
        { return QSize(w,h); }
        QPolygon areaPoints() const override;
        QRect rect() const
        { return QRect(int(x()),int(y()),w,h); }

        bool collidesWith( const KtlQCanvasItem* ) const override;

    protected:
        void drawShape(QPainter &) override;
        QPolygon chunks() const override;

    private:
        bool collidesWith( const KtlQCanvasPolygonalItem*,
                   const KtlQCanvasRectangle*,
                   const KtlQCanvasEllipse* ) const override;

        int w, h;
};


class KtlQCanvasPolygon : public KtlQCanvasPolygonalItem
{
    public:
        KtlQCanvasPolygon(KtlQCanvas* canvas);
        ~KtlQCanvasPolygon();
        void setPoints(QPolygon);
        QPolygon points() const;
        void moveBy(double dx, double dy) override;

        QPolygon areaPoints() const override;

    protected:
        void drawShape(QPainter &) override;
        // TODO FIXME guarts are added for debugging memory corruption (poly takes non-pointer values)
        int guardBef[10];
        QPolygon *poly;
        int guardAft[10];
};


class KtlQCanvasLine : public KtlQCanvasPolygonalItem
{
    public:
        KtlQCanvasLine(KtlQCanvas* canvas);
        ~KtlQCanvasLine();
        void setPoints(int x1, int y1, int x2, int y2);

        QPoint startPoint() const
        { return QPoint(x1,y1); }
        QPoint endPoint() const
        { return QPoint(x2,y2); }

        void setPen( const QPen & p ) override;
        void moveBy(double dx, double dy) override;

    protected:
        void drawShape(QPainter &) override;
        QPolygon areaPoints() const override;

    private:
        int x1,y1,x2,y2;
};


class KtlQCanvasEllipse : public KtlQCanvasPolygonalItem
{

    public:
        KtlQCanvasEllipse( KtlQCanvas* canvas );
        KtlQCanvasEllipse( int width, int height, KtlQCanvas* canvas );
        KtlQCanvasEllipse( int width, int height, int startangle, int angle,
                    KtlQCanvas* canvas );

        ~KtlQCanvasEllipse();

        int width() const;
        int height() const;
        void setSize(int w, int h);
        void setAngles(int start, int length);
        int angleStart() const { return a1; }
        int angleLength() const { return a2; }
        QPolygon areaPoints() const override;

        bool collidesWith( const KtlQCanvasItem* ) const override;

    protected:
        void drawShape(QPainter &) override;

    private:
        bool collidesWith( const KtlQCanvasPolygonalItem*,
                   const KtlQCanvasRectangle*,
                   const KtlQCanvasEllipse* ) const override;
        int w, h;
        int a1, a2;
};


#endif // KTL_CANVASITEMS_H_
